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
	
	// DEBUG: Every 1000 callbacks (5 seconds), log state info if not IDLE
	if ((m_callbackCounter % 1000) == 0 && m_currentState != WwAdaptiveState::IDLE) {
		efiPrintf("WW State: %s, Elapsed: %.1fs, Transients: %d, Corrections: %d", 
			getStateString(), getElapsedSeconds(), m_debugTransientsDetected, m_debugCorrectionsApplied);
	}
	
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
	if (m_loadData.transientMagnitude > 30.0f) {
		efiPrintf("WW DelayLambda interrupted by new transient (%.1f)", m_loadData.transientMagnitude);
		resetToIdle();
		return;
	}
	
	// Check delay timeout
	if (elapsed >= m_lambdaDelayCallbacks) {
		efiPrintf("WW DelayLambda complete (%d callbacks), starting immediate gathering", elapsed);
		transitionTo(WwAdaptiveState::GATHERING_IMMEDIATE);
	}
}

void WwAdaptiveStateMachine::handleGatheringImmediateState() {
	uint32_t elapsed = getElapsedCallbacks();
	
	// Check for interruption by new transient
	if (m_loadData.transientMagnitude > 30.0f) {
		efiPrintf("WW ImmediateGathering interrupted by new transient (%.1f)", m_loadData.transientMagnitude);
		resetToIdle();
		return;
	}
	
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
		efiPrintf("WW ImmediateGathering complete: %d samples collected", m_gatheringData.immediateBufferCount);
		transitionTo(WwAdaptiveState::GATHERING_PROLONGED);
	}
}

void WwAdaptiveStateMachine::handleGatheringProlongedState() {
	uint32_t elapsed = getElapsedCallbacks();
	
	// Check for interruption by new strong transient
	if (m_loadData.transientMagnitude > 50.0f) {
		efiPrintf("WW ProlongedGathering interrupted by strong transient (%.1f)", m_loadData.transientMagnitude);
		// Apply incomplete correction (beta only)
		if (m_gatheringData.immediateBufferCount > 0 && shouldAdaptBeta()) {
			transitionTo(WwAdaptiveState::APPLYING_CORRECTION);
		} else {
			resetToIdle();
		}
		return;
	}
	
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
	
	// Check if prolonged phase is complete or timeout
	bool phaseComplete = (m_gatheringData.prolongedBufferCount >= m_gatheringData.prolongedBufferTarget);
	bool timeout = elapsed >= m_incompleteTimeoutCallbacks;
	
	if (phaseComplete) {
		efiPrintf("WW ProlongedGathering complete: %d samples collected", m_gatheringData.prolongedBufferCount);
		transitionTo(WwAdaptiveState::LEARNING_ANALYSIS);
	} else if (timeout) {
		efiPrintf("WW ProlongedGathering timeout after %d callbacks", elapsed);
		// Apply incomplete correction (beta only)
		if (m_gatheringData.immediateBufferCount > 0 && shouldAdaptBeta()) {
			transitionTo(WwAdaptiveState::APPLYING_CORRECTION);
		} else {
			resetToIdle();
		}
	}
}

void WwAdaptiveStateMachine::handleLearningAnalysisState() {
	// Calculate average immediate lambda error for beta correction
	if (m_gatheringData.immediateBufferCount > 0 && shouldAdaptBeta()) {
		float sum = 0.0f;
		int validSamples = 0;
		
		// PERFORMANCE FIX: WW_IMMEDIATE_BUFFER_SIZE is 40, which is acceptable for 200Hz
		int maxSamples = fminf(m_gatheringData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
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
	if (m_gatheringData.prolongedBufferCount > 10 && shouldAdaptTau()) {
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
	// DEBUG: Log what corrections are being applied
	efiPrintf("WW APPLYING CORRECTIONS: beta=%.3f tau=%.3f at rpm=%.0f map=%.1f", 
		m_correctionData.betaCorrection, m_correctionData.tauCorrection,
		m_correctionData.targetRpm, m_correctionData.targetMap);
	
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
	
	efiPrintf("WW Applied correction #%d (total cycles: %d)", 
		m_debugCorrectionsApplied, m_learningData.completedLearningCycles);
	
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
	WwAdaptiveState oldState = m_currentState;
	m_currentState = newState;
	m_stateStartCallback = m_callbackCounter;
	
	// DEBUG: Log state transitions  
	efiPrintf("WW Transition: %s -> %s (callback %d)", 
		getStateString(oldState), getStateString(), m_callbackCounter);
	
	// State entry actions
	switch (newState) {
		case WwAdaptiveState::TRANSIENT_DETECTED:
			// Reset gathering data for new transient
			m_gatheringData.reset();
			m_correctionData.reset();
			break;
			
		case WwAdaptiveState::GATHERING_PROLONGED:
			// Calculate dynamic prolonged phase duration in callbacks
			float durationSeconds = WW_TAU_MULTIPLIER * m_transientData.currentTau;
			uint32_t durationCallbacks = (uint32_t)(durationSeconds * CALLBACK_FREQUENCY_HZ);
			m_gatheringData.prolongedBufferTarget = fminf(durationCallbacks, WW_PROLONGED_BUFFER_SIZE_MAX);
			break;
			
		default:
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
	
	// DEBUG: Log transient detection details every 200 callbacks (1 second)
	if ((m_callbackCounter % 200) == 0) {
		efiPrintf("WW Detect: mag=%.1f thresh=%.1f rpm=%.0f map=%.1f clt=%.1f hasT=%d", 
			m_loadData.transientMagnitude, transientThreshold, rpm, map, clt.Value, hasTransient);
	}
	
	if (hasTransient) {
		m_debugTransientsDetected++;
		efiPrintf("WW TRANSIENT DETECTED! Magnitude=%.1f (thresh=%.1f)", 
			m_loadData.transientMagnitude, transientThreshold);
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
	return m_learningData.currentMode == LearningData::ADAPT_BETA_ONLY ||
		   m_learningData.currentMode == LearningData::ADAPT_BOTH;
}

bool WwAdaptiveStateMachine::shouldAdaptTau() const {
	return m_learningData.currentMode == LearningData::ADAPT_TAU_ONLY ||
		   m_learningData.currentMode == LearningData::ADAPT_BOTH;
}

void WwAdaptiveStateMachine::updateAdaptationMode() {
	// Increment transient counter
	m_learningData.transientCounter++;
	
	// Check if we need to switch adaptation modes
	if (m_learningData.transientCounter >= m_learningData.adaptationCycleLength) {
		m_learningData.transientCounter = 0;
		m_learningData.currentCycleCount++;
		
		// Determine next adaptation mode based on cycle count
		if (m_learningData.currentMode == LearningData::ADAPT_BETA_ONLY) {
			if (m_learningData.currentCycleCount >= m_learningData.betaAdaptationCycles) {
				// Switch to tau adaptation
				m_learningData.currentMode = LearningData::ADAPT_TAU_ONLY;
				m_learningData.currentCycleCount = 0;
			}
		} else if (m_learningData.currentMode == LearningData::ADAPT_TAU_ONLY) {
			if (m_learningData.currentCycleCount >= m_learningData.tauAdaptationCycles) {
				// Switch back to beta adaptation
				m_learningData.currentMode = LearningData::ADAPT_BETA_ONLY;
				m_learningData.currentCycleCount = 0;
			}
		}
	}
}

float WwAdaptiveStateMachine::calculateBetaCorrection(float avgLambdaError) {
	// Beta correction based on immediate lambda response
	const float correctionRate = engineConfiguration->wwBetaLearningRate;
	const float maxCorrection = 0.67f;
	
	// Validate learning rate
	if (correctionRate <= 0 || correctionRate > 1.0f) {
		efiPrintf("WW Invalid beta learning rate: %.3f", correctionRate);
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
	return fmaxf(1.0f - maxCorrection, fminf(1.0f + maxCorrection, correction));
}

float WwAdaptiveStateMachine::calculateTauCorrection() {
	// Tau correction based on lambda trend during prolonged phase
	const float correctionRate = engineConfiguration->wwTauLearningRate;
	const float maxCorrection = 0.67f;
	
	// Validate learning rate
	if (correctionRate <= 0 || correctionRate > 1.0f) {
		efiPrintf("WW Invalid tau learning rate: %.3f", correctionRate);
		return 1.0f;
	}
	
	if (m_gatheringData.prolongedBufferCount < 10) {
		return 1.0f;
	}
	
	// Calculate linear trend (slope) of lambda error over time
	float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
	int validSamples = 0;
	
	// PERFORMANCE FIX: Process all samples but with step to reduce computational load
	// At 200Hz (5ms period), we need to limit processing time while preserving data coverage
	int totalSamples = fminf(m_gatheringData.prolongedBufferCount, m_gatheringData.prolongedBufferTarget);
	int step = (totalSamples > 100) ? (totalSamples / 100) : 1; // Dynamic step based on sample count
	
	for (int i = 0; i < totalSamples; i += step) {
		float sample = m_gatheringData.prolongedLambdaBuffer[i];
		if (!std::isnan(sample)) {
			float x = (float)i;
			float y = sample;
			
			sumX += x;
			sumY += y;
			sumXY += x * y;
			sumX2 += x * x;
			validSamples++;
		}
	}
	
	if (validSamples < 10) {
		return 1.0f;
	}
	
	// Calculate slope
	float denominator = validSamples * sumX2 - sumX * sumX;
	if (fabsf(denominator) < 0.001f) {
		return 1.0f;
	}
	
	float slope = (validSamples * sumXY - sumX * sumY) / denominator;
	
	if (std::isnan(slope) || fabsf(slope) < 0.001f) {
		return 1.0f;
	}
	
	float correction = 1.0f;
	
	if (m_transientData.isPositive) {
		// Positive transient (acceleration)
		correction = 1.0f - (slope * correctionRate * 100.0f);
	} else {
		// Negative transient (deceleration)
		correction = 1.0f + (slope * correctionRate * 100.0f);
	}
	
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	// Clamp correction to bounds
	return fmaxf(1.0f - maxCorrection, fminf(1.0f + maxCorrection, correction));
}

void WwAdaptiveStateMachine::applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map) {
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	// Cross-coupling correction to reduce instability
	float cross_coupling = 1.0f - (0.2f * fabsf(betaCorrection - tauCorrection));
	cross_coupling = fmaxf(0.5f, fminf(1.0f, cross_coupling));
	
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
		efiPrintf("WW applyCorrectionToTable BOUNDS ERROR: mapIdx=%d rpmIdx=%d (size=%d)", 
			mapIdx, rpmIdx, WWAE_CORRECTION_SIZE);
		return;
	}
	
	// Apply beta correction
	if (betaCorrection != 1.0f && !std::isnan(betaCorrection)) {
		float currentBetaCorrection = config->wwBetaCorrection[mapIdx][rpmIdx];
		
		if (!std::isnan(currentBetaCorrection)) {
			float newBetaCorrection = currentBetaCorrection * betaCorrection;
			
			if (!std::isnan(newBetaCorrection)) {
				newBetaCorrection = fmaxf(0.5f, fminf(2.0f, newBetaCorrection));
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
				newTauCorrection = fmaxf(0.5f, fminf(2.0f, newTauCorrection));
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
		m_stateStartTime = 0;
		// Reset counters directly without function calls
		m_learningData.transientCounter = 0;
		m_learningData.currentCycleCount = 0;
		m_learningData.currentMode = LearningData::ADAPT_BETA_ONLY;
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