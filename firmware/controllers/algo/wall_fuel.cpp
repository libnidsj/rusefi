/*
 * @file wall_fuel.cpp
 *
 * @author Matthew Kennedy
 */

#include "pch.h"
#include "wall_fuel.h"
#include "flash_main.h"
#include "table_helper.h"

void WallFuel::resetWF() {
	wallFuel = 0;
}

float WallFuel::adjust(float desiredMassGrams) {
	invocationCounter++;
	if (std::isnan(desiredMassGrams)) {
		return desiredMassGrams;
	}

	ScopePerf perf(PE::WallFuelAdjust);

	/*
		this math is based on
				SAE 810494 by C. F. Aquino
				SAE 1999-01-0553 by Peter J Maloney

		M_cmd = commanded fuel mass (output of this function)
		desiredMassGrams = desired fuel mass (input to this function)
		fuelFilmMass = fuel film mass (how much is currently on the wall)

		First we compute how much fuel to command, by accounting for
		a) how much fuel will evaporate from the walls, entering the air
		b) how much fuel from the injector will hit the walls, being deposited

		Next, we compute how much fuel will be deposited on the walls.  The net
		effect of these two steps is computed (some leaves walls, some is deposited)
		and stored back in fuelFilmMass.

		alpha describes the amount of fuel that REMAINS on the wall per cycle.
		It is computed as a function of the evaporation time constant (tau) and
		the time the fuel spent on the wall this cycle, (recriprocal RPM).

		beta describes the amount of fuel that hits the wall.
	*/

	// If disabled, pass value through
	if (!engine->module<WallFuelController>()->getEnable()) {
		return desiredMassGrams;
	}

	float alpha = engine->module<WallFuelController>()->getAlpha();
	float beta = engine->module<WallFuelController>()->getBeta();

	float fuelFilmMass = wallFuel;
	float M_cmd = (desiredMassGrams - (1 - alpha) * fuelFilmMass) / (1 - beta);

	// We can't inject a negative amount of fuel
	// If this goes below zero we will be over-fueling slightly,
	// but that's ok.
	if (M_cmd <= 0) {
		M_cmd = 0;
	}

	// remainder on walls from last time + new from this time
	float fuelFilmMassNext = alpha * fuelFilmMass + beta * M_cmd;

	wallFuel = fuelFilmMassNext;
	wallFuelCorrection = M_cmd - desiredMassGrams;
	return M_cmd;
}

float WallFuel::getWallFuel() const {
	return wallFuel;
}

float WallFuelController::computeTau() const {
	if (!engineConfiguration->complexWallModel) {
		return engineConfiguration->wwaeTau;
	}

	// Default to normal operating temperature in case of
	// CLT failure, this is not critical to get perfect
	float clt = Sensor::get(SensorType::Clt).value_or(90);

	float tau = interpolate2d(
		clt,
		config->wwCltBins,
		config->wwTauCltValues
	);

	// If you have a MAP sensor, apply MAP x RPM correction using 3D table
	if (Sensor::hasSensor(SensorType::Map)) {
		auto map = Sensor::get(SensorType::Map).value_or(60);
		auto rpm = Sensor::getOrZero(SensorType::Rpm);

		tau *= interpolate3d(
			config->wwTauMapRpmValues,
			config->wwCorrectionMapBins, map,
			config->wwCorrectionRpmBins, rpm
		);

		// Apply adaptive correction table if directional corrections are enabled
		if (engineConfiguration->wwEnableAdaptiveLearning) {
			float tauCorr = interpolate3d(
				config->wwTauCorrection,
				config->wwCorrectionMapBins, map,
				config->wwCorrectionRpmBins, rpm
			);
			tau *= tauCorr;
		}
	}

	return tau;
}

float WallFuelController::computeBeta() const {
	if (!engineConfiguration->complexWallModel) {
		return engineConfiguration->wwaeBeta;
	}

	// Default to normal operating temperature in case of
	// CLT failure, this is not critical to get perfect
	float clt = Sensor::get(SensorType::Clt).value_or(90);

	float beta = interpolate2d(
		clt,
		config->wwCltBins,
		config->wwBetaCltValues
	);

	// If you have a MAP sensor, apply MAP x RPM correction using 3D table
	if (Sensor::hasSensor(SensorType::Map)) {
		auto map = Sensor::get(SensorType::Map).value_or(60);
		auto rpm = Sensor::getOrZero(SensorType::Rpm);

		beta *= interpolate3d(
			config->wwBetaMapRpmValues,
			config->wwCorrectionMapBins, map,
			config->wwCorrectionRpmBins, rpm
		);

		// Apply adaptive correction table if directional corrections are enabled
		if (engineConfiguration->wwEnableAdaptiveLearning) {
			float betaCorr = interpolate3d(
				config->wwBetaCorrection,
				config->wwCorrectionMapBins, map,
				config->wwCorrectionRpmBins, rpm
			);
			beta *= betaCorr;
		}
	}

	// Clamp to 0..1 (you can't have more than 100% of the fuel hit the wall!)
	return clampF(0, beta, 1);
}

void WallFuelController::onFastCallback() {
	// disable wall wetting cranking
	// TODO: is this correct? Why not correct for cranking?
	if (engine->rpmCalculator.isCranking()) {
		m_enable = false;
		return;
	}
	
	float tau = computeTau();
	float beta = computeBeta();

	// if tau or beta is really small, we get div/0.
	// you probably meant to disable wwae.
	if (tau < 0.001f || beta < 0.01f) {
		m_enable = false;
		return;
	}
	
	auto rpm = Sensor::getOrZero(SensorType::Rpm);

	// Ignore low RPM - additional protection against division by zero
	if (rpm < 100) {
		m_enable = false;
		return;
	}

	// Additional protection against division by zero in alpha calculation
	float rpmTauProduct = rpm * tau;
	if (rpmTauProduct < 0.001f) {
		m_enable = false;
		return;
	}

	float alpha = expf_taylor(-120 / rpmTauProduct);

	// If beta is larger than alpha, the system is underdamped.
	// For reasonable values {tau, beta}, this should only be possible
	// at extremely low engine speeds (<300rpm ish)
	// Clamp beta to less than alpha.
	if (beta > alpha) {
		beta = alpha;
	}

	// Store parameters so the model can read them
	m_alpha = alpha;
	m_beta = beta;
	m_enable = true;

	// Adaptive learning state machine
	if (engineConfiguration->wwEnableAdaptiveLearning) {
		// Initialize state machine on first call (safe to call multiple times)
		static bool initialized = false;
		if (!initialized) {
			m_stateMachine.initialize();
			initialized = true;
		}
		m_stateMachine.update();
	}
}

void WallFuelController::onIgnitionStateChanged(bool ignitionOn) {
	m_ignitionState = ignitionOn;
	
	if (ignitionOn) {
		if (engineConfiguration->wwEnableAdaptiveLearning) {
			// Reset state machine when ignition turns on
			m_stateMachine.onIgnitionHandler(true);
			m_pendingSave = false;
		}
	} else {
		// When ignition turns off, schedule save if we have updates
		if (engineConfiguration->wwEnableAdaptiveLearning) {
			m_stateMachine.onIgnitionHandler(false);
			setNeedToWriteConfiguration();
			m_pendingSave = true;
			m_ignitionOffCallbacks = m_slowCallbackCounter; // Mark when ignition turned off
		}
	}
}

void WallFuelController::onSlowCallback() {
	m_slowCallbackCounter++;
	
	if (m_pendingSave && !m_ignitionState) {
		// Save after 5 seconds delay (1000 slow callbacks = 5s at 200Hz)
		if ((m_slowCallbackCounter - m_ignitionOffCallbacks) >= 1000) {
			setNeedToWriteConfiguration();
			m_pendingSave = false;
		}
	}
}

void WallFuelController::onActualFuelInjection(float injectedMass, int cylinderIndex) {
	(void)injectedMass;
	(void)cylinderIndex;
	if (!engineConfiguration->wwEnableAdaptiveLearning || !m_enable) {
		return;
	}
	
	float rpm = Sensor::getOrZero(SensorType::Rpm);
	auto clt = Sensor::get(SensorType::Clt);
	
	// Verificar condições mínimas
	if (rpm < 100 || !clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return;
	}
	
	// Esta função pode ser expandida no futuro para integração com sistema de adaptação
}

// ================ STATE MACHINE IMPLEMENTATION ================

void WwAdaptiveStateMachine::initialize() {
	m_currentState = WwAdaptiveState::IDLE;
	m_callbackCounter = 0;
	m_stateStartCallback = 0;
	
	// Initialize all data
	m_transientData.reset();
	m_gatheringData.reset();
	m_learningData.resetAdaptationCycle();
	m_correctionData.reset();
	m_loadData.reset();
	
	// Reset debug counters
	m_debugTransientsDetected = 0;
	m_debugCorrectionsApplied = 0;
	m_debugResetCount = 0;
	m_lastTransientMagnitude = 0;
}

void WwAdaptiveStateMachine::update() {
	// Increment callback counter
	m_callbackCounter++;
	
	// PERFORMANCE FIX: Global timeout to prevent stuck states
	uint32_t elapsed = getElapsedCallbacks();
	if (m_currentState != WwAdaptiveState::IDLE && elapsed > m_globalTimeoutCallbacks) {
		// Force reset if any state runs too long
		resetToIdle();
		return;
	}
	
	// Update shared data first
	updateSharedData();
	
	// Execute current state handler
	switch (m_currentState) {
		case WwAdaptiveState::IDLE:
			handleIdleState();
			break;
		case WwAdaptiveState::TRANSIENT_DETECTED:
			handleTransientDetectedState();
			break;
		case WwAdaptiveState::DELAY_LAMBDA:
			handleDelayLambdaState();
			break;
		case WwAdaptiveState::GATHERING_IMMEDIATE:
			handleGatheringImmediateState();
			break;
		case WwAdaptiveState::GATHERING_PROLONGED:
			handleGatheringProlongedState();
			break;
		case WwAdaptiveState::LEARNING_ANALYSIS:
			handleLearningAnalysisState();
			break;
		case WwAdaptiveState::APPLYING_CORRECTION:
			handleApplyingCorrectionState();
			break;
		case WwAdaptiveState::SAVING:
			handleSavingState();
			break;
	}
}

void WwAdaptiveStateMachine::updateSharedData() {
	// Get current load (MAP)
	auto mapSensor = Sensor::get(SensorType::Map);
	if (!mapSensor.Valid) {
		return;
	}
	float currentLoad = mapSensor.Value;
	
	// Update load derivative calculation
	m_loadData.loadBuffer[m_loadData.bufferIndex] = currentLoad;
	m_loadData.bufferIndex = (m_loadData.bufferIndex + 1) % WW_LOAD_BUFFER_SIZE;
	
	// Calculate load derivative (rate of change)
	int oldestIndex = m_loadData.bufferIndex;
	float oldLoad = m_loadData.loadBuffer[oldestIndex];
	
	// Calculate derivative in kPa/s (assuming onFastCallback runs at 200Hz)
	const float deltaTime = 0.005f * WW_LOAD_BUFFER_SIZE; // 8 samples * 5ms = 40ms
	m_loadData.loadDerivative = (currentLoad - oldLoad) / deltaTime;
	m_loadData.transientMagnitude = fabsf(m_loadData.loadDerivative);
}

void WwAdaptiveStateMachine::handleIdleState() {
	// Check for transient detection
	if (detectTransient()) {
		transitionTo(WwAdaptiveState::TRANSIENT_DETECTED);
	}
}

void WwAdaptiveStateMachine::handleTransientDetectedState() {
	// Capture transient conditions
	if (captureTransientConditions()) {
		transitionTo(WwAdaptiveState::DELAY_LAMBDA);
	} else {
		// Invalid transient, reset to idle
		resetToIdle();
	}
}

void WwAdaptiveStateMachine::handleDelayLambdaState() {
	uint32_t elapsed = getElapsedCallbacks();
	
	// Check for interruption by new transient
	// if (m_loadData.transientMagnitude > 30.0f) {
	// 	resetToIdle();
	// 	return;
	// }
	
	// Check delay timeout
	if (elapsed >= m_lambdaDelayCallbacks) {
		transitionTo(WwAdaptiveState::GATHERING_IMMEDIATE);
	}
}

void WwAdaptiveStateMachine::handleGatheringImmediateState() {
	uint32_t elapsed = getElapsedCallbacks();
	
	// Check for interruption by new transient
	// if (m_loadData.transientMagnitude > 70.0f) {
	// 	resetToIdle();
	// 	return;
	// }
	
	// Collect lambda data
	auto lambda = Sensor::get(SensorType::Lambda1);
	auto targetLambda = engine->fuelComputer.targetLambda;
	
	if (lambda.Valid && lambda.Value > 0.5f && lambda.Value < 3.5f &&
		targetLambda > 0.5f && targetLambda < 1.5f) {
		
		float lambdaError = lambda.Value - targetLambda;
		collectLambdaData(lambdaError);
	}
	
	// Check if immediate phase is complete
	if (elapsed >= m_immediatePhaseCallbacks) {
		transitionTo(WwAdaptiveState::GATHERING_PROLONGED);
	}
}

void WwAdaptiveStateMachine::handleGatheringProlongedState() {
	uint32_t elapsed = getElapsedCallbacks();
	
	// Collect lambda data
	auto lambda = Sensor::get(SensorType::Lambda1);
	auto targetLambda = engine->fuelComputer.targetLambda;
	
	if (lambda.Valid && lambda.Value > 0.5f && lambda.Value < 3.5f &&
		targetLambda > 0.5f && targetLambda < 1.5f) {
		
		float lambdaError = lambda.Value - targetLambda;
		collectLambdaData(lambdaError);
		
		// Capture final conditions during prolonged phase
		auto rpm = Sensor::getOrZero(SensorType::Rpm);
		auto map = Sensor::getOrZero(SensorType::Map);
		if (rpm > 100 && map > 10) {
			m_gatheringData.finalRpm = rpm;
			m_gatheringData.finalMap = map;
		}
	}
	
	// SIMPLIFIED TRANSITION LOGIC - Focus on data quality over complex timing
	bool hasImmediateData = (m_gatheringData.immediateBufferCount > 5); // Require minimum valid samples
	bool hasMinimalProlongedData = (m_gatheringData.prolongedBufferCount >= 10); // Minimum for trend detection
	
	// Simple fixed timing windows
	uint32_t minTimeCallbacks = 100; // 500ms minimum
	bool minTimeElapsed = elapsed >= minTimeCallbacks;
	
	// Simple target: 2 seconds of data collection (simplified from complex tau calculations)
	bool phaseComplete = (m_gatheringData.prolongedBufferCount >= m_gatheringData.prolongedBufferTarget) || 
						 (elapsed >= 400); // 2s = 400 callbacks
	
	// Reasonable timeout: 3 seconds maximum
	bool timeout = elapsed >= 600; // 3s = 600 callbacks
	
	// SIMPLIFIED TRANSITION LOGIC: Two main paths
	if (hasImmediateData && hasMinimalProlongedData && minTimeElapsed) {
		// Good case: have sufficient data for both beta and tau
		transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
	} else if (timeout) {
		// Timeout: proceed with whatever data we have, or reset if insufficient
		if (hasImmediateData || hasMinimalProlongedData) {
			transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
		} else {
			resetToIdle(); // No useful data collected
		}
	}
	// Otherwise continue collecting data
}

void WwAdaptiveStateMachine::handleLearningAnalysisState() {
	// Calculate average immediate lambda error for beta correction
	if (m_gatheringData.immediateBufferCount > 0) {
		float sum = 0.0f;
		int validSamples = 0;
		
		// PERFORMANCE FIX: WW_IMMEDIATE_BUFFER_SIZE is 40, which is acceptable for 200Hz
		int maxSamples = minI(m_gatheringData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
		for (int i = 0; i < maxSamples; i++) {
			float sample = m_gatheringData.immediateLambdaBuffer[i];
			if (!std::isnan(sample)) {
				sum += sample;
				validSamples++;
			}
		}
		
		if (validSamples > 0) {
			m_learningData.avgImmediateLambdaError = sum / validSamples;
			m_correctionData.betaCorrection = calculateBetaCorrection(m_learningData.avgImmediateLambdaError);
		}
	}
	
	// Calculate tau correction if we have prolonged data
	if (m_gatheringData.prolongedBufferCount > 10) {
		m_correctionData.tauCorrection = calculateTauCorrection();
	}
	
	// Set target conditions for correction
	m_correctionData.targetRpm = m_transientData.initialRpm;
	m_correctionData.targetMap = m_transientData.initialMap;
	
	// Update adaptation mode
	updateAdaptationMode();
	
	// Proceed to apply corrections
	transitionTo(WwAdaptiveState::APPLYING_CORRECTION);
}

void WwAdaptiveStateMachine::handleApplyingCorrectionState() {

	// Apply corrections to tables
	applyCorrectionToTable(
		m_correctionData.betaCorrection,
		m_correctionData.tauCorrection,
		m_correctionData.targetRpm,
		m_correctionData.targetMap
	);
	
	// Increment completed learning cycles
	m_learningData.completedLearningCycles++;
	m_debugCorrectionsApplied++;
	
	// Return to idle
	resetToIdle();
}

void WwAdaptiveStateMachine::handleSavingState() {
	// Save configuration
	setNeedToWriteConfiguration();
	
	// Return to idle
	transitionTo(WwAdaptiveState::IDLE);
}

void WwAdaptiveStateMachine::transitionTo(WwAdaptiveState newState) {
	m_currentState = newState;
	m_stateStartCallback = m_callbackCounter;
	
	// State entry actions
	switch (newState) {
		case WwAdaptiveState::IDLE:
			// No special actions for IDLE
			break;
			
		case WwAdaptiveState::TRANSIENT_DETECTED:
			// Reset gathering data for new transient
			m_gatheringData.reset();
			m_correctionData.reset();
			break;
			
		case WwAdaptiveState::DELAY_LAMBDA:
			// No special actions for DELAY_LAMBDA
			break;
			
		case WwAdaptiveState::GATHERING_IMMEDIATE:
			// No special actions for GATHERING_IMMEDIATE
			break;
			
		case WwAdaptiveState::GATHERING_PROLONGED: {
			// Calculate dynamic prolonged phase duration in callbacks
			float durationSeconds = WW_TAU_MULTIPLIER * m_transientData.currentTau;
			uint32_t durationCallbacks = (uint32_t)(durationSeconds * CALLBACK_FREQUENCY_HZ);
			m_gatheringData.prolongedBufferTarget = minI(durationCallbacks, WW_PROLONGED_BUFFER_SIZE_MAX);
			break;
		}
		
		case WwAdaptiveState::LEARNING_ANALYSIS:
			// No special actions for LEARNING_ANALYSIS
			break;
			
		case WwAdaptiveState::APPLYING_CORRECTION:
			// No special actions for APPLYING_CORRECTION
			break;
			
		case WwAdaptiveState::SAVING:
			// No special actions for SAVING
			break;
	}
}

void WwAdaptiveStateMachine::resetToIdle() {
	m_debugResetCount++;
	m_transientData.reset();
	m_gatheringData.reset();
	m_correctionData.reset();
	transitionTo(WwAdaptiveState::IDLE);
}

bool WwAdaptiveStateMachine::detectTransient() {
	// Check if adaptive learning is enabled
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return false;
	}
	
	// Check basic conditions
	auto clt = Sensor::get(SensorType::Clt);
	if (!clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return false;
	}
	
	auto rpm = Sensor::getOrZero(SensorType::Rpm);
	auto map = Sensor::getOrZero(SensorType::Map);
	
	// Additional validation
	if (rpm < 100 || map < 10) {
		return false;
	}
	
	// Check for transient threshold - improved validation and units
	float transientThreshold = engineConfiguration->wwMapThreshold; 
	if (transientThreshold <= 0) {
		transientThreshold = 40.0f; // Increased default threshold to 40 kPa/s for better detection
	}
	
	// Validate transient magnitude in kPa/s units
	m_lastTransientMagnitude = m_loadData.transientMagnitude;
	bool hasTransient = m_loadData.transientMagnitude > transientThreshold;
	
	// Additional validation: ensure transient magnitude is reasonable (< 500 kPa/s)
	if (m_loadData.transientMagnitude > 500.0f) {
		hasTransient = false; // Likely sensor noise or invalid reading
	}
	
	// Update debug counter when transient is detected
	if (hasTransient) {
		m_debugTransientsDetected++;
	}
	
	return hasTransient;
}

bool WwAdaptiveStateMachine::captureTransientConditions() {
	auto rpm = Sensor::getOrZero(SensorType::Rpm);
	auto map = Sensor::getOrZero(SensorType::Map);
	
	if (rpm < 100 || map < 10) {
		return false;
	}
	
	// Capture transient data
	m_transientData.isPositive = m_loadData.loadDerivative > 0;
	m_transientData.magnitude = m_loadData.transientMagnitude;
	m_transientData.startTime = m_callbackCounter * CALLBACK_PERIOD_SEC; // Convert to seconds for compatibility
	m_transientData.initialRpm = rpm;
	m_transientData.initialMap = map;
	
	// Use default tau for duration calculation
	m_transientData.currentTau = engineConfiguration->wwaeTau;
	
	return true;
}

void WwAdaptiveStateMachine::collectLambdaData(float lambdaError) {
	// Collect immediate phase data
	if (m_currentState == WwAdaptiveState::GATHERING_IMMEDIATE) {
		if (m_gatheringData.immediateBufferCount < WW_IMMEDIATE_BUFFER_SIZE) {
			m_gatheringData.immediateLambdaBuffer[m_gatheringData.immediateBufferIndex] = lambdaError;
			m_gatheringData.immediateBufferIndex = (m_gatheringData.immediateBufferIndex + 1) % WW_IMMEDIATE_BUFFER_SIZE;
			m_gatheringData.immediateBufferCount++;
		}
	}
	
	// Collect prolonged phase data
	if (m_currentState == WwAdaptiveState::GATHERING_PROLONGED) {
		if (m_gatheringData.prolongedBufferCount < m_gatheringData.prolongedBufferTarget) {
			m_gatheringData.prolongedLambdaBuffer[m_gatheringData.prolongedBufferIndex] = lambdaError;
			m_gatheringData.prolongedBufferIndex = (m_gatheringData.prolongedBufferIndex + 1) % WW_PROLONGED_BUFFER_SIZE_MAX;
			m_gatheringData.prolongedBufferCount++;
		}
	}
}

bool WwAdaptiveStateMachine::shouldAdaptBeta() const {
	// Always adapt beta when we have immediate data
	return true;
}

bool WwAdaptiveStateMachine::shouldAdaptTau() const {
	// Always adapt tau when we have prolonged data
	return true;
}

void WwAdaptiveStateMachine::updateAdaptationMode() {
	// Increment transient counter for statistics
	m_learningData.transientCounter++;
	
	// Since we always adapt both beta and tau, no mode switching needed
	// Keep the counter for potential future use or debugging
}

float WwAdaptiveStateMachine::calculateBetaCorrection(float avgLambdaError) {
	// Beta correction based on immediate lambda response
	const float correctionRate = engineConfiguration->wwBetaLearningRate;
	const float maxCorrection = 0.67f;
	
	// Validate learning rate
	if (correctionRate <= 0 || correctionRate > 1.0f) {
		return 1.0f;
	}
	
	if (std::isnan(avgLambdaError) || fabsf(avgLambdaError) < 0.02f) {
		return 1.0f;
	}
	
	float correction = 1.0f;
	
	// CRITICAL FIX: Corrected physics direction
	// lambdaError = lambda - target: positive = lean, negative = rich
	if (m_transientData.isPositive) {
		// Acceleration: lean lambda → need MORE beta (increase beta)
		// rich lambda → need LESS beta (decrease beta)
		correction = 1.0f + (avgLambdaError * correctionRate);
	} else {
		// Deceleration: OPPOSITE physics
		// lean lambda → need LESS beta (decrease beta)
		// rich lambda → need MORE beta (increase beta)
		correction = 1.0f - (avgLambdaError * correctionRate);
	}
	
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	// Clamp correction to bounds
	return maxF(1.0f - maxCorrection, minF(1.0f + maxCorrection, correction));
}

float WwAdaptiveStateMachine::calculateTauCorrection() {
	// CRITICAL FIX: Use LINEAR REGRESSION to detect lambda slope/trend
	// Based on lessons learned - tau correction should detect tendency, not exponential fitting
	const float correctionRate = engineConfiguration->wwTauLearningRate;
	const float maxCorrection = 0.67f;
	
	// Validate learning rate
	if (correctionRate <= 0 || correctionRate > 1.0f) {
		return 1.0f;
	}
	
	// Require minimum samples for valid trend detection
	if (m_gatheringData.prolongedBufferCount < 10) {
		return 1.0f;
	}
	
	// Calculate linear regression slope: slope = (n*sumXY - sumX*sumY) / (n*sumX2 - sumX*sumX)
	float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
	int n = m_gatheringData.prolongedBufferCount;
	
	for (int i = 0; i < n; i++) {
		float x = i * CALLBACK_PERIOD_SEC; // time in seconds
		float y = m_gatheringData.prolongedLambdaBuffer[i]; // lambda error
		
		if (!std::isnan(y)) {
			sumX += x;
			sumY += y;
			sumXY += x * y;
			sumX2 += x * x;
		}
	}
	
	// Calculate slope - positive slope = lambda trending lean, negative = trending rich
	float denominator = n * sumX2 - sumX * sumX;
	if (fabsf(denominator) < 0.001f) {
		return 1.0f; // Cannot calculate reliable slope
	}
	
	float slope = (n * sumXY - sumX * sumY) / denominator;
	
	// Require minimum significant trend for correction
	if (fabsf(slope) < 0.001f) {
		return 1.0f; // No significant trend detected
	}
	
	float correction = 1.0f;
	
	// PHYSICS: Apply correction based on lambda trend direction and transient type
	if (m_transientData.isPositive) {
		// Acceleration physics
		if (slope > 0.001f) {
			// Lambda trending lean (positive slope) → tau too low → increase tau
			correction = 1.0f + (slope * correctionRate * 100.0f);
		} else if (slope < -0.001f) {
			// Lambda trending rich (negative slope) → tau too high → decrease tau
			correction = 1.0f - (fabsf(slope) * correctionRate * 100.0f);
		}
	} else {
		// Deceleration physics - OPPOSITE direction
		if (slope > 0.001f) {
			// Lambda trending lean (positive slope) → tau too high → decrease tau
			correction = 1.0f - (slope * correctionRate * 100.0f);
		} else if (slope < -0.001f) {
			// Lambda trending rich (negative slope) → tau too low → increase tau
			correction = 1.0f + (fabsf(slope) * correctionRate * 100.0f);
		}
	}
	
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	// Apply bounds - allow wider range for tau corrections
	return maxF(1.0f - maxCorrection, minF(1.0f + maxCorrection, correction));
}

void WwAdaptiveStateMachine::applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map) {
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	// Cross-coupling correction to reduce instability - reduced aggressiveness
	float cross_coupling = 1.0f - (0.1f * fabsf(betaCorrection - tauCorrection));
	cross_coupling = maxF(0.7f, minF(1.0f, cross_coupling));
	
	// Apply cross-coupling factor
	betaCorrection = 1.0f + (betaCorrection - 1.0f) * cross_coupling;
	tauCorrection = 1.0f + (tauCorrection - 1.0f) * cross_coupling;
	
	// Use getBin() for table indexing
	auto binMap = priv::getBin(map, config->wwCorrectionMapBins);
	auto binRpm = priv::getBin(rpm, config->wwCorrectionRpmBins);
	
	int mapIdx = binMap.Idx;
	int rpmIdx = binRpm.Idx;
	
	// Bounds check - FIX: Permitir índice máximo também
	if (mapIdx < 0 || mapIdx >= WWAE_CORRECTION_SIZE || 
		rpmIdx < 0 || rpmIdx >= WWAE_CORRECTION_SIZE) {
		return;
	}
	
	// Apply beta correction
	if (betaCorrection != 1.0f && !std::isnan(betaCorrection)) {
		float currentBetaCorrection = config->wwBetaCorrection[mapIdx][rpmIdx];
		
		if (!std::isnan(currentBetaCorrection)) {
			float newBetaCorrection = currentBetaCorrection * betaCorrection;
			
			if (!std::isnan(newBetaCorrection)) {
				newBetaCorrection = maxF(0.5f, minF(2.0f, newBetaCorrection));
				config->wwBetaCorrection[mapIdx][rpmIdx] = newBetaCorrection;
				
				// Apply smoothing to adjacent cells
				smoothCorrectionTable(mapIdx, rpmIdx, betaCorrection, 1.0f);
			}
		}
	}
	
	// Apply tau correction
	if (tauCorrection != 1.0f && !std::isnan(tauCorrection)) {
		float currentTauCorrection = config->wwTauCorrection[mapIdx][rpmIdx];
		
		if (!std::isnan(currentTauCorrection)) {
			float newTauCorrection = currentTauCorrection * tauCorrection;
			
			if (!std::isnan(newTauCorrection)) {
				newTauCorrection = maxF(0.5f, minF(2.0f, newTauCorrection));
				config->wwTauCorrection[mapIdx][rpmIdx] = newTauCorrection;
				
				// Apply smoothing to adjacent cells
				smoothCorrectionTable(mapIdx, rpmIdx, 1.0f, tauCorrection);
			}
		}
	}
}

void WwAdaptiveStateMachine::smoothCorrectionTable(int mapIdx, int rpmIdx, float betaCorrection, float tauCorrection) {
	const float smoothingFactor = 0.3f;
	
	for (int dMap = -1; dMap <= 1; dMap++) {
		for (int dRpm = -1; dRpm <= 1; dRpm++) {
			if (dMap == 0 && dRpm == 0) continue;
			
			int adjMapIdx = mapIdx + dMap;
			int adjRpmIdx = rpmIdx + dRpm;
			
			if (adjMapIdx < 0 || adjMapIdx >= WWAE_CORRECTION_SIZE ||
				adjRpmIdx < 0 || adjRpmIdx >= WWAE_CORRECTION_SIZE) {
				continue;
			}
			
			int distance = abs(dMap) + abs(dRpm);
			if (distance == 0) continue;
			
			float factor = smoothingFactor / distance;
			
			// Apply smoothed beta correction
			if (betaCorrection != 1.0f && !std::isnan(betaCorrection)) {
				float smoothedBetaCorr = 1.0f + factor * (betaCorrection - 1.0f);
				float oldBeta = config->wwBetaCorrection[adjMapIdx][adjRpmIdx];
				
				if (!std::isnan(oldBeta) && !std::isnan(smoothedBetaCorr)) {
					float newBeta = oldBeta * smoothedBetaCorr;
					if (!std::isnan(newBeta)) {
						config->wwBetaCorrection[adjMapIdx][adjRpmIdx] = clampF(0.5f, newBeta, 2.0f);
					}
				}
			}
			
			// Apply smoothed tau correction
			if (tauCorrection != 1.0f && !std::isnan(tauCorrection)) {
				float smoothedTauCorr = 1.0f + factor * (tauCorrection - 1.0f);
				float oldTau = config->wwTauCorrection[adjMapIdx][adjRpmIdx];
				
				if (!std::isnan(oldTau) && !std::isnan(smoothedTauCorr)) {
					float newTau = oldTau * smoothedTauCorr;
					if (!std::isnan(newTau)) {
						config->wwTauCorrection[adjMapIdx][adjRpmIdx] = clampF(0.5f, newTau, 2.0f);
					}
				}
			}
		}
	}
}

void WwAdaptiveStateMachine::onIgnitionHandler(bool ignitionOn) {
	// CRITICAL FIX: Minimal implementation to avoid hangups during ignition changes
	// Complex operations are handled in update() loop where they're safer
	
	if (ignitionOn) {
		// Only set flags, don't call complex functions during ignition change
		m_currentState = WwAdaptiveState::IDLE;
		m_stateStartCallback = m_callbackCounter; // Reset state start callback instead of m_stateStartTime
		// Reset counters directly without function calls
		m_learningData.transientCounter = 0;
		m_learningData.currentCycleCount = 0;
		// No specific mode needed since we always adapt both
	} else {
		// Simply flag that ignition is off - update() will handle saving
		// Don't do complex state transitions during ignition change
		if (m_currentState != WwAdaptiveState::IDLE) {
			m_currentState = WwAdaptiveState::SAVING;
		}
	}
}

const char* WwAdaptiveStateMachine::getStateString() const {
	return getStateString(m_currentState);
}

const char* WwAdaptiveStateMachine::getStateString(WwAdaptiveState state) const {
	switch (state) {
		case WwAdaptiveState::IDLE: return "IDLE";
		case WwAdaptiveState::TRANSIENT_DETECTED: return "TRANSIENT_DETECTED";
		case WwAdaptiveState::DELAY_LAMBDA: return "DELAY_LAMBDA";
		case WwAdaptiveState::GATHERING_IMMEDIATE: return "GATHERING_IMMEDIATE";
		case WwAdaptiveState::GATHERING_PROLONGED: return "GATHERING_PROLONGED";
		case WwAdaptiveState::LEARNING_ANALYSIS: return "LEARNING_ANALYSIS";
		case WwAdaptiveState::APPLYING_CORRECTION: return "APPLYING_CORRECTION";
		case WwAdaptiveState::SAVING: return "SAVING";
		default: return "UNKNOWN";
	}
}