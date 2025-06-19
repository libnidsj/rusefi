/*
 * @file wall_fuel.h
 *
 */

#pragma once

#include "wall_fuel_state_generated.h"
#include "engine_module.h"
#include <rusefi/timer.h>
#include "adaptive_wall_wetting.h"

// Forward declarations for adaptive wall wetting
enum class AWWTransientState : uint8_t;
struct adaptive_wall_wetting_state_s;
class AdaptiveWallWettingController;

// Forward declarations for neural network integration  
class NeuralNetworkCoordinator;

/**
 * Wall wetting, also known as fuel film
 * See https://github.com/rusefi/rusefi/issues/151 for the theory
 */
class WallFuel : public wall_fuel_state_s {
public:
	/**
	 * @param desiredMassGrams desired fuel quantity, in grams
	 * @return total adjusted fuel squirt mass in grams once wall wetting is taken into effect
	 */
	float adjust(float desiredMassGrams);
	float getWallFuel() const;
	void resetWF();
	int invocationCounter = 0;
};

struct IWallFuelController {
	virtual bool getEnable() const = 0;
	virtual float getAlpha() const = 0;
	virtual float getBeta() const = 0;
};

class WallFuelController : public IWallFuelController, public EngineModule {
public:
	using interface_t = IWallFuelController;

	void onFastCallback() override;
	void onSlowCallback() override;
	void onIgnitionStateChanged(bool ignitionOn) override;

	bool getEnable() const override {
		return m_enable;
	}

	float getAlpha() const override {
		return m_alpha;
	}

	float getBeta() const override {
		return m_beta;
	}
	
	// Neural network integration interfaces
	float getBetaWithNeuralCorrection(float rpm, float load) const;
	float getTauWithNeuralCorrection(float rpm, float load) const;

protected:
	float computeTau() const;
	float computeBeta() const;

private:
	bool m_enable = false;
	float m_alpha = 0;
	float m_beta = 0;
	
	// Adaptive wall wetting controller
	AdaptiveWallWettingController m_adaptiveController;
	
	// Flag to avoid recursion
	bool m_processingAdaptive = false;
	
	// Neural network integration
	NeuralNetworkCoordinator* m_neural_coordinator;
	bool m_neural_integration_active;
	
	// Neural correction cache
	mutable float m_cached_beta_correction = 1.0f;
	mutable float m_cached_tau_correction = 1.0f;
	mutable Timer m_correction_cache_timer;
	
	// Internal methods
	float getNeuralBetaCorrection(float rpm, float load) const;
	float getNeuralTauCorrection(float rpm, float load) const;
	void updateNeuralCorrections();
	bool isNeuralCorrectionValid() const;
};