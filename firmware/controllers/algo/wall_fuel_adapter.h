#pragma once

#include "pch.h"
#include "engine_configuration.h"
#include "periodic_task.h"
#include "engine_math.h"
#include "sensor.h"

// Forward declaration
class WallFuelController;

// Size of the buffer for transient data
#define WW_ADAPT_BUFFER_SIZE 200 // Stores ~2 seconds at 100Hz/10ms interval

// Structure to hold data for one sample point during a transient
typedef struct {
	float timestamp;
	float lambda;
	float map;
	float rpm;
	float m_fi; // Injected fuel mass (or commanded fuel mass from previous cycle)
	float m_ac; // Air mass per cylinder
	float clt; // Coolant temperature
} WwAdaptSample;

class WallFuelAdapter : public PeriodicTimerController {
public:
	WallFuelAdapter();
	void init(WallFuelController *wallFuelController);

	// Periodic function called to check for transients and run adaptation
	void PeriodicTask() override;

	// Getters for adapted correction factors
	float getTauCorrection() const;
	float getXCorrection() const;

private:
	WallFuelController *m_wallFuelController = nullptr;

	// Transient Detection State
	bool m_isTransientActive = false;
	uint32_t m_bufferIndex = 0;
	float m_transientStartTime = 0;

	// Data Buffer
	WwAdaptSample m_dataBuffer[WW_ADAPT_BUFFER_SIZE];

	// Adaptation State
	float m_tauCorrection = 0; // Multiplicative correction factor (applied as 1 + correction)
	float m_xCorrection = 0;   // Multiplicative correction factor (applied as 1 + correction)

	// Methods
	void detectTransient(float currentTime, float currentLambda, float currentMap);
	void startDataBuffering(float currentTime);
	void bufferData(float currentTime, float currentLambda, float currentMap, float currentRpm, float currentMfi, float currentMac, float currentClt);
	void stopDataBufferingAndAdapt(float currentTime);
	bool checkPmanVariation();
	bool estimateParameters(float& tauEstimate, float& xEstimate);
	float estimateDelayMs(float rpm);
	void updateCorrections(float tauEstimate, float xEstimate, float avgRpm, float avgMap, float avgClt);
};

// Function to get the global WallFuelAdapter instance
WallFuelAdapter* getWallFuelAdapter();

