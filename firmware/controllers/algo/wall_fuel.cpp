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

	// Adaptive learning system - only if enabled and warmed up
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
	// Only learn when engine is warm enough
	auto clt = Sensor::get(SensorType::Clt);
	if (!clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return;
	}
	
	// Get current load (MAP)
	auto mapSensor = Sensor::get(SensorType::Map);
	if (!mapSensor.Valid) {
		return;
	}
	float currentLoad = mapSensor.Value;
	
	// Update load derivative calculation
	updateLoadDerivative(currentLoad);
	
	// Update transient magnitude continuously for accurate detection
	m_adaptiveData.transientMagnitude = fabsf(m_adaptiveData.loadDerivative);
	
	// Detect transients based on load derivative
	detectTransients();
	
	// Update lambda response collection if in a collection phase
	auto lambda = Sensor::get(SensorType::Lambda1);
	auto targetLambda = engine->fuelComputer.targetLambda;
	
	if (lambda.Valid && 
		lambda.Value > 0.5f && lambda.Value < 3.5f &&
		targetLambda > 0.5f && targetLambda < 1.5f) {
		
		float lambdaError = lambda.Value - targetLambda;
		float currentTime = m_learningTimer.getElapsedSeconds();
		
		updateLambdaResponse(lambdaError, currentTime);
	}
}

void WallFuelController::onIgnitionStateChanged(bool ignitionOn) {
	m_ignitionState = ignitionOn;
	
	if (ignitionOn) {
		// Reset timers and adaptive data when ignition turns on
		m_learningTimer.reset();
		m_ignitionOffTimer.reset();
		m_adaptiveData.reset();
		m_adaptiveData.resetAdaptationCycle(); // Reset adaptation cycle on ignition
		
		// Ensure all critical values are properly initialized
		m_adaptiveData.adaptationCycleLength = 10;  // 10 transients per cycle
		m_adaptiveData.betaAdaptationCycles = 5;    // 5 cycles for beta (50 transients)
		m_adaptiveData.tauAdaptationCycles = 5;     // 5 cycles for tau (50 transients)
		m_adaptiveData.incompleteTimeout = 3.0f;    // 3 seconds timeout
		m_adaptiveData.minTransientDuration = 0.5f; // 500ms minimum duration
		
		m_pendingSave = false;
	} else {
		// When ignition turns off, schedule save if we have updates
		if (engineConfiguration->wwEnableAdaptiveLearning) {
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
	
	// Calculate derivative in kPa/s (assuming onFastCallback runs at 50Hz)
	const float deltaTime = 0.02f * WW_LOAD_BUFFER_SIZE; // 8 samples * 20ms = 160ms
	m_adaptiveData.loadDerivative = (currentLoad - oldLoad) / deltaTime;
	
	m_adaptiveData.lastLoad = currentLoad;
}

void WallFuelController::detectTransients() {
	// Threshold for transient detection (kPa/s)
	const float transientThreshold = 50.0f; // 50 kPa/s
	
	// transientMagnitude is now updated continuously in onFastCallback
	// m_adaptiveData.transientMagnitude = fabsf(m_adaptiveData.loadDerivative);
	
	// Check if we're starting a new transient
	bool wasInTransient = m_adaptiveData.isPositiveTransient || m_adaptiveData.isNegativeTransient;
	
	if (m_adaptiveData.transientMagnitude > transientThreshold) {
		// QUALQUER transiente (positivo ou negativo) afeta AMBOS beta e tau
		// Beta atua imediatamente (0-200ms), tau atua a longo prazo (200ms-3s)
		
		if (m_adaptiveData.loadDerivative > 0) {
			// Positive transient (acceleration)
			if (!m_adaptiveData.isPositiveTransient) {
				// Starting new positive transient
				m_adaptiveData.isPositiveTransient = true;
				m_adaptiveData.isNegativeTransient = false;
				m_adaptiveData.transientStartTime = m_learningTimer.getElapsedSeconds();
				
				// Reset incomplete transient flags
				m_adaptiveData.transientCompleted = false;
				m_adaptiveData.incompleteTransientDetected = false;
				m_adaptiveData.transientDuration = 0;
				
				// Capture INITIAL conditions for beta correction
				auto rpm = Sensor::getOrZero(SensorType::Rpm);
				auto map = Sensor::getOrZero(SensorType::Map);
				m_adaptiveData.initialTransientRpm = rpm;
				m_adaptiveData.initialTransientMap = map;
				
				// Start immediate phase for beta tuning
				startImmediatePhase();
			}
		} else {
			// Negative transient (deceleration)
			if (!m_adaptiveData.isNegativeTransient) {
				// Starting new negative transient
				m_adaptiveData.isNegativeTransient = true;
				m_adaptiveData.isPositiveTransient = false;
				m_adaptiveData.transientStartTime = m_learningTimer.getElapsedSeconds();
				
				// Reset incomplete transient flags
				m_adaptiveData.transientCompleted = false;
				m_adaptiveData.incompleteTransientDetected = false;
				m_adaptiveData.transientDuration = 0;
				
				// Capture INITIAL conditions for beta correction
				auto rpm = Sensor::getOrZero(SensorType::Rpm);
				auto map = Sensor::getOrZero(SensorType::Map);
				m_adaptiveData.initialTransientRpm = rpm;
				m_adaptiveData.initialTransientMap = map;
				
				// Start immediate phase for beta tuning
				startImmediatePhase();
			}
		}
	} else {
		// No significant transient detected
		if (wasInTransient) {
			// Transient just ended - capture FINAL conditions for tau correction
			auto rpm = Sensor::getOrZero(SensorType::Rpm);
			auto map = Sensor::getOrZero(SensorType::Map);
			m_adaptiveData.finalTransientRpm = rpm;
			m_adaptiveData.finalTransientMap = map;
		}
		
		// Don't clear transient flags here - they will be cleared after processing
		// This prevents conflicts with updateLambdaResponse that needs these flags
		// Flags will be cleared in applyAdaptiveCorrections() or applyIncompleteTransientCorrection()
	}
}

void WallFuelController::applyAdaptiveCorrections() {
	// Check if we have valid data from both phases
	bool hasImmediateData = m_adaptiveData.immediateBufferCount > 0;
	bool hasProlongedData = m_adaptiveData.prolongedBufferCount > 0;
	
	if (!hasImmediateData && !hasProlongedData) {
		return;
	}
	
	// Update adaptation mode management
	updateAdaptationMode();
	
	float betaCorrection = 1.0f;
	float tauCorrection = 1.0f;
	
	// Calculate average immediate lambda error for beta correction
	if (hasImmediateData && shouldAdaptBeta()) {
		float sum = 0.0f;
		int validSamples = 0;
		
		// Bounds check for buffer access
		int maxSamples = fminf(m_adaptiveData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
		for (int i = 0; i < maxSamples; i++) {
			if (i >= 0 && i < WW_IMMEDIATE_BUFFER_SIZE) {
				float sample = m_adaptiveData.immediateLambdaBuffer[i];
				if (!std::isnan(sample)) {
					sum += sample;
					validSamples++;
				}
			}
		}
		
		if (validSamples == 0) {
			// No valid samples
			m_adaptiveData.reset();
			return;
		}
		
		m_adaptiveData.avgImmediateLambdaError = sum / validSamples;
		
		// Calculate beta correction based on immediate response
		betaCorrection = calculateBetaCorrection(m_adaptiveData.avgImmediateLambdaError);
	}
	
	// Calculate average prolonged lambda error for tau correction
	if (hasProlongedData && shouldAdaptTau()) {
		float sum = 0.0f;
		int validSamples = 0;
		
		// Bounds check for buffer access
		int maxSamples = fminf(m_adaptiveData.prolongedBufferCount, WW_PROLONGED_BUFFER_SIZE);
		for (int i = 0; i < maxSamples; i++) {
			if (i >= 0 && i < WW_PROLONGED_BUFFER_SIZE) {
				float sample = m_adaptiveData.prolongedLambdaBuffer[i];
				if (!std::isnan(sample)) {
					sum += sample;
					validSamples++;
				}
			}
		}
		
		if (validSamples > 0) {
			m_adaptiveData.avgProlongedLambdaError = sum / validSamples;
			
			// Calculate tau correction based on prolonged response
			tauCorrection = calculateTauCorrection(m_adaptiveData.avgProlongedLambdaError);
		}
	}
	
	// Apply corrections to tables
	applyCorrectionToTable(betaCorrection, tauCorrection, 
		m_adaptiveData.initialTransientRpm, m_adaptiveData.initialTransientMap);
	
	// Debug logging
	if (betaCorrection != 1.0f || tauCorrection != 1.0f) {
		const char* modeStr = (m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BETA_ONLY) ? "BETA" :
							  (m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_TAU_ONLY) ? "TAU" : "BOTH";
		efiPrintf("WW Adaptive [%s]: Beta=%.3f Tau=%.3f RPM=%.0f MAP=%.0f", 
			modeStr, betaCorrection, tauCorrection, 
			m_adaptiveData.initialTransientRpm, m_adaptiveData.initialTransientMap);
	}
	
	// Mark that we need to save configuration
	setNeedToWriteConfiguration();
	
	// Clear transient flags after processing is complete
	m_adaptiveData.isPositiveTransient = false;
	m_adaptiveData.isNegativeTransient = false;
	
	// Reset for next learning cycle
	m_adaptiveData.reset();
}

void WallFuelController::applyIncompleteTransientCorrection() {
	// For incomplete transients, only apply beta correction
	// This handles short accelerations where prolonged phase cannot be captured
	
	if (m_adaptiveData.immediateBufferCount == 0) {
		// No immediate data available
		m_adaptiveData.reset();
		return;
	}
	
	// Update adaptation mode management
	updateAdaptationMode();
	
	// Only apply beta correction if we're in a beta adaptation period
	if (!shouldAdaptBeta()) {
		m_adaptiveData.reset();
		return;
	}
	
	// Calculate average immediate lambda error for beta correction
	float sum = 0.0f;
	int validSamples = 0;
	
	// Bounds check for buffer access
	int maxSamples = fminf(m_adaptiveData.immediateBufferCount, WW_IMMEDIATE_BUFFER_SIZE);
	for (int i = 0; i < maxSamples; i++) {
		if (i >= 0 && i < WW_IMMEDIATE_BUFFER_SIZE) {
			float sample = m_adaptiveData.immediateLambdaBuffer[i];
			if (!std::isnan(sample)) {
				sum += sample;
				validSamples++;
			}
		}
	}
	
	if (validSamples == 0) {
		// No valid samples
		m_adaptiveData.reset();
		return;
	}
	
	m_adaptiveData.avgImmediateLambdaError = sum / validSamples;
	
	// Calculate beta correction based on immediate response
	float betaCorrection = calculateBetaCorrection(m_adaptiveData.avgImmediateLambdaError);
	
	// Apply only beta correction (tau = 1.0f means no tau correction)
	applyCorrectionToTable(betaCorrection, 1.0f, 
		m_adaptiveData.initialTransientRpm, m_adaptiveData.initialTransientMap);
	
	// Mark that we need to save configuration
	setNeedToWriteConfiguration();
	
	// Clear transient flags after processing is complete
	m_adaptiveData.isPositiveTransient = false;
	m_adaptiveData.isNegativeTransient = false;
	
	// Reset for next learning cycle
	m_adaptiveData.reset();
}

float WallFuelController::calculateBetaCorrection(float avgLambdaError) {
	// Beta correction based on immediate lambda response (relative error)
	// Conservative 5% correction rate with ±67% bounds per cycle (0.33 to 3.00)
	const float correctionRate = 0.05f; // 5% correction rate
	const float maxCorrection = 0.67f;   // ±67% bounds (allows 0.33 to 3.00 range)
	
	// Protect against NaN input
	if (std::isnan(avgLambdaError)) {
		return 1.0f; // No correction for invalid input
	}
	
	if (fabsf(avgLambdaError) < 0.02f) {
		return 1.0f; // No correction needed
	}
	
	// Beta correction: 
	// Lambda lean (negative error) -> increase beta (more fuel sticks to walls)
	// Lambda rich (positive error) -> decrease beta (less fuel sticks to walls)
	float correction = 1.0f - (avgLambdaError * correctionRate);
	
	// Protect against NaN in calculation
	if (std::isnan(correction)) {
		return 1.0f; // No correction for invalid calculation
	}
	
	// Clamp correction to reasonable bounds (0.33 to 3.00)
	correction = fmaxf(1.0f - maxCorrection, fminf(1.0f + maxCorrection, correction));
	
	return correction;
}

float WallFuelController::calculateTauCorrection(float avgLambdaError) {
	// Tau correction based on prolonged lambda response (absolute error)
	// Conservative 5% correction rate with ±67% bounds per cycle (0.33 to 3.00)
	const float correctionRate = 0.05f; // 5% correction rate
	const float maxCorrection = 0.67f;   // ±67% bounds (allows 0.33 to 3.00 range)
	
	// Protect against NaN input
	if (std::isnan(avgLambdaError)) {
		return 1.0f; // No correction for invalid input
	}
	
	if (fabsf(avgLambdaError) < 0.05f) {
		return 1.0f; // No correction needed (higher threshold for tau)
	}
	
	// Tau correction:
	// Lambda lean (negative error) -> increase tau (slower evaporation)
	// Lambda rich (positive error) -> decrease tau (faster evaporation)
	float correction = 1.0f - (avgLambdaError * correctionRate);
	
	// Protect against NaN in calculation
	if (std::isnan(correction)) {
		return 1.0f; // No correction for invalid calculation
	}
	
	// Clamp correction to reasonable bounds (0.33 to 3.00)
	correction = fmaxf(1.0f - maxCorrection, fminf(1.0f + maxCorrection, correction));
	
	return correction;
}

void WallFuelController::applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map) {
	if (!engineConfiguration->wwEnableAdaptiveLearning) {
		return;
	}
	
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
		auto finalBinMap = priv::getBin(m_adaptiveData.finalTransientMap, config->wwCorrectionMapBins);
		auto finalBinRpm = priv::getBin(m_adaptiveData.finalTransientRpm, config->wwCorrectionRpmBins);
		
		int finalMapIdx = finalBinMap.Idx;
		int finalRpmIdx = finalBinRpm.Idx;
		
		if (finalMapIdx >= 0 && finalMapIdx < WWAE_CORRECTION_SIZE - 1 && 
			finalRpmIdx >= 0 && finalRpmIdx < WWAE_CORRECTION_SIZE - 1) {
			
			// Apply tau correction directly (no autoscale multiplication needed)
			float currentTauCorrection = config->wwTauCorrection[finalMapIdx][finalRpmIdx];
			
			// Protect against NaN in calculations
			if (!std::isnan(currentTauCorrection)) {
				float newTauCorrection = currentTauCorrection * tauCorrection;
				
				// Additional NaN check after multiplication
				if (!std::isnan(newTauCorrection)) {
					// Clamp to reasonable bounds
					newTauCorrection = fmaxf(0.5f, fminf(2.0f, newTauCorrection));
					config->wwTauCorrection[finalMapIdx][finalRpmIdx] = newTauCorrection;
					
					// Apply smoothing to adjacent cells
					smoothCorrectionTable(finalMapIdx, finalRpmIdx, 1.0f, tauCorrection);
				}
			}
		}
	}
}

void WallFuelController::smoothCorrectionTable(int mapIdx, int rpmIdx, float betaCorrection, float tauCorrection) {
	// Apply smoothing to adjacent cells to avoid sharp transitions
	const float smoothingFactor = 0.3f; // 30% of the main correction
	
	for (int dMap = -1; dMap <= 1; dMap++) {
		for (int dRpm = -1; dRpm <= 1; dRpm++) {
			// Skip the center cell (already corrected)
			if (dMap == 0 && dRpm == 0) continue;
			
			int adjMapIdx = mapIdx + dMap;
			int adjRpmIdx = rpmIdx + dRpm;
			
			// Check bounds
			if (adjMapIdx < 0 || adjMapIdx >= WWAE_CORRECTION_SIZE ||
				adjRpmIdx < 0 || adjRpmIdx >= WWAE_CORRECTION_SIZE) {
				continue;
			}
			
			// Calculate distance-based smoothing factor
			int distance = abs(dMap) + abs(dRpm);
			// Protect against division by zero (though distance should never be 0 here)
			if (distance == 0) continue;
			
			float factor = smoothingFactor / distance;
			
			// Apply smoothed beta correction with NaN protection
			if (betaCorrection != 1.0f && !std::isnan(betaCorrection)) {
				float smoothedBetaCorr = 1.0f + factor * (betaCorrection - 1.0f);
				float oldBeta = config->wwBetaCorrection[adjMapIdx][adjRpmIdx];
				
				// Protect against NaN in calculations
				if (!std::isnan(oldBeta) && !std::isnan(smoothedBetaCorr)) {
					float newBeta = oldBeta * smoothedBetaCorr;
					if (!std::isnan(newBeta)) {
						config->wwBetaCorrection[adjMapIdx][adjRpmIdx] = clampF(0.5f, newBeta, 2.0f);
					}
				}
			}
			
			// Apply smoothed tau correction with NaN protection
			if (tauCorrection != 1.0f && !std::isnan(tauCorrection)) {
				float smoothedTauCorr = 1.0f + factor * (tauCorrection - 1.0f);
				float oldTau = config->wwTauCorrection[adjMapIdx][adjRpmIdx];
				
				// Protect against NaN in calculations
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

// Handle delayed save after ignition off (called from periodic callback)
void WallFuelController::onSlowCallback() {
	if (m_pendingSave && !m_ignitionState) {
		// Save after 5 seconds delay
		if (m_ignitionOffTimer.hasElapsedSec(5.0f)) {
			setNeedToWriteConfiguration();
			m_pendingSave = false;
		}
	}
}

void WallFuelController::startImmediatePhase() {
	// Start collecting immediate response for beta tuning (0-200ms)
	m_adaptiveData.collectingImmediate = true;
	m_adaptiveData.collectingProlonged = false;
	m_adaptiveData.phaseStartTime = m_learningTimer.getElapsedSeconds();
	m_adaptiveData.immediateBufferIndex = 0;
	m_adaptiveData.immediateBufferCount = 0;
	m_adaptiveData.avgImmediateLambdaError = 0;
}

void WallFuelController::startProlongedPhase() {
	// Start collecting prolonged response for tau tuning (200ms-3s)
	m_adaptiveData.collectingImmediate = false;
	m_adaptiveData.collectingProlonged = true;
	m_adaptiveData.phaseStartTime = m_learningTimer.getElapsedSeconds();
	m_adaptiveData.prolongedBufferIndex = 0;
	m_adaptiveData.prolongedBufferCount = 0;
	m_adaptiveData.avgProlongedLambdaError = 0;
}

void WallFuelController::updateLambdaResponse(float lambdaError, float currentTime) {
	// Update transient duration tracking
	if (m_adaptiveData.collectingImmediate || m_adaptiveData.collectingProlonged) {
		m_adaptiveData.transientDuration = currentTime - m_adaptiveData.transientStartTime;
	}
	
	// Immediate phase collection (0-200ms for beta tuning)
	if (m_adaptiveData.collectingImmediate) {
		float timeSincePhaseStart = currentTime - m_adaptiveData.phaseStartTime;
		
		if (timeSincePhaseStart <= 0.2f && m_adaptiveData.immediateBufferCount < WW_IMMEDIATE_BUFFER_SIZE) {
			// Still in immediate phase window
			// Bounds check for buffer access
			if (m_adaptiveData.immediateBufferIndex >= 0 && m_adaptiveData.immediateBufferIndex < WW_IMMEDIATE_BUFFER_SIZE) {
				m_adaptiveData.immediateLambdaBuffer[m_adaptiveData.immediateBufferIndex] = lambdaError;
				m_adaptiveData.immediateBufferIndex = (m_adaptiveData.immediateBufferIndex + 1) % WW_IMMEDIATE_BUFFER_SIZE;
				m_adaptiveData.immediateBufferCount++;
			}
		} else {
			// Immediate phase completed
			m_adaptiveData.collectingImmediate = false;
			
			// Check if transient is still ongoing for prolonged phase
			// Note: Check transient magnitude instead of flags since flags might be cleared
			if (m_adaptiveData.transientMagnitude > 50.0f) {
				// Transient still active - start prolonged phase
				startProlongedPhase();
			} else {
				// Transient ended early - mark as incomplete
				m_adaptiveData.incompleteTransientDetected = true;
				// Apply beta-only correction for incomplete transients
				applyIncompleteTransientCorrection();
			}
		}
	}
	
	// Prolonged phase collection (200ms-3s for tau tuning)
	if (m_adaptiveData.collectingProlonged) {
		float timeSincePhaseStart = currentTime - m_adaptiveData.phaseStartTime;
		
		if (timeSincePhaseStart <= 2.8f && m_adaptiveData.prolongedBufferCount < WW_PROLONGED_BUFFER_SIZE) {
			// Still in prolonged phase window (200ms + 2.8s = 3s total)
			// Bounds check for buffer access
			if (m_adaptiveData.prolongedBufferIndex >= 0 && m_adaptiveData.prolongedBufferIndex < WW_PROLONGED_BUFFER_SIZE) {
				m_adaptiveData.prolongedLambdaBuffer[m_adaptiveData.prolongedBufferIndex] = lambdaError;
				m_adaptiveData.prolongedBufferIndex = (m_adaptiveData.prolongedBufferIndex + 1) % WW_PROLONGED_BUFFER_SIZE;
				m_adaptiveData.prolongedBufferCount++;
			}
		} else {
			// Prolonged phase completed
			m_adaptiveData.collectingProlonged = false;
			m_adaptiveData.transientCompleted = true;
			
			// Apply full correction (both beta and tau)
			applyAdaptiveCorrections();
		}
	}
	
	// Handle incomplete transient timeout
	if ((m_adaptiveData.collectingImmediate || m_adaptiveData.collectingProlonged) && 
		m_adaptiveData.transientDuration > m_adaptiveData.incompleteTimeout) {
		
		// Timeout reached - treat as incomplete transient
		m_adaptiveData.collectingImmediate = false;
		m_adaptiveData.collectingProlonged = false;
		m_adaptiveData.incompleteTransientDetected = true;
		
		// Apply beta-only correction for incomplete transients
		applyIncompleteTransientCorrection();
	}
}

void WallFuelController::updateAdaptationMode() {
	// Increment transient counter
	m_adaptiveData.transientCounter++;
	
	// Debug logging for adaptation state
	efiPrintf("WW Adaptive: Transient %d/%d in %s mode", 
		m_adaptiveData.transientCounter, 
		m_adaptiveData.adaptationCycleLength,
		(m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BETA_ONLY) ? "BETA" : "TAU");
	
	// Check if we need to switch adaptation modes
	if (m_adaptiveData.transientCounter >= m_adaptiveData.adaptationCycleLength) {
		m_adaptiveData.transientCounter = 0;
		m_adaptiveData.currentCycleCount++;
		
		// Determine next adaptation mode based on cycle count
		if (m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BETA_ONLY) {
			if (m_adaptiveData.currentCycleCount >= m_adaptiveData.betaAdaptationCycles) {
				// Switch to tau adaptation
				m_adaptiveData.currentAdaptationMode = WwAdaptiveData::ADAPT_TAU_ONLY;
				m_adaptiveData.currentCycleCount = 0;
				efiPrintf("WW Adaptive: Switching to TAU adaptation period (cycle %d complete)", 
					m_adaptiveData.betaAdaptationCycles);
			}
		} else if (m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_TAU_ONLY) {
			if (m_adaptiveData.currentCycleCount >= m_adaptiveData.tauAdaptationCycles) {
				// Switch back to beta adaptation
				m_adaptiveData.currentAdaptationMode = WwAdaptiveData::ADAPT_BETA_ONLY;
				m_adaptiveData.currentCycleCount = 0;
				efiPrintf("WW Adaptive: Switching to BETA adaptation period (cycle %d complete)", 
					m_adaptiveData.tauAdaptationCycles);
			}
		}
	}
}

bool WallFuelController::shouldAdaptBeta() const {
	return m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BETA_ONLY ||
		   m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BOTH;
}

bool WallFuelController::shouldAdaptTau() const {
	return m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_TAU_ONLY ||
		   m_adaptiveData.currentAdaptationMode == WwAdaptiveData::ADAPT_BOTH;
}

void WallFuelController::onActualFuelInjection(float injectedMass, int cylinderIndex) {
	if (!engineConfiguration->wwEnableAdaptiveLearning || !m_enable) {
		return;
	}
	
	float rpm = Sensor::getOrZero(SensorType::Rpm);
	float map = Sensor::getOrZero(SensorType::Map);
	auto clt = Sensor::get(SensorType::Clt);
	
	// Verificar condições mínimas
	if (rpm < 100 || !clt.Valid || clt.Value < engineConfiguration->wwMinCoolantTemp) {
		return;
	}
	
	// Log para debug (pode ser removido em produção)
	if (engineConfiguration->debugMode == DBG_WALL_WETTING) {
		efiPrintf("WW: Injection cyl=%d mass=%.3f rpm=%.0f map=%.1f", 
				 cylinderIndex, injectedMass, rpm, map);
	}
	
	// Esta função pode ser expandida no futuro para integração com sistema de adaptação
	// Por enquanto, apenas registra a injeção para possível uso futuro
}