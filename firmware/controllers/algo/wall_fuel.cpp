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
	if (tau < 0.001f || beta < 0.01f) {  // More strict tau check (0.001f instead of 0.01f)
		m_enable = false;
		return;
	}
	
	auto rpm = Sensor::getOrZero(SensorType::Rpm);

	// Ignore low RPM - additional protection against division by zero
	if (rpm < 100) {  // Keep original 100 RPM limit for practical operation
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

	// === NEW STATE MACHINE IMPLEMENTATION ===
	// Only proceed with adaptive learning if enabled and conditions are met
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	float currentTime = m_learningTimer.getElapsedSeconds();
	
	// Validate sensor data before proceeding
	if (!validateSensorData()) {
		if (m_adaptiveData.isTransientActive()) {
			handleErrorState("Invalid sensor data");
		}
		return;
	}
	
	// Update derivatives continuously
	auto mapSensor = Sensor::get(SensorType::Map);
	auto tpsSensor = Sensor::get(SensorType::Tps1);
	updateLoadDerivative(mapSensor.Value);
	updateTpsDerivative(tpsSensor.Value);
	
	// Update and process state machine (ONLY NEW SYSTEM)
	updateStateMachine(currentTime);
	processCurrentState(currentTime);
	
	// Periodic status logging (every 2 seconds)
	static float lastLogTime = 0;
	if (currentTime - lastLogTime > 2.0f) {
		logPeriodicStatus(currentTime);
		lastLogTime = currentTime;
	}
}

void WallFuelController::onIgnitionStateChanged(bool ignitionOn) {
	m_ignitionState = ignitionOn;
	
	if (ignitionOn) {
		// Reset timers and adaptive data when ignition turns on
		m_learningTimer.reset();
		m_ignitionOffTimer.reset();
		m_adaptiveData.reset();
		
		m_pendingSave = false;
	} else {
		// When ignition turns off, schedule save if we have updates
		if (engineConfiguration->wwEnableAdaptiveLearning) {
			setNeedToWriteConfiguration();
			m_pendingSave = true;
			m_ignitionOffTimer.reset();
		}
	}
}

void WallFuelController::updateLoadDerivative(float currentLoad) {
	// Store current load in circular buffer
	m_adaptiveData.loadBuffer[m_adaptiveData.bufferIndex] = currentLoad;
	m_adaptiveData.bufferIndex = (m_adaptiveData.bufferIndex + 1) % WW_LOAD_BUFFER_SIZE;
	
	// Calculate load derivative (rate of change)
	// Use difference between current and oldest value in buffer
	int oldestIndex = m_adaptiveData.bufferIndex; // Points to oldest after increment
	float oldLoad = m_adaptiveData.loadBuffer[oldestIndex];
	
	// Calculate derivative in kPa/s (assuming onFastCallback runs at 200Hz)
	const float deltaTime = 0.005f * WW_LOAD_BUFFER_SIZE; // 40 samples * 5ms = 200ms
	m_adaptiveData.loadDerivative = (currentLoad - oldLoad) / deltaTime;
	
	m_adaptiveData.lastLoad = currentLoad;
}

void WallFuelController::updateTpsDerivative(float currentTps) {
	// Store current TPS in circular buffer
	m_adaptiveData.tpsBuffer[m_adaptiveData.tpsBufferIndex] = currentTps;
	m_adaptiveData.tpsBufferIndex = (m_adaptiveData.tpsBufferIndex + 1) % WW_TPS_BUFFER_SIZE;
	
	// Calculate TPS derivative (rate of change)
	// Use difference between current and oldest value in buffer
	int oldestIndex = m_adaptiveData.tpsBufferIndex; // Points to oldest after increment
	float oldTps = m_adaptiveData.tpsBuffer[oldestIndex];
	
	// Calculate derivative in %/s (assuming onFastCallback runs at 200Hz)
	const float deltaTime = 0.005f * WW_TPS_BUFFER_SIZE; // 40 samples * 5ms = 200ms
	m_adaptiveData.tpsDerivative = (currentTps - oldTps) / deltaTime;
	
	m_adaptiveData.lastTps = currentTps;
}

void WallFuelController::onActualFuelInjection(float injectedMass, int cylinderIndex) {
	(void)injectedMass;
	(void)cylinderIndex;
	if (!engineConfiguration->wwEnableAdaptiveLearning || !m_enable) {
		return;
	}
	
	float rpm = Sensor::getOrZero(SensorType::Rpm);
	// float map = Sensor::getOrZero(SensorType::Map);
	auto clt = Sensor::get(SensorType::Clt);
	
	// Verificar condições mínimas
	if (rpm < 100 || !clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return;
	}
	
	// Esta função pode ser expandida no futuro para integração com sistema de adaptação
	// Por enquanto, apenas registra a injeção para possível uso futuro
}

// === STATE MACHINE IMPLEMENTATION ===

bool WallFuelController::validateSensorData() const {
	// Check if engine is warm enough
	auto clt = Sensor::get(SensorType::Clt);
	if (!clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return false;
	}
	
	// Check MAP sensor
	auto mapSensor = Sensor::get(SensorType::Map);
	if (!mapSensor.Valid || mapSensor.Value < 10 || mapSensor.Value > 300) {
		return false;
	}
	
	// Check TPS sensor
	auto tpsSensor = Sensor::get(SensorType::Tps1);
	if (!tpsSensor.Valid || tpsSensor.Value < 0 || tpsSensor.Value > 100) {
		return false;
	}
	
	// Check lambda sensor
	auto lambda = Sensor::get(SensorType::Lambda1);
	if (!lambda.Valid || lambda.Value < 0.5f || lambda.Value > 3.5f) {
		return false;
	}
	
	// Check target lambda
	float targetLambda = engine->fuelComputer.targetLambda;
	if (targetLambda < 0.5f || targetLambda > 1.5f) {
		return false;
	}
	
	// Check RPM
	auto rpm = Sensor::getOrZero(SensorType::Rpm);
	if (rpm < 100) {
		return false;
	}
	
	// Additional validation: Check if tau and beta are reasonable
	float tau = computeTau();
	float beta = computeBeta();
	if (tau < 0.01f || tau > 10.0f || beta < 0.01f || beta > 1.0f) {
		return false;
	}
	
	// Validate Aquino parameters are configured
	if (engineConfiguration->wwAquinoAnalysisMaxDuration < 0.5f ||
		engineConfiguration->wwAquinoTransMinDuration < 0.1f ||
		engineConfiguration->wwAquinoTransMinDuration >= engineConfiguration->wwAquinoAnalysisMaxDuration) {
		return false;
	}
	
	return true;
}

void WallFuelController::updateStateMachine(float currentTime) {
	WwLearningState oldState = m_adaptiveData.currentState;
	
	// Update transient magnitudes for detection
	m_adaptiveData.transientMagnitude = fabsf(m_adaptiveData.loadDerivative);
	m_adaptiveData.tpsTransientMagnitude = fabsf(m_adaptiveData.tpsDerivative);
	
	switch (m_adaptiveData.currentState) {
		case WwLearningState::IDLE:
			// Look for transients
			detectTransients(currentTime);
			if (m_adaptiveData.mapTransientDetected || m_adaptiveData.tpsTransientDetected) {
				m_adaptiveData.transientType = classifyTransientType();
				m_adaptiveData.transientStartTime = currentTime;
				captureOperatingConditions(true); // Initial conditions
				m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_DETECTED, currentTime);
			}
			break;
			
		case WwLearningState::TRANSIENT_DETECTED:
			// Calculate Aquino model parameters and start beta collection
			{
				float tau = computeTau();
				m_adaptiveData.currentTau = tau;
				
				// Aquino Model W_beta window: 0 to min(0.2s, 0.5*tau)
				m_adaptiveData.wBetaEndTime = fminf(0.2f, 0.5f * tau);
				
				// Aquino Model W_tau window: 2.5*tau duration (after W_beta)
				float maxTauDuration = engineConfiguration->wwAquinoAnalysisMaxDuration - m_adaptiveData.wBetaEndTime;
				m_adaptiveData.wTauDuration = fminf(2.5f * tau, maxTauDuration);
				
				// Ideal settle time for tau analysis
				m_adaptiveData.settleTimeIdeal = tau * engineConfiguration->wwAquinoTauIdealFactor;
				
				// Calculate target buffer sizes based on actual sampling rate
				// Assume onFastCallback runs at ~200Hz (5ms intervals)
				const float samplingRate = 200.0f;
				
				// Beta buffer: W_beta duration * sampling rate
				int betaTargetSize = (int)(m_adaptiveData.wBetaEndTime * samplingRate);
				betaTargetSize = fminf(betaTargetSize, WW_IMMEDIATE_BUFFER_SIZE);
				
				// Tau buffer: W_tau duration * sampling rate  
				int tauTargetSize = (int)(m_adaptiveData.wTauDuration * samplingRate);
				m_adaptiveData.prolongedBufferSizeTarget = fminf(tauTargetSize, WW_PROLONGED_BUFFER_SIZE_MAX);
				
				// Reset buffers and counters
				m_adaptiveData.immediateBufferCount = 0;
				m_adaptiveData.prolongedBufferCount = 0;
				m_adaptiveData.immediateBufferIndex = 0;
				m_adaptiveData.prolongedBufferIndex = 0;
				
				m_adaptiveData.transitionToState(WwLearningState::COLLECTING_BETA, currentTime);
			}
			break;
			
		case WwLearningState::COLLECTING_BETA:
			// Check if beta collection window is complete
			if (m_adaptiveData.getPhaseElapsedTime(currentTime) >= m_adaptiveData.wBetaEndTime) {
				// Check minimum transient duration before proceeding to tau
				if (m_adaptiveData.getTransientDuration(currentTime) >= engineConfiguration->wwAquinoTransMinDuration) {
					m_adaptiveData.transitionToState(WwLearningState::COLLECTING_TAU, currentTime);
				} else {
					// Transient too short, timeout
					m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_TIMEOUT, currentTime);
				}
			}
			// Check for new transient interruption
			else if (m_adaptiveData.mapTransientDetected || m_adaptiveData.tpsTransientDetected) {
				// New transient detected, restart
				m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_TIMEOUT, currentTime);
			}
			// Check for maximum duration timeout
			else if (m_adaptiveData.getTransientDuration(currentTime) > engineConfiguration->wwAquinoAnalysisMaxDuration) {
				m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_TIMEOUT, currentTime);
			}
			break;
			
		case WwLearningState::COLLECTING_TAU:
			// Check if tau collection window is complete
			if (m_adaptiveData.getPhaseElapsedTime(currentTime) >= m_adaptiveData.wTauDuration ||
				m_adaptiveData.prolongedBufferCount >= m_adaptiveData.prolongedBufferSizeTarget) {
				// Tau collection complete, analyze response
				captureOperatingConditions(false); // Final conditions
				m_adaptiveData.transitionToState(WwLearningState::ANALYZING_RESPONSE, currentTime);
			}
			// Check for new transient interruption
			else if (m_adaptiveData.mapTransientDetected || m_adaptiveData.tpsTransientDetected) {
				// New transient detected, apply beta-only correction
				m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_TIMEOUT, currentTime);
			}
			// Check for maximum duration timeout
			else if (m_adaptiveData.getTransientDuration(currentTime) > engineConfiguration->wwAquinoAnalysisMaxDuration) {
				m_adaptiveData.transitionToState(WwLearningState::TRANSIENT_TIMEOUT, currentTime);
			}
			break;
			
		case WwLearningState::ANALYZING_RESPONSE:
			// Analysis is quick, immediately proceed to corrections
			m_adaptiveData.transitionToState(WwLearningState::APPLYING_CORRECTIONS, currentTime);
			break;
			
		case WwLearningState::APPLYING_CORRECTIONS:
			// Corrections applied, return to idle
			m_adaptiveData.completedLearningCycles++;
			m_adaptiveData.transitionToState(WwLearningState::IDLE, currentTime);
			break;
			
		case WwLearningState::TRANSIENT_TIMEOUT:
			// Timeout handled, return to idle
			m_adaptiveData.timeoutTransients++;
			m_adaptiveData.transitionToState(WwLearningState::IDLE, currentTime);
			break;
			
		case WwLearningState::ERROR_STATE:
			// Stay in error state until conditions improve
			if (validateSensorData()) {
				m_adaptiveData.transitionToState(WwLearningState::IDLE, currentTime);
			}
			break;
	}
	
	// Log state transitions
	if (oldState != m_adaptiveData.currentState) {
		logStateTransition(oldState, m_adaptiveData.currentState, currentTime);
	}
}

void WallFuelController::processCurrentState(float currentTime) {
	switch (m_adaptiveData.currentState) {
		case WwLearningState::IDLE:
			// Nothing to process in idle state
			break;
			
		case WwLearningState::TRANSIENT_DETECTED:
			// State transition will handle this
			break;
			
		case WwLearningState::COLLECTING_BETA:
		case WwLearningState::COLLECTING_TAU:
			// Collect lambda data if in collection phase
			{
				auto lambda = Sensor::get(SensorType::Lambda1);
				float targetLambda = engine->fuelComputer.targetLambda;
				if (lambda.Valid && targetLambda > 0) {
					float lambdaError = lambda.Value - targetLambda;
					collectLambdaData(lambdaError, currentTime);
				}
			}
			break;
			
		case WwLearningState::ANALYZING_RESPONSE:
			analyzeCollectedResponse();
			break;
			
		case WwLearningState::APPLYING_CORRECTIONS:
			applyCorrections();
			break;
			
		case WwLearningState::TRANSIENT_TIMEOUT:
			handleTransientTimeout(currentTime);
			break;
			
		case WwLearningState::ERROR_STATE:
			// Error state is handled in updateStateMachine
			break;
	}
}

void WallFuelController::detectTransients(float currentTime) {
	(void)currentTime;
	// Use Aquino model thresholds from configuration
	float mapAccelThresh = engineConfiguration->wwAquinoMapAccelThresh;    // kPa/s
	float mapDecelThresh = engineConfiguration->wwAquinoMapDecelThresh;    // kPa/s  
	float tpsAccelThresh = engineConfiguration->wwAquinoAccelThresh;       // %/s
	float tpsDecelThresh = engineConfiguration->wwAquinoDecelThresh;       // %/s
	
	// Reset detection flags
	m_adaptiveData.mapTransientDetected = false;
	m_adaptiveData.tpsTransientDetected = false;
	
	// Check MAP-based transients with hysteresis to avoid noise
	bool mapAccel = m_adaptiveData.loadDerivative > mapAccelThresh;
	bool mapDecel = m_adaptiveData.loadDerivative < mapDecelThresh;
	m_adaptiveData.mapTransientDetected = mapAccel || mapDecel;
	
	// Check TPS-based transients with hysteresis to avoid noise
	bool tpsAccel = m_adaptiveData.tpsDerivative > tpsAccelThresh;
	bool tpsDecel = m_adaptiveData.tpsDerivative < tpsDecelThresh;
	m_adaptiveData.tpsTransientDetected = tpsAccel || tpsDecel;
	
	// Additional validation: Require minimum magnitude for detection
	if (m_adaptiveData.mapTransientDetected) {
		float mapMagnitude = fabsf(m_adaptiveData.loadDerivative);
		if (mapMagnitude < fabsf(mapAccelThresh) * 0.5f && mapMagnitude < fabsf(mapDecelThresh) * 0.5f) {
			m_adaptiveData.mapTransientDetected = false;
		}
	}
	
	if (m_adaptiveData.tpsTransientDetected) {
		float tpsMagnitude = fabsf(m_adaptiveData.tpsDerivative);
		if (tpsMagnitude < fabsf(tpsAccelThresh) * 0.5f && tpsMagnitude < fabsf(tpsDecelThresh) * 0.5f) {
			m_adaptiveData.tpsTransientDetected = false;
		}
	}
	
	// Store magnitudes for debugging and state machine decisions
	m_adaptiveData.transientMagnitude = fabsf(m_adaptiveData.loadDerivative);
	m_adaptiveData.tpsTransientMagnitude = fabsf(m_adaptiveData.tpsDerivative);
}

WwTransientType WallFuelController::classifyTransientType() const {
	bool mapAccel = m_adaptiveData.loadDerivative > engineConfiguration->wwAquinoMapAccelThresh;
	bool mapDecel = m_adaptiveData.loadDerivative < engineConfiguration->wwAquinoMapDecelThresh;
	bool tpsAccel = m_adaptiveData.tpsDerivative > engineConfiguration->wwAquinoAccelThresh;
	bool tpsDecel = m_adaptiveData.tpsDerivative < engineConfiguration->wwAquinoDecelThresh;
	
	// Determine overall direction (prioritize TPS, fallback to MAP)
	bool isAcceleration = false;
	bool isDeceleration = false;
	
	if (m_adaptiveData.tpsTransientDetected) {
		// Use TPS for direction detection (more direct driver intent)
		isAcceleration = tpsAccel;
		isDeceleration = tpsDecel;
	} else if (m_adaptiveData.mapTransientDetected) {
		// Fallback to MAP for direction detection
		isAcceleration = mapAccel;
		isDeceleration = mapDecel;
	}
	
	// Check for conflicting signals (mixed transient)
	bool tpsMapConflict = (tpsAccel && mapDecel) || (tpsDecel && mapAccel);
	
	if (tpsMapConflict) {
		return WwTransientType::MIXED;
	} else if (isAcceleration) {
		return WwTransientType::ACCELERATION;
	} else if (isDeceleration) {
		return WwTransientType::DECELERATION;
	} else {
		return WwTransientType::NONE;
	}
}

void WallFuelController::collectLambdaData(float lambdaError, float currentTime) {
	(void)currentTime;
	// Validate lambda error
	if (std::isnan(lambdaError) || fabsf(lambdaError) > 1.0f) {
		return; // Invalid lambda error
	}
	
	switch (m_adaptiveData.currentState) {
		case WwLearningState::COLLECTING_BETA:
			// Collect data for beta correction (immediate response)
			if (m_adaptiveData.immediateBufferCount < WW_IMMEDIATE_BUFFER_SIZE) {
				int idx = m_adaptiveData.immediateBufferIndex;
				if (idx >= 0 && idx < WW_IMMEDIATE_BUFFER_SIZE) {
					m_adaptiveData.immediateLambdaBuffer[idx] = lambdaError;
					m_adaptiveData.immediateBufferIndex = (idx + 1) % WW_IMMEDIATE_BUFFER_SIZE;
					m_adaptiveData.immediateBufferCount++;
				}
			}
			break;
			
		case WwLearningState::COLLECTING_TAU:
			// Collect data for tau correction (prolonged response)
			if (m_adaptiveData.prolongedBufferCount < m_adaptiveData.prolongedBufferSizeTarget &&
				m_adaptiveData.prolongedBufferCount < WW_PROLONGED_BUFFER_SIZE_MAX) {
				int idx = m_adaptiveData.prolongedBufferIndex;
				if (idx >= 0 && idx < WW_PROLONGED_BUFFER_SIZE_MAX) {
					m_adaptiveData.prolongedLambdaBuffer[idx] = lambdaError;
					m_adaptiveData.prolongedBufferIndex = (idx + 1) % WW_PROLONGED_BUFFER_SIZE_MAX;
					m_adaptiveData.prolongedBufferCount++;
					
					// Continuously capture final conditions during tau collection
					captureOperatingConditions(false);
				}
			}
			break;
			
		default:
			// Not in a collection state
			break;
	}
}

void WallFuelController::captureOperatingConditions(bool isInitial) {
	auto rpm = Sensor::getOrZero(SensorType::Rpm);
	auto map = Sensor::getOrZero(SensorType::Map);
	
	if (rpm < 100 || map < 10) {
		return; // Invalid conditions
	}
	
	if (isInitial) {
		// Capture initial conditions for beta correction
		m_adaptiveData.initialTransientRpm = rpm;
		m_adaptiveData.initialTransientMap = map;
	} else {
		// Capture final conditions for tau correction
		m_adaptiveData.finalTransientRpm = rpm;
		m_adaptiveData.finalTransientMap = map;
	}
}

void WallFuelController::analyzeCollectedResponse() {
	// Calculate average immediate lambda error for beta correction
	if (m_adaptiveData.immediateBufferCount > 0) {
		float sum = 0.0f;
		int validSamples = 0;
		
		int maxSamples = fminf(m_adaptiveData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
		for (int i = 0; i < maxSamples; i++) {
			float sample = m_adaptiveData.immediateLambdaBuffer[i];
			if (!std::isnan(sample)) {
				sum += sample;
				validSamples++;
			}
		}
		
		if (validSamples > 0) {
			m_adaptiveData.avgImmediateLambdaError = sum / validSamples;
		}
	}
	
	// Calculate average prolonged lambda error for tau correction
	if (m_adaptiveData.prolongedBufferCount > 0) {
		float sum = 0.0f;
		int validSamples = 0;
		
		int maxSamples = fminf(m_adaptiveData.prolongedBufferCount, WW_PROLONGED_BUFFER_SIZE_MAX);
		for (int i = 0; i < maxSamples; i++) {
			float sample = m_adaptiveData.prolongedLambdaBuffer[i];
			if (!std::isnan(sample)) {
				sum += sample;
				validSamples++;
			}
		}
		
		if (validSamples > 0) {
			m_adaptiveData.avgProlongedLambdaError = sum / validSamples;
		}
	}
	
	// Perform settling analysis for tau correction
	if (m_adaptiveData.prolongedBufferCount >= 10) {
		performSettlingAnalysis();
	}
	
	// Calculate corrections
	m_adaptiveData.calculatedBetaCorrection = calculateBetaCorrection();
	m_adaptiveData.calculatedTauCorrection = calculateTauCorrection();
}

float WallFuelController::calculateBetaCorrection() {
	// Use the average immediate lambda error
	float avgLambdaError = m_adaptiveData.avgImmediateLambdaError;
	
	// Get Aquino beta parameters from configuration
	float betaAccelGain = engineConfiguration->wwAquinoBetaAccelGain;
	float betaDecelGain = engineConfiguration->wwAquinoBetaDecelGain;
	float betaLeanThresh = engineConfiguration->wwAquinoBetaLeanThresh;
	float betaRichThresh = engineConfiguration->wwAquinoBetaRichThresh;
	float minLambdaErr = engineConfiguration->wwAquinoMinLambdaErr;
	
	// Protect against NaN input
	if (std::isnan(avgLambdaError)) {
		return 1.0f; // No correction for invalid input
	}
	
	// Always work with absolute error magnitude for robust comparisons
	float errorMagnitude = fabsf(avgLambdaError);
	bool isLean = avgLambdaError > 0.0f;  // Positive error = lean condition
	bool isRich = avgLambdaError < 0.0f;  // Negative error = rich condition
	
	// Check if error is significant enough for adaptation
	if (errorMagnitude < minLambdaErr) {
		return 1.0f; // Error too small, no correction needed
	}
	
	float deltaCorrection = 0.0f;
	
	// Apply physics-correct beta correction based on transient type
	switch (m_adaptiveData.transientType) {
		case WwTransientType::ACCELERATION:
			// ACCELERATION: More fuel hits walls during acceleration
			if (isLean && errorMagnitude > betaLeanThresh) {
				// Too lean during acceleration, need more beta
				deltaCorrection = betaAccelGain * errorMagnitude;
			} else if (isRich && errorMagnitude > fabsf(betaRichThresh)) {
				// Too rich during acceleration, need less beta
				deltaCorrection = -betaAccelGain * errorMagnitude;
			}
			break;
			
		case WwTransientType::DECELERATION:
			// DECELERATION: Less fuel hits walls, existing fuel evaporates
			if (isRich && errorMagnitude > fabsf(betaRichThresh)) {
				// Too rich during deceleration, decrease beta
				deltaCorrection = -betaDecelGain * errorMagnitude;
			} else if (isLean && errorMagnitude > betaLeanThresh) {
				// Too lean during deceleration, increase beta
				deltaCorrection = betaDecelGain * errorMagnitude;
			}
			break;
			
		case WwTransientType::MIXED:
		case WwTransientType::NONE:
		default:
			// Use conservative acceleration logic for mixed/unknown transients
			if (isLean && errorMagnitude > betaLeanThresh) {
				deltaCorrection = betaAccelGain * errorMagnitude * 0.5f; // Conservative
			} else if (isRich && errorMagnitude > fabsf(betaRichThresh)) {
				deltaCorrection = -betaAccelGain * errorMagnitude * 0.5f; // Conservative
			}
			break;
	}
	
	// Convert delta correction to multiplicative correction factor
	float correction = 1.0f + deltaCorrection;
	
	// Protect against NaN and clamp to reasonable bounds
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	return fmaxf(0.5f, fminf(2.0f, correction));
}

float WallFuelController::calculateTauCorrection() {
	// Get Aquino tau parameters from configuration
	float toleranceFactor = engineConfiguration->wwAquinoTauToleranceFactor;
	float overshootMinMag = engineConfiguration->wwAquinoTauOvershootMinMag;
	float overshootGain = engineConfiguration->wwAquinoTauOvershootGain;
	float slowGain = engineConfiguration->wwAquinoTauSlowGain;
	float fastGain = engineConfiguration->wwAquinoTauFastGain;
	
	// Need sufficient data for analysis
	if (m_adaptiveData.prolongedBufferCount < 20) {
		return 1.0f; // Not enough data for Aquino analysis
	}
	
	float deltaCorrection = 0.0f;
	
	// Aquino Model Tau Correction Logic:
	if (m_adaptiveData.hasOvershoot) {
		// Scenario A: Overshoot detected - tau is too low
		float overshootRatio = m_adaptiveData.overshootMagnitude / overshootMinMag;
		deltaCorrection = overshootGain * overshootRatio;
	} else {
		// Scenario B: No significant overshoot - analyze settling time
		float settleIdeal = m_adaptiveData.settleTimeIdeal;
		float settleToleranceMargin = toleranceFactor * settleIdeal;
		float settleMeasured = m_adaptiveData.settleTime;
		
		if (settleMeasured > (settleIdeal + settleToleranceMargin)) {
			// Settling too slow - tau is too high
			float slowRatio = (settleMeasured - settleIdeal) / settleIdeal;
			deltaCorrection = -slowGain * slowRatio; // Negative = decrease tau
		} else if (settleMeasured < (settleIdeal - settleToleranceMargin)) {
			// Settling too fast - tau is too low
			float fastRatio = (settleIdeal - settleMeasured) / settleIdeal;
			deltaCorrection = fastGain * fastRatio; // Positive = increase tau
		}
		// If within tolerance, no correction needed (deltaCorrection = 0)
	}
	
	// Convert delta correction to multiplicative correction factor
	float correction = 1.0f + deltaCorrection;
	
	// Protect against NaN and clamp to reasonable bounds
	if (std::isnan(correction)) {
		return 1.0f;
	}
	
	return fmaxf(0.5f, fminf(2.0f, correction));
}

void WallFuelController::applyCorrections() {
	// Apply both beta and tau corrections to tables
	float betaCorrection = m_adaptiveData.calculatedBetaCorrection;
	float tauCorrection = m_adaptiveData.calculatedTauCorrection;
	
	// Use initial conditions for beta, final conditions for tau (with fallback)
	float betaRpm = m_adaptiveData.initialTransientRpm;
	float betaMap = m_adaptiveData.initialTransientMap;
	
	// Apply corrections to tables
	applyCorrectionToTable(betaCorrection, tauCorrection, betaRpm, betaMap);
	
	// Mark corrections as applied
	m_adaptiveData.correctionsApplied = true;
	
	// Log correction application
	efiPrintf("WW_CORRECTION_APPLIED: beta=%.3f, tau=%.3f, rpm=%.0f, map=%.1f, cycles=%d", 
		betaCorrection, tauCorrection, betaRpm, betaMap, m_adaptiveData.completedLearningCycles + 1);
}

void WallFuelController::handleTransientTimeout(float currentTime) {
	// For timeout transients, apply beta-only correction if we have immediate data
	if (m_adaptiveData.immediateBufferCount > 0) {
		// Calculate beta correction from immediate data
		float sum = 0.0f;
		int validSamples = 0;
		
		int maxSamples = fminf(m_adaptiveData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
		for (int i = 0; i < maxSamples; i++) {
			float sample = m_adaptiveData.immediateLambdaBuffer[i];
			if (!std::isnan(sample)) {
				sum += sample;
				validSamples++;
			}
		}
		
		if (validSamples > 0) {
			m_adaptiveData.avgImmediateLambdaError = sum / validSamples;
			float betaCorrection = calculateBetaCorrection();
			
			// Apply beta-only correction (tau = 1.0f means no tau correction)
			applyCorrectionToTable(betaCorrection, 1.0f, 
				m_adaptiveData.initialTransientRpm, m_adaptiveData.initialTransientMap);
			
			efiPrintf("WW_TIMEOUT_CORRECTION: beta=%.3f, tau=1.000, rpm=%.0f, map=%.1f, reason=timeout", 
				betaCorrection, m_adaptiveData.initialTransientRpm, m_adaptiveData.initialTransientMap);
		}
	}
}

void WallFuelController::handleErrorState(const char* errorReason) {
	efiPrintf("WW_ERROR: %s, state=%s, time=%.1f", 
		errorReason, m_adaptiveData.getStateName(), m_learningTimer.getElapsedSeconds());
	
	// Reset transient data but keep statistics
	m_adaptiveData.resetTransientData();
}

void WallFuelController::logStateTransition(WwLearningState oldState, WwLearningState newState, float currentTime) {
	efiPrintf("WW_STATE_TRANSITION: %s -> %s, time=%.3f, transient=%s", 
		getStateName(oldState), m_adaptiveData.getStateName(), currentTime, 
		m_adaptiveData.getTransientTypeName());
}

void WallFuelController::logPeriodicStatus(float currentTime) {
	auto clt = Sensor::get(SensorType::Clt);
	auto rpm = Sensor::getOrZero(SensorType::Rpm);
	
	efiPrintf("WW_STATUS: state=%s, enable=%d, adaptive=%d, CLT=%.1f, RPM=%.0f, cycles=%d/%d/%d", 
		m_adaptiveData.getStateName(),
		m_enable ? 1 : 0,
		engineConfiguration->wwEnableAdaptiveLearning ? 1 : 0,
		clt.value_or(0),
		rpm,
		m_adaptiveData.completedLearningCycles,
		m_adaptiveData.incompleteTransients,
		m_adaptiveData.errorStateCount);
	
	// Log transient detection details if active
	if (m_adaptiveData.isTransientActive()) {
		efiPrintf("WW_TRANSIENT_DETAIL: MAP_deriv=%.1f, TPS_deriv=%.1f, duration=%.3f, beta_samples=%d, tau_samples=%d", 
			m_adaptiveData.loadDerivative,
			m_adaptiveData.tpsDerivative,
			m_adaptiveData.getTransientDuration(currentTime),
			m_adaptiveData.immediateBufferCount,
			m_adaptiveData.prolongedBufferCount);
	}
}

const char* WallFuelController::getStateName(WwLearningState state) const {
	switch (state) {
		case WwLearningState::IDLE: return "IDLE";
		case WwLearningState::TRANSIENT_DETECTED: return "TRANSIENT_DETECTED";
		case WwLearningState::COLLECTING_BETA: return "COLLECTING_BETA";
		case WwLearningState::COLLECTING_TAU: return "COLLECTING_TAU";
		case WwLearningState::ANALYZING_RESPONSE: return "ANALYZING_RESPONSE";
		case WwLearningState::APPLYING_CORRECTIONS: return "APPLYING_CORRECTIONS";
		case WwLearningState::TRANSIENT_TIMEOUT: return "TRANSIENT_TIMEOUT";
		case WwLearningState::ERROR_STATE: return "ERROR_STATE";
		default: return "UNKNOWN";
	}
}

void WallFuelController::onSlowCallback() {
	// Empty implementation as requested
}

void WallFuelController::applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map) {
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	// Cross-coupling correction to reduce instability when both parameters are being corrected
	// When beta and tau corrections are both significant, reduce their magnitude to prevent oscillations
	float cross_coupling = 1.0f - (0.2f * fabsf(betaCorrection - tauCorrection));
	cross_coupling = fmaxf(0.5f, fminf(1.0f, cross_coupling)); // Clamp between 0.5 and 1.0
	
	// Apply cross-coupling factor to both corrections
	betaCorrection = 1.0f + (betaCorrection - 1.0f) * cross_coupling;
	tauCorrection = 1.0f + (tauCorrection - 1.0f) * cross_coupling;
	
	// Use the same approach as LTFT - getBin() instead of findIndexMsg()
	auto binMap = priv::getBin(map, config->wwCorrectionMapBins);
	auto binRpm = priv::getBin(rpm, config->wwCorrectionRpmBins);
	
	int mapIdx = binMap.Idx;
	int rpmIdx = binRpm.Idx;
	
	// Bounds check - getBin() already handles this, but double-check for safety
	if (mapIdx < 0 || mapIdx >= WWAE_CORRECTION_SIZE - 1 || 
		rpmIdx < 0 || rpmIdx >= WWAE_CORRECTION_SIZE - 1) {
		return; // Invalid indices
	}
	
	// Apply beta correction to INITIAL transient conditions (where transient started)
	if (betaCorrection != 1.0f && !std::isnan(betaCorrection) && m_adaptiveData.initialTransientRpm > 0) {
		auto initialBinMap = priv::getBin(m_adaptiveData.initialTransientMap, config->wwCorrectionMapBins);
		auto initialBinRpm = priv::getBin(m_adaptiveData.initialTransientRpm, config->wwCorrectionRpmBins);
		
		int initialMapIdx = initialBinMap.Idx;
		int initialRpmIdx = initialBinRpm.Idx;
		
		if (initialMapIdx >= 0 && initialMapIdx < WWAE_CORRECTION_SIZE - 1 && 
			initialRpmIdx >= 0 && initialRpmIdx < WWAE_CORRECTION_SIZE - 1) {
			
			// Apply beta correction directly (no autoscale multiplication needed)
			float currentBetaCorrection = config->wwBetaCorrection[initialMapIdx][initialRpmIdx];
			
			// Protect against NaN in calculations
			if (!std::isnan(currentBetaCorrection)) {
				float newBetaCorrection = currentBetaCorrection * betaCorrection;
				
				// Additional NaN check after multiplication
				if (!std::isnan(newBetaCorrection)) {
					// Clamp to reasonable bounds
					newBetaCorrection = fmaxf(0.5f, fminf(2.0f, newBetaCorrection));
					config->wwBetaCorrection[initialMapIdx][initialRpmIdx] = newBetaCorrection;
					
					// Apply smoothing to adjacent cells
					smoothCorrectionTable(initialMapIdx, initialRpmIdx, betaCorrection, 1.0f);
				}
			}
		}
	}
	
	// Apply tau correction to FINAL transient conditions (where transient ended)
	if (tauCorrection != 1.0f && !std::isnan(tauCorrection) && m_adaptiveData.finalTransientRpm > 0) {
		/*
		auto finalBinMap = priv::getBin(m_adaptiveData.finalTransientMap, config->wwCorrectionMapBins);
		auto finalBinRpm = priv::getBin(m_adaptiveData.finalTransientRpm, config->wwCorrectionRpmBins);
		
		int finalMapIdx = finalBinMap.Idx;
		int finalRpmIdx = finalBinRpm.Idx;
		*/

		auto initialBinMap = priv::getBin(m_adaptiveData.initialTransientMap, config->wwCorrectionMapBins);
		auto initialBinRpm = priv::getBin(m_adaptiveData.initialTransientRpm, config->wwCorrectionRpmBins);
		
		int initialMapIdx = initialBinMap.Idx;
		int initialRpmIdx = initialBinRpm.Idx;
		
		if (initialMapIdx >= 0 && initialMapIdx < WWAE_CORRECTION_SIZE - 1 && 
			initialRpmIdx >= 0 && initialRpmIdx < WWAE_CORRECTION_SIZE - 1) {
			
			// Apply tau correction directly (no autoscale multiplication needed)
			float currentTauCorrection = config->wwTauCorrection[initialMapIdx][initialRpmIdx];
			
			// Protect against NaN in calculations
			if (!std::isnan(currentTauCorrection)) {
				float newTauCorrection = currentTauCorrection * tauCorrection;
				
				// Additional NaN check after multiplication
				if (!std::isnan(newTauCorrection)) {
					// Clamp to reasonable bounds
					newTauCorrection = fmaxf(0.5f, fminf(2.0f, newTauCorrection));
					config->wwTauCorrection[initialMapIdx][initialRpmIdx] = newTauCorrection;
					
					// Apply smoothing to adjacent cells
					smoothCorrectionTable(initialMapIdx, initialRpmIdx, 1.0f, tauCorrection);
				}
			}
		}
	}
}

void WallFuelController::performSettlingAnalysis() {
	// Empty implementation as requested
}