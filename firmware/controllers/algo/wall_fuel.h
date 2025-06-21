/*
 * @file wall_fuel.h
 *
 */

#pragma once

#include "wall_fuel_state_generated.h"
#include "engine_module.h"
#include <rusefi/timer.h>

// Forward declaration for neural wall wetting controller
class NeuralWallWettingController;

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
	virtual void onActualFuelInjection(float injectedMass, int cylinderIndex = 0) = 0;
};

// Buffer sizes and constants
#define WW_LOAD_BUFFER_SIZE 8
#define WW_IMMEDIATE_BUFFER_SIZE 40  // Beta: primeiros 200ms (40 amostras a 200Hz)
#define WW_PROLONGED_BUFFER_SIZE_MAX 1000 // Tau: máximo para tau=5s (1000 amostras a 200Hz)
#define WW_TAU_MULTIPLIER 3.0f       // Coleta dados por 3×tau (captura ~95% do efeito)

// Wall Wetting Adaptive State Machine
enum class WwAdaptiveState {
	IDLE,                    // Monitoring for transients
	TRANSIENT_DETECTED,      // Capturing transient conditions
	DELAY_LAMBDA,            // Waiting for lambda sensor excitation
	GATHERING_IMMEDIATE,     // Collecting immediate response (0-200ms, beta)
	GATHERING_PROLONGED,     // Collecting prolonged response (200ms-3*tau, tau)
	LEARNING_ANALYSIS,       // Analyzing data and calculating corrections
	APPLYING_CORRECTION,     // Updating correction tables
	SAVING                   // Persisting configuration
};

// State-specific data structures
struct TransientData {
	bool isPositive = false;
	float magnitude = 0;
	float startTime = 0;
	float initialRpm = 0;
	float initialMap = 0;
	float currentTau = 1.0f;
	
	void reset() {
		isPositive = false;
		magnitude = 0;
		startTime = 0;
		initialRpm = 0;
		initialMap = 0;
		currentTau = 1.0f;
	}
};

struct GatheringData {
	// Immediate phase (beta)
	float immediateLambdaBuffer[WW_IMMEDIATE_BUFFER_SIZE];
	int immediateBufferCount = 0;
	int immediateBufferIndex = 0;
	
	// Prolonged phase (tau)
	float prolongedLambdaBuffer[WW_PROLONGED_BUFFER_SIZE_MAX];
	int prolongedBufferCount = 0;
	int prolongedBufferIndex = 0;
	int prolongedBufferTarget = 0;
	
	// Phase timing
	float phaseStartTime = 0;
	float prolongedPhaseDuration = 0;
	
	// Final conditions (used for debug)
	float finalRpm = 0;
	float finalMap = 0;
	
	void reset() {
		immediateBufferCount = 0;
		immediateBufferIndex = 0;
		prolongedBufferCount = 0;
		prolongedBufferIndex = 0;
		prolongedBufferTarget = 0;
		phaseStartTime = 0;
		prolongedPhaseDuration = 0;
		finalRpm = 0;
		finalMap = 0;
	}
};

struct LearningData {
	// Simplified: always adapt both beta and tau when data is available
	int transientCounter = 0;
	int currentCycleCount = 0;
	
	float avgImmediateLambdaError = 0;
	
	int completedLearningCycles = 0;
	
	void resetAdaptationCycle() {
		transientCounter = 0;
		currentCycleCount = 0;
	}
};

struct CorrectionData {
	float betaCorrection = 1.0f;
	float tauCorrection = 1.0f;
	float targetRpm = 0;
	float targetMap = 0;
	
	void reset() {
		betaCorrection = 1.0f;
		tauCorrection = 1.0f;
		targetRpm = 0;
		targetMap = 0;
	}
};

// Shared data for load monitoring
struct LoadMonitoringData {
	float loadBuffer[WW_LOAD_BUFFER_SIZE];
	int bufferIndex = 0;
	float loadDerivative = 0;
	float transientMagnitude = 0;
	
	void reset() {
		bufferIndex = 0;
		loadDerivative = 0;
		transientMagnitude = 0;
		for (int i = 0; i < WW_LOAD_BUFFER_SIZE; i++) {
			loadBuffer[i] = 0;
		}
	}
};

// Main state machine class
class WwAdaptiveStateMachine {
private:
	WwAdaptiveState m_currentState = WwAdaptiveState::IDLE;
	
	// State-specific data
	TransientData m_transientData;
	GatheringData m_gatheringData;
	LearningData m_learningData;
	CorrectionData m_correctionData;
	LoadMonitoringData m_loadData;
	
	// Callback-based timing (200Hz = 5ms per callback)
	static constexpr float CALLBACK_FREQUENCY_HZ = 200.0f;
	static constexpr float CALLBACK_PERIOD_SEC = 1.0f / CALLBACK_FREQUENCY_HZ; // 0.005s = 5ms
	
	uint32_t m_callbackCounter = 0;          // Total callbacks since start
	uint32_t m_stateStartCallback = 0;       // Callback when current state started
	
	// Configuration parameters in callback counts (converted from time)
	uint32_t m_lambdaDelayCallbacks = 20;        // 100ms delay = 20 callbacks
	uint32_t m_immediatePhaseCallbacks = 40;     // 200ms phase = 40 callbacks  
	uint32_t m_incompleteTimeoutCallbacks = 2000; // INCREASED: 10s timeout = 2000 callbacks
	uint32_t m_globalTimeoutCallbacks = 6000;    // 30s timeout = 6000 callbacks
	
	// Debug and monitoring
	uint32_t m_debugTransientsDetected = 0;
	uint32_t m_debugCorrectionsApplied = 0;
	uint32_t m_debugResetCount = 0;
	float m_lastTransientMagnitude = 0;
	
	// State handlers
	void handleIdleState();
	void handleTransientDetectedState();
	void handleDelayLambdaState();
	void handleGatheringImmediateState();
	void handleGatheringProlongedState();
	void handleLearningAnalysisState();
	void handleApplyingCorrectionState();
	void handleSavingState();
	
	// Helper methods
	void transitionTo(WwAdaptiveState newState);
	void resetToIdle();
	void updateSharedData();
	bool detectTransient();
	bool captureTransientConditions();
	void collectLambdaData(float lambdaError);
	bool shouldAdaptBeta() const;
	bool shouldAdaptTau() const;
	void updateAdaptationMode();
	float calculateBetaCorrection(float avgImmediateLambdaError);
	float calculateTauCorrection();
	void applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map);
	void smoothCorrectionTable(int mapIdx, int rpmIdx, float betaCorrection, float tauCorrection);
	const char* getStateString() const;
	const char* getStateString(WwAdaptiveState state) const;
	uint32_t getElapsedCallbacks() const { return m_callbackCounter - m_stateStartCallback; }
	float getElapsedSeconds() const { return getElapsedCallbacks() * CALLBACK_PERIOD_SEC; }
	
public:
	void initialize();
	void update(); // Now uses callback counter instead of timer
	void onIgnitionHandler(bool ignitionOn);
	WwAdaptiveState getCurrentState() const { return m_currentState; }
	
	// Debug access methods
	uint32_t getDebugTransientsDetected() const { return m_debugTransientsDetected; }
	uint32_t getDebugCorrectionsApplied() const { return m_debugCorrectionsApplied; }
	uint32_t getDebugResetCount() const { return m_debugResetCount; }
	float getLastTransientMagnitude() const { return m_lastTransientMagnitude; }
	uint32_t getCallbackCounter() const { return m_callbackCounter; }
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

protected:
	float computeTau() const;
	float computeBeta() const;

private:
	bool m_enable = false;
	float m_alpha = 0;
	float m_beta = 0;
	
	// Adaptive learning state machine (now timer-independent)
	WwAdaptiveStateMachine m_stateMachine;
	bool m_ignitionState = false;
	bool m_pendingSave = false;
	uint32_t m_ignitionOffCallbacks = 0; // Callback counter for ignition off delay
	uint32_t m_slowCallbackCounter = 0;  // Counter for slow callback (saves after 1000 callbacks = 5s)
	
	// Integration with injection system
	void onActualFuelInjection(float injectedMass, int cylinderIndex = 0) override;
};