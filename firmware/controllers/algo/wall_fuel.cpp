/*
 * @file wall_fuel.cpp
 *
 * @author Matthew Kennedy
 */

#include "pch.h"
#include "wall_fuel.h"
#include "neural_network_engine.h"

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
	auto map = Sensor::get(SensorType::Map).value_or(60);
	auto rpm = Sensor::get(SensorType::Rpm).value_or(1000);

	// First apply CLT compensation (2D)
	float tauClt = interpolate2d(
		clt,
		config->wwCltBins,
		config->wwTauCltValues
	);

	// Apply 3D MAP x RPM base table (like LTFT and other implementations)
	float tauBase = interpolate3d(config->wwTauMapRpmValues,
	                             config->wwMapBins, map,     // First axis (Y/rows)
	                             config->wwRpmBins, rpm);    // Second axis (X/columns)

	// Apply adaptive correction (if enabled)
	float tauCorrection = 1.0f;
	if (engineConfiguration->wwEnableAdaptiveLearning) {
		tauCorrection = interpolate3d(config->wwTauCorrection,
		                             config->wwCorrectionMapBins, map,
		                             config->wwCorrectionRpmBins, rpm);
	}

	return tauClt * tauBase * tauCorrection;
}

float WallFuelController::computeBeta() const {
	if (!engineConfiguration->complexWallModel) {
		return engineConfiguration->wwaeBeta;
	}

	// Default to normal operating temperature in case of
	// CLT failure, this is not critical to get perfect
	float clt = Sensor::get(SensorType::Clt).value_or(90);
	auto map = Sensor::get(SensorType::Map).value_or(60);
	auto rpm = Sensor::get(SensorType::Rpm).value_or(1000);

	// CLT compensation (2D)
	float betaClt = interpolate2d(
		clt,
		config->wwCltBins,
		config->wwBetaCltValues
	);

	// 3D MAP x RPM base table
	float betaBase = interpolate3d(config->wwBetaMapRpmValues,
	                              config->wwMapBins, map,     
	                              config->wwRpmBins, rpm);    

	// Adaptive correction
	float betaCorrection = 1.0f;
	if (engineConfiguration->wwEnableAdaptiveLearning) {
		betaCorrection = interpolate3d(config->wwBetaCorrection,
		                              config->wwCorrectionMapBins, map,
		                              config->wwCorrectionRpmBins, rpm);
	}

	float result = betaClt * betaBase * betaCorrection;
	return clampF(0, result, 1); // Beta cannot exceed 100%
}

// Neural network integration methods
float WallFuelController::getBetaWithNeuralCorrection(float rpm, float load) const {
	float baseBeta = computeBeta();
	
	// Apply neural correction if enabled and available
	if (engineConfiguration->neuralLearningEnabled && m_neural_integration_active) {
		float neuralCorrection = getNeuralBetaCorrection(rpm, load);
		if (isNeuralCorrectionValid() && isfinite(neuralCorrection)) {
			baseBeta *= neuralCorrection;
		}
	}
	
	return clampF(0, baseBeta, 1); // Beta cannot exceed 100%
}

float WallFuelController::getTauWithNeuralCorrection(float rpm, float load) const {
	float baseTau = computeTau();
	
	// Apply neural correction if enabled and available
	if (engineConfiguration->neuralLearningEnabled && m_neural_integration_active) {
		float neuralCorrection = getNeuralTauCorrection(rpm, load);
		if (isNeuralCorrectionValid() && isfinite(neuralCorrection)) {
			baseTau *= neuralCorrection;
		}
	}
	
	return maxF(0.01f, baseTau); // Tau must be positive
}

float WallFuelController::getNeuralBetaCorrection(float rpm, float load) const {
	// Use cached value if recent enough (update every 100ms)
	if (m_correction_cache_timer.hasElapsedSec(0.1f)) {
		updateNeuralCorrections();
	}
	
	return m_cached_beta_correction;
}

float WallFuelController::getNeuralTauCorrection(float rpm, float load) const {
	// Use cached value if recent enough (update every 100ms)  
	if (m_correction_cache_timer.hasElapsedSec(0.1f)) {
		updateNeuralCorrections();
	}
	
	return m_cached_tau_correction;
}

void WallFuelController::updateNeuralCorrections() {
	if (!m_neural_coordinator || !engineConfiguration->neuralLearningEnabled) {
		m_cached_beta_correction = 1.0f;
		m_cached_tau_correction = 1.0f;
		return;
	}
	
	auto rpm = Sensor::get(SensorType::Rpm);
	auto map = Sensor::get(SensorType::Map);
	
	if (!rpm.Valid || !map.Valid) {
		return;
	}
	
	// Get neural corrections from coordinator
	auto& wallWettingController = m_neural_coordinator->getWallWettingController();
	
	m_cached_beta_correction = wallWettingController.getBetaCorrection(rpm.Value, map.Value);
	m_cached_tau_correction = wallWettingController.getTauCorrection(rpm.Value, map.Value);
	
	// Clamp corrections to reasonable bounds
	m_cached_beta_correction = clampF(0.5f, m_cached_beta_correction, 2.0f);
	m_cached_tau_correction = clampF(0.5f, m_cached_tau_correction, 2.0f);
	
	m_correction_cache_timer.reset();
}

bool WallFuelController::isNeuralCorrectionValid() const {
	return m_neural_coordinator && 
	       m_neural_coordinator->isSystemHealthy() &&
	       engineConfiguration->neuralLearningEnabled;
}

void WallFuelController::onFastCallback() {
	// disable wall wetting cranking
	// TODO: is this correct? Why not correct for cranking?
	if (engine->rpmCalculator.isCranking()) {
		m_enable = false;
		return;
	}

	// Initialize neural coordinator if not done yet
	if (!m_neural_coordinator && engineConfiguration->neuralLearningEnabled) {
		m_neural_coordinator = g_neural_coordinator;
		m_neural_integration_active = (m_neural_coordinator != nullptr);
	}

	// Compute tau and beta with neural corrections if available
	float tau, beta;
	
	if (m_neural_integration_active && engineConfiguration->neuralLearningEnabled) {
		auto rpm = Sensor::get(SensorType::Rpm);
		auto map = Sensor::get(SensorType::Map);
		
		if (rpm.Valid && map.Valid) {
			tau = getTauWithNeuralCorrection(rpm.Value, map.Value);
			beta = getBetaWithNeuralCorrection(rpm.Value, map.Value);
		} else {
			// Fallback to classical computation
			tau = computeTau();
			beta = computeBeta();
		}
	} else {
		// Classical computation
		tau = computeTau();
		beta = computeBeta();
	}

	// if tau or beta is really small, we get div/0.
	// you probably meant to disable wwae.
	if (tau < 0.01f || beta < 0.01f) {
		m_enable = false;
		return;
	}

	auto rpm = Sensor::getOrZero(SensorType::Rpm);

	// Ignore low RPM
	if (rpm < 100) {
		m_enable = false;
		return;
	}

	float alpha = expf_taylor(-120 / (rpm * tau));

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
}

void WallFuelController::onSlowCallback() {
	// Update neural corrections periodically
	if (m_neural_integration_active) {
		updateNeuralCorrections();
	}
}

void WallFuelController::onIgnitionStateChanged(bool ignitionOn) {
	if (ignitionOn) {
		// Initialize neural integration
		m_neural_coordinator = g_neural_coordinator;
		m_neural_integration_active = (m_neural_coordinator != nullptr);
		m_cached_beta_correction = 1.0f;
		m_cached_tau_correction = 1.0f;
		m_correction_cache_timer.reset();
	} else {
		// Save neural learned data if available
		if (m_neural_coordinator) {
			// Neural coordinator will handle saving via its ignition callback
		}
	}
}