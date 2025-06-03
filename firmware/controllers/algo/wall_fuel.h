/*
 * @file wall_fuel.h
 *
 */

#pragma once

#include "wall_fuel_state_generated.h"
#include "engine_module.h"
#include <rusefi/timer.h>
#include "cyclic_buffer.h"

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

// Circular buffer for load derivative calculation
#define WW_LOAD_BUFFER_SIZE 40
#define WW_TPS_BUFFER_SIZE 40         // Buffer for TPS derivative calculation  
#define WW_IMMEDIATE_BUFFER_SIZE 40  // Beta: primeiros 200ms (40 amostras a 200Hz)
#define WW_PROLONGED_BUFFER_SIZE_MAX 1000 // Tau: máximo para tau=5s (1000 amostras a 200Hz)
#define WW_TAU_MULTIPLIER 3.0f       // Coleta dados por 3×tau (captura ~95% do efeito)

// Adaptive correction data structure
struct WwAdaptiveData {
	float loadBuffer[WW_LOAD_BUFFER_SIZE];
	float tpsBuffer[WW_TPS_BUFFER_SIZE];  // TPS buffer for derivative calculation
	int bufferIndex = 0;
	int tpsBufferIndex = 0;               // TPS buffer index
	float lastLoad = 0;
	float lastTps = 0;                    // Last TPS value
	float loadDerivative = 0;
	float tpsDerivative = 0;              // TPS derivative (dTPS_dt)
	
	// Transient detection and timing
	bool isPositiveTransient = false;
	bool isNegativeTransient = false;
	float transientMagnitude = 0;
	float tpsTransientMagnitude = 0;      // TPS-based transient magnitude
	float transientStartTime = 0;
	
	// Aquino model: track both MAP and TPS transients
	bool mapTransientDetected = false;    // MAP transient flag
	bool tpsTransientDetected = false;    // TPS transient flag
	bool combinedTransientActive = false; // Either MAP or TPS transient is active
	
	// Separate buffers for beta (immediate) and tau (prolonged) responses
	float immediateLambdaBuffer[WW_IMMEDIATE_BUFFER_SIZE];  // Beta: 0-200ms
	float prolongedLambdaBuffer[WW_PROLONGED_BUFFER_SIZE_MAX];  // Tau: 200ms-3s
	int immediateBufferIndex = 0;
	int prolongedBufferIndex = 0;
	int immediateBufferCount = 0;
	int prolongedBufferCount = 0;
	
	// Response phase tracking
	bool collectingImmediate = false;  // 0-200ms for beta
	bool collectingProlonged = false;  // 200ms-3s for tau
	float phaseStartTime = 0;
	
	// Dynamic prolonged phase duration based on tau
	float currentTau = 1.0f;           // Current tau value for this transient
	float prolongedPhaseDuration = 0;  // Duration in seconds (Aquino W_beta/W_tau)
	int prolongedBufferSizeTarget = 0; // Target buffer size for current tau
	
	// Average errors for correction calculation
	float avgImmediateLambdaError = 0;  // For beta correction
	float avgProlongedLambdaError = 0;  // For tau correction
	
	// Aquino model: tau settling analysis
	float settleTime = 0;               // Measured settle time
	float settleTimeIdeal = 0;          // Ideal settle time (tau * factor)
	bool hasOvershoot = false;          // Overshoot detected flag
	float overshootMagnitude = 0;       // Maximum overshoot magnitude
	float overshootDuration = 0;        // Duration of overshoot
	bool settlingAnalysisComplete = false; // Settling analysis completed flag
	
	// Timestamp tracking for settling analysis
	int consecutiveSettledSamples = 0;  // Consecutive samples within settle threshold
	int requiredSettledSamples = 10;    // Required samples to confirm settling (50ms at 200Hz)
	
	// Separate conditions for beta (initial) and tau (final) corrections
	float initialTransientRpm = 0;  // Beta: condições no início do transiente
	float initialTransientMap = 0;
	float finalTransientRpm = 0;    // Tau: condições no final do transiente  
	float finalTransientMap = 0;
	
	// Transient completion tracking
	bool transientCompleted = false;
	bool incompleteTransientDetected = false;
	float transientDuration = 0;
	
	// Adaptation mode - always adapt both parameters
	enum AdaptationMode {
		ADAPT_BETA_ONLY,    // Adapt only beta, keep tau fixed
		ADAPT_TAU_ONLY,     // Adapt only tau, keep beta fixed
		ADAPT_BOTH          // Adapt both (always used now)
	};
	
	AdaptationMode currentAdaptationMode = ADAPT_BOTH;  // Always adapt both
	
	// Statistics for debugging
	int interruptedBetaPhases = 0;   // Count of beta phases interrupted by new transients
	int interruptedTauPhases = 0;    // Count of tau phases interrupted by new transients
	int completedLearningCycles = 0; // Count of successfully completed learning cycles
	
	void reset() {
		// Reset all learning state
		isPositiveTransient = false;
		isNegativeTransient = false;
		collectingImmediate = false;
		collectingProlonged = false;
		transientCompleted = false;
		incompleteTransientDetected = false;
		
		// Reset Aquino model transient flags
		mapTransientDetected = false;
		tpsTransientDetected = false;
		combinedTransientActive = false;
		
		// Reset buffers
		immediateBufferCount = 0;
		prolongedBufferCount = 0;
		immediateBufferIndex = 0;
		prolongedBufferIndex = 0;
		tpsBufferIndex = 0;
		
		// Reset timing
		transientStartTime = 0;
		phaseStartTime = 0;
		transientDuration = 0;
		
		// Reset derivatives
		loadDerivative = 0;
		tpsDerivative = 0;
		transientMagnitude = 0;
		tpsTransientMagnitude = 0;
		
		// Reset Aquino settling analysis
		settleTime = 0;
		settleTimeIdeal = 0;
		hasOvershoot = false;
		overshootMagnitude = 0;
		overshootDuration = 0;
		settlingAnalysisComplete = false;
		consecutiveSettledSamples = 0;
		
		// Reset conditions
		initialTransientRpm = 0;
		initialTransientMap = 0;
		finalTransientRpm = 0;
		finalTransientMap = 0;
		
		// Reset errors
		avgImmediateLambdaError = 0;
		avgProlongedLambdaError = 0;
		
		// Note: Don't reset adaptation mode variables here
		// They should persist across individual transient resets
		// Only reset on ignition cycle or manual reset
		
		// Note: Don't reset statistics counters (interruptedBetaPhases, etc.)
		// They should persist to provide debugging information across multiple cycles
	}
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
	
	// Adaptive learning system
	WwAdaptiveData m_adaptiveData;
	Timer m_learningTimer;
	Timer m_ignitionOffTimer;
	bool m_ignitionState = false;
	bool m_pendingSave = false;
	
	// Adaptive learning methods
	void updateLoadDerivative(float currentLoad);
	void updateTpsDerivative(float currentTps);    // TPS derivative calculation
	void detectAquinoTransients();                 // Aquino model transient detection
	void updateLambdaResponse(float lambdaError, float currentTime);
	void startImmediatePhase();
	void startProlongedPhase();
	void applyAdaptiveCorrections();
	void applyIncompleteTransientCorrection();
	void applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map);
	void smoothCorrectionTable(int mapIdx, int rpmIdx, float betaCorrection, float tauCorrection);
	float calculateBetaCorrection(float avgImmediateLambdaError);
	float calculateAquinoTauCorrection();          // Aquino tau correction with settling analysis
	void performSettlingAnalysis(float lambdaError, float currentTime); // Settling analysis
	
	// Integration with injection system
	void onActualFuelInjection(float injectedMass, int cylinderIndex = 0) override;
};