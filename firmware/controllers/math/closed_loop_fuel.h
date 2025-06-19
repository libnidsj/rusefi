#pragma once

#include "flash_main.h"
#include "table_helper.h"
#include "engine_module.h"
#include <rusefi/timer.h>

// Forward declarations for neural network integration
class NeuralNetworkCoordinator;
class NeuralLongTermFuelTrim;

struct stft_s;

struct ClosedLoopFuelResult {
	ClosedLoopFuelResult() {
		// Default is no correction, aka 1.0 multiplier
		for (size_t i = 0; i < STFT_BANK_COUNT; i++) {
			banks[i] = 1.0f;
		}
	}

	float banks[STFT_BANK_COUNT];
};

ClosedLoopFuelResult fuelStftClosedLoopCorrection();
size_t computeStftBin(float rpm, float load, stft_s& cfg);
bool shouldUpdateCorrection(SensorType sensor);

class LongTermFuelTrim : public EngineModule{
	float ltftTableHelper[16][16];
	bool ltftTableHelperInit = false;
	float ltftResult = 1;
	bool updatedLtft = false;
	// Variáveis para filtro EMA
	float stftEma = 1.0f;
	bool m_ignitionState = false;
	
	// Neural network integration
	NeuralNetworkCoordinator* m_neural_coordinator;
	NeuralLongTermFuelTrim* m_neural_ltft;
	bool m_neural_integration_active;
	
	// Neural prediction cache
	mutable float m_cached_neural_prediction = 1.0f;
	mutable Timer m_prediction_cache_timer;
	
	bool canLearn();
	float filterStft(float stftRaw);
	
	// Timer-based implementation
	Timer m_updateTimer;            // Controls update frequency
	Timer m_ignitionOnTimer;        // Tracks time since ignition on
	Timer m_ignitionOffTimer;       // Tracks time since ignition off
	bool isLearnConditionsMet = false;
	
	// Neural integration methods
	float getNeuralPrediction(float load, float rpm) const;
	float getLtftWithNeuralEnhancement(float load, float rpm) const;
	void updateNeuralPrediction();
	bool isNeuralPredictionValid() const;
	
public:
	LongTermFuelTrim();
	float getLtft(float load, float rpm);
	void resetLtftTimer();
	void updateLtft(float load, float rpm);
	void onIgnitionStateChanged(bool ignitionState) override;
	
	// Neural network integration interface
	float getLtftWithNeural(float load, float rpm);
	void setNeuralCoordinator(NeuralNetworkCoordinator* coordinator);
};