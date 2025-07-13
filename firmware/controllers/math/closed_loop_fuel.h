#pragma once

#include "flash_main.h"
#include "table_helper.h"
#include "engine_module.h"
#include <rusefi/timer.h>

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
	
	// Variáveis para modo simples
	uint32_t lastLtftUpdateTime = 0;
	
	// Timer para modo agressivo
	Timer m_aggressiveModeTimer;
	
public:
	LongTermFuelTrim();
	float getLtft(float load, float rpm);
	void resetLtftTimer();
	void updateLtft(float load, float rpm);
};