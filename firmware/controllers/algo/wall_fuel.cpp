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
	
	// CRITICAL FIX 3: SIMPLIFIED TRANSITION LOGIC - Much more permissive
	bool hasImmediateData = (m_gatheringData.immediateBufferCount > 0);
	bool hasMinimalProlongedData = (m_gatheringData.prolongedBufferCount >= 20); // Relaxed from 50
	
	// RELAXED TIMING: Base minimum time on a simple 400ms instead of complex tau calculations
	uint32_t minTimeCallbacks = 80; // 400ms = 80 callbacks (relaxed from 1×tau)
	bool minTimeElapsed = elapsed >= minTimeCallbacks;
	
	// PERMISSIVE TARGET: Allow shorter data collection for responsive learning
	float configuredTau = m_transientData.currentTau;
	uint32_t adaptiveTarget = minI(
		(uint32_t)(configuredTau * CALLBACK_FREQUENCY_HZ * 1.5f), // 1.5×tau instead of 3×tau
		400 // Maximum 2s collection time
	);
	adaptiveTarget = maxI(80, adaptiveTarget); // Minimum 400ms
	
	// Update target if it was too conservative
	if (m_gatheringData.prolongedBufferTarget > adaptiveTarget) {
		m_gatheringData.prolongedBufferTarget = adaptiveTarget;
	}
	
	bool phaseComplete = (m_gatheringData.prolongedBufferCount >= m_gatheringData.prolongedBufferTarget);
	
	// GENEROUS TIMEOUT: 8 seconds should be enough for any reasonable tau
	bool timeout = elapsed >= 1600; // 8s = 1600 callbacks
	
	// TRANSITION LOGIC: Multiple paths to proceed with learning
	if (phaseComplete) {
		// Ideal case: collected target amount of data
		transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
	} else if (hasImmediateData && minTimeElapsed && hasMinimalProlongedData) {
		// Good case: have both beta and minimal tau data
		transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
	} else if (hasImmediateData && timeout) {
		// Fallback case: timeout but at least have beta data
		transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
	} else if (timeout && !hasImmediateData) {
		// Failure case: timeout with no useful data
		resetToIdle();
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
	
	// Check for transient threshold - usar configuração ou padrão
	float transientThreshold = engineConfiguration->wwMapThreshold; 
	if (transientThreshold <= 0) {
		transientThreshold = 30.0f; // fallback padrão
	}
	
	m_lastTransientMagnitude = m_loadData.transientMagnitude;
	bool hasTransient = m_loadData.transientMagnitude > transientThreshold;
	
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
	
	if (m_transientData.isPositive) {
		// Positive transient (acceleration)
		correction = 1.0f - (avgLambdaError * correctionRate);
	} else {
		// Negative transient (deceleration)
		correction = 1.0f + (avgLambdaError * correctionRate * 1.2f);
	}
	
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	// Clamp correction to bounds
	return maxF(1.0f - maxCorrection, minF(1.0f + maxCorrection, correction));
}

float WwAdaptiveStateMachine::calculateTauCorrection() {
	// Tau correction based on exponential decay analysis during prolonged phase
	const float correctionRate = engineConfiguration->wwTauLearningRate;
	const float maxCorrection = 0.67f;
	
	// Validate learning rate
	if (correctionRate <= 0 || correctionRate > 1.0f) {
		return 1.0f;
	}
	
	// CRITICAL FIX 1: RELAXED TAU VALIDATION - Much less restrictive requirements
	float configuredTau = m_transientData.currentTau;
	
	// CRITICAL FIX: Protect against division by zero
	if (configuredTau <= 0.001f) {
		return 1.0f;
	}
	
	// RELAXED VALIDATION: Only require minimum 20 samples instead of 0.5×tau
	uint32_t minSamplesForTau = 20; // Fixed minimum, much more permissive
	
	if (m_gatheringData.prolongedBufferCount < minSamplesForTau) {
		return 1.0f;
	}
	
	// CRITICAL FIX 2: PERFORMANCE OPTIMIZATION - Strict limits on processing
	float timeStep = CALLBACK_PERIOD_SEC; // 5ms per sample
	int totalSamples = minI(m_gatheringData.prolongedBufferCount, m_gatheringData.prolongedBufferTarget);
	
	// PERFORMANCE LIMIT: Never process more than 100 samples regardless of tau
	int maxProcessingSamples = 100;
	int step = maxI(1, totalSamples / maxProcessingSamples); // Ensure we stay under limit
	
	// First pass: Calculate moving average to reduce noise for exponential fitting
	float smoothedData[100]; // Fixed maximum size for performance
	int validSamples = 0;
	
	for (int i = 0; i < totalSamples && validSamples < maxProcessingSamples; i += step) {
		float sample = m_gatheringData.prolongedLambdaBuffer[i];
		if (!std::isnan(sample)) {
			// Simple 3-point moving average if we have enough samples
			if (i >= step && i < totalSamples - step) {
				float prev = m_gatheringData.prolongedLambdaBuffer[i - step];
				float next = m_gatheringData.prolongedLambdaBuffer[i + step];
				if (!std::isnan(prev) && !std::isnan(next)) {
					smoothedData[validSamples] = (prev + sample + next) / 3.0f;
				} else {
					smoothedData[validSamples] = sample;
				}
			} else {
				smoothedData[validSamples] = sample;
			}
			validSamples++;
		}
	}
	
	// RELAXED VALIDATION: Accept even fewer samples for analysis
	if (validSamples < 10) {
		return 1.0f;
	}
	
	// Calculate mean for offset estimation
	float meanLambda = 0;
	for (int i = 0; i < validSamples; i++) {
		meanLambda += smoothedData[i];
	}
	meanLambda /= validSamples;
	
	// Estimate initial and final values for exponential fitting
	// Take average of first 20% and last 20% of samples
	int earlyCount = maxI(2, validSamples * 0.2f);
	int lateCount = maxI(2, validSamples * 0.2f);
	
	float initialValue = 0, finalValue = 0;
	for (int i = 0; i < earlyCount; i++) {
		initialValue += smoothedData[i];
	}
	initialValue /= earlyCount;
	
	for (int i = validSamples - lateCount; i < validSamples; i++) {
		finalValue += smoothedData[i];
	}
	finalValue /= lateCount;
	
	// Check if we have exponential decay/growth pattern
	float totalChange = fabsf(finalValue - initialValue);
	if (totalChange < 0.003f) { // Even more relaxed
		return 1.0f; // Not enough change to fit exponential
	}
	
	// EXPONENTIAL FITTING: y(t) = A * exp(-t/τ) + B
	// Simplified approach: estimate τ from 63.2% decay point
	float targetValue = finalValue + 0.632f * (initialValue - finalValue); // 63.2% point
	float estimatedTau = 0;
	
	// Find time where signal crosses 63.2% point
	bool foundCrossing = false;
	for (int i = 1; i < validSamples; i++) {
		
		if ((initialValue > finalValue && smoothedData[i] <= targetValue && smoothedData[i-1] > targetValue) ||
			(initialValue < finalValue && smoothedData[i] >= targetValue && smoothedData[i-1] < targetValue)) {
			
			// Linear interpolation for precise crossing time
			float t1 = (i-1) * timeStep * step;
			float t2 = i * timeStep * step;
			float y1 = smoothedData[i-1];
			float y2 = smoothedData[i];
			
			float crossingTime = t1 + (t2 - t1) * (targetValue - y1) / (y2 - y1);
			estimatedTau = crossingTime;
			foundCrossing = true;
			break;
		}
	}
	
	// Alternative method: least squares fitting if crossing not found
	if (!foundCrossing || estimatedTau <= 0 || estimatedTau > 10.0f) {
		// Use linearized exponential fitting: ln(|y-B|) = ln(A) - t/τ
		float sumT = 0, sumLnY = 0, sumT2 = 0, sumTLnY = 0;
		int fitSamples = 0;
		
		for (int i = 0; i < validSamples; i++) {
			float t = i * timeStep * step;
			float y = smoothedData[i] - finalValue; // Remove offset
			
			if (fabsf(y) > 0.001f) { // Avoid log of small numbers
				float lnY = logf(fabsf(y));
				if (!std::isnan(lnY) && !std::isinf(lnY)) {
					sumT += t;
					sumLnY += lnY;
					sumT2 += t * t;
					sumTLnY += t * lnY;
					fitSamples++;
				}
			}
		}
		
		if (fitSamples >= 5) { // Relaxed from 10 to 5
			float denominator = fitSamples * sumT2 - sumT * sumT;
			if (fabsf(denominator) > 0.001f) {
				float slope = (fitSamples * sumTLnY - sumT * sumLnY) / denominator;
				if (slope < -0.01f) { // Negative slope for decay
					estimatedTau = -1.0f / slope;
				}
			}
		}
	}
	
	// Validate estimated tau
	if (estimatedTau <= 0.05f || estimatedTau > 10.0f || std::isnan(estimatedTau)) {
		return 1.0f;
	}
	
	// CALCULATE R² FOR QUALITY VALIDATION - RELAXED THRESHOLD
	float ssRes = 0, ssTot = 0;
	for (int i = 0; i < validSamples; i++) {
		float t = i * timeStep * step;
		float predicted = finalValue + (initialValue - finalValue) * expf_taylor(-t / estimatedTau);
		float actual = smoothedData[i];
		
		ssRes += (actual - predicted) * (actual - predicted);
		ssTot += (actual - meanLambda) * (actual - meanLambda);
	}
	
	float rSquared = (ssTot > 0.001f) ? (1.0f - ssRes / ssTot) : 0.0f;
	
	// RELAXED QUALITY: Require minimum R² of 0.15 instead of 0.3 for permissive learning
	if (rSquared < 0.15f) {
		return 1.0f;
	}
	
	// STEP 4: APPLY PHYSICAL CORRECTION
	float tauError = estimatedTau - configuredTau;
	float relativeError = tauError / configuredTau;
	
	// Adaptive correction rate based on confidence (R²) and error magnitude
	float confidenceFactor = minF(1.0f, rSquared * 2.0f); // Boost confidence for R² < 0.5
	float errorMagnitude = minF(1.0f, fabsf(relativeError)); // Limit max error magnitude
	float adaptiveRate = correctionRate * confidenceFactor * errorMagnitude;
	
	// Physical correction calculation
	float correction = 1.0f;
	
	if (m_transientData.isPositive) {
		// Positive transient (acceleration): if measured tau > configured tau, increase tau
		if (tauError > 0.03f) { // Relaxed from 0.05f
			correction = 1.0f + (relativeError * adaptiveRate);
		} else if (tauError < -0.03f) { // Relaxed from -0.05f
			correction = 1.0f + (relativeError * adaptiveRate); // relativeError is negative
		}
	} else {
		// Negative transient (deceleration): opposite correction direction
		if (tauError > 0.03f) {
			correction = 1.0f - (relativeError * adaptiveRate * 0.8f); // Slightly less aggressive
		} else if (tauError < -0.03f) {
			correction = 1.0f - (relativeError * adaptiveRate * 0.8f);
		}
	}
	
	// Validate final correction
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	// Apply maximum correction bounds
	correction = maxF(1.0f - maxCorrection, minF(1.0f + maxCorrection, correction));
	
	return correction;
}

void WwAdaptiveStateMachine::applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map) {
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	// Cross-coupling correction to reduce instability
	float cross_coupling = 1.0f - (0.2f * fabsf(betaCorrection - tauCorrection));
	cross_coupling = maxF(0.5f, minF(1.0f, cross_coupling));
	
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