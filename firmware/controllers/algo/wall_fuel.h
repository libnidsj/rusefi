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

// Wall Wetting Adaptive Learning State Machine
enum class WwLearningState {
	IDLE,                    // Não há transiente ativo, aguardando detecção
	TRANSIENT_DETECTED,      // Transiente detectado, capturando condições iniciais
	COLLECTING_BETA,         // Coletando dados para correção de beta (W_beta: 0-200ms)
	COLLECTING_TAU,          // Coletando dados para correção de tau (W_tau: 200ms-3s)
	ANALYZING_RESPONSE,      // Analisando resposta coletada (settling analysis)
	APPLYING_CORRECTIONS,    // Aplicando correções às tabelas
	TRANSIENT_TIMEOUT,       // Transiente incompleto (timeout)
	ERROR_STATE             // Estado de erro (dados inválidos, sensores falhos)
};

// Transient Type Classification
enum class WwTransientType {
	NONE,           // Nenhum transiente
	ACCELERATION,   // Transiente positivo (aceleração)
	DECELERATION,   // Transiente negativo (desaceleração)
	MIXED          // Transiente misto (MAP e TPS divergem)
};

// Learning Phase for data collection
enum class WwLearningPhase {
	NONE,           // Nenhuma fase ativa
	BETA_WINDOW,    // W_beta: 0 até min(0.2s, 0.5*tau)
	TAU_WINDOW      // W_tau: W_beta_end até W_beta_end + 2.5*tau
};

// Adaptive correction data structure with clear state machine
struct WwAdaptiveData {
	// === STATE MACHINE CORE ===
	WwLearningState currentState = WwLearningState::IDLE;
	WwTransientType transientType = WwTransientType::NONE;
	WwLearningPhase currentPhase = WwLearningPhase::NONE;
	
	// State transition timestamps
	float stateEntryTime = 0;        // Quando entrou no estado atual
	float transientStartTime = 0;    // Quando o transiente foi detectado
	float phaseStartTime = 0;        // Quando a fase atual começou
	
	// === TRANSIENT DETECTION ===
	float loadBuffer[WW_LOAD_BUFFER_SIZE];
	float tpsBuffer[WW_TPS_BUFFER_SIZE];
	int bufferIndex = 0;
	int tpsBufferIndex = 0;
	float lastLoad = 0;
	float lastTps = 0;
	float loadDerivative = 0;        // dMAP/dt em kPa/s
	float tpsDerivative = 0;         // dTPS/dt em %/s
	
	// Transient magnitude tracking
	float transientMagnitude = 0;        // Magnitude do transiente MAP
	float tpsTransientMagnitude = 0;     // Magnitude do transiente TPS
	
	// Detection flags (for debugging)
	bool mapTransientDetected = false;
	bool tpsTransientDetected = false;
	
	// === AQUINO MODEL PARAMETERS ===
	float currentTau = 1.0f;             // Tau atual para este transiente
	float wBetaEndTime = 0.2f;           // Fim da janela W_beta
	float wTauDuration = 2.5f;           // Duração da janela W_tau
	float settleTimeIdeal = 1.5f;        // Tempo ideal de settling (tau * factor)
	
	// === DATA COLLECTION BUFFERS ===
	float immediateLambdaBuffer[WW_IMMEDIATE_BUFFER_SIZE];
	float prolongedLambdaBuffer[WW_PROLONGED_BUFFER_SIZE_MAX];
	int immediateBufferIndex = 0;
	int prolongedBufferIndex = 0;
	int immediateBufferCount = 0;
	int prolongedBufferCount = 0;
	int prolongedBufferSizeTarget = 0;   // Tamanho alvo baseado em tau
	
	// === OPERATING CONDITIONS ===
	float initialTransientRpm = 0;       // Condições no início (para beta)
	float initialTransientMap = 0;
	float finalTransientRpm = 0;         // Condições no fim (para tau)
	float finalTransientMap = 0;
	
	// === RESPONSE ANALYSIS ===
	float avgImmediateLambdaError = 0;   // Erro médio na fase beta
	float avgProlongedLambdaError = 0;   // Erro médio na fase tau
	
	// Aquino settling analysis
	float settleTime = 0;                // Tempo medido de settling
	bool hasOvershoot = false;           // Overshoot detectado
	float overshootMagnitude = 0;        // Magnitude do overshoot
	float overshootDuration = 0;         // Duração do overshoot
	bool settlingAnalysisComplete = false;
	int consecutiveSettledSamples = 0;
	int requiredSettledSamples = 10;     // 50ms a 200Hz
	
	// === CORRECTION RESULTS ===
	float calculatedBetaCorrection = 1.0f;
	float calculatedTauCorrection = 1.0f;
	bool correctionsApplied = false;
	
	// === STATISTICS & DEBUGGING ===
	int completedLearningCycles = 0;
	int incompleteTransients = 0;
	int timeoutTransients = 0;
	int errorStateCount = 0;
	
	// === STATE MACHINE METHODS ===
	void transitionToState(WwLearningState newState, float currentTime) {
		if (currentState != newState) {
			// Log state transition for debugging
			currentState = newState;
			stateEntryTime = currentTime;
			
			// State-specific initialization
			switch (newState) {
				case WwLearningState::IDLE:
					resetTransientData();
					break;
				case WwLearningState::TRANSIENT_DETECTED:
					// Initialization handled in detectTransient()
					break;
				case WwLearningState::COLLECTING_BETA:
					currentPhase = WwLearningPhase::BETA_WINDOW;
					phaseStartTime = currentTime;
					break;
				case WwLearningState::COLLECTING_TAU:
					currentPhase = WwLearningPhase::TAU_WINDOW;
					phaseStartTime = currentTime;
					break;
				case WwLearningState::ANALYZING_RESPONSE:
					currentPhase = WwLearningPhase::NONE;
					break;
				case WwLearningState::APPLYING_CORRECTIONS:
					// Corrections will be calculated and applied
					break;
				case WwLearningState::TRANSIENT_TIMEOUT:
					incompleteTransients++;
					break;
				case WwLearningState::ERROR_STATE:
					errorStateCount++;
					break;
			}
		}
	}
	
	float getTimeInCurrentState(float currentTime) const {
		return currentTime - stateEntryTime;
	}
	
	float getTransientDuration(float currentTime) const {
		return currentTime - transientStartTime;
	}
	
	float getPhaseElapsedTime(float currentTime) const {
		return currentTime - phaseStartTime;
	}
	
	bool isCollectingData() const {
		return currentState == WwLearningState::COLLECTING_BETA || 
		       currentState == WwLearningState::COLLECTING_TAU;
	}
	
	bool isTransientActive() const {
		return currentState != WwLearningState::IDLE && 
		       currentState != WwLearningState::ERROR_STATE;
	}
	
	void resetTransientData() {
		// Reset transient-specific data but keep statistics
		transientType = WwTransientType::NONE;
		currentPhase = WwLearningPhase::NONE;
		
		// Reset timing
		transientStartTime = 0;
		phaseStartTime = 0;
		
		// Reset detection flags
		mapTransientDetected = false;
		tpsTransientDetected = false;
		
		// Reset buffers
		immediateBufferCount = 0;
		prolongedBufferCount = 0;
		immediateBufferIndex = 0;
		prolongedBufferIndex = 0;
		
		// Reset derivatives
		loadDerivative = 0;
		tpsDerivative = 0;
		transientMagnitude = 0;
		tpsTransientMagnitude = 0;
		
		// Reset conditions
		initialTransientRpm = 0;
		initialTransientMap = 0;
		finalTransientRpm = 0;
		finalTransientMap = 0;
		
		// Reset analysis
		avgImmediateLambdaError = 0;
		avgProlongedLambdaError = 0;
		settlingAnalysisComplete = false;
		hasOvershoot = false;
		overshootMagnitude = 0;
		overshootDuration = 0;
		consecutiveSettledSamples = 0;
		
		// Reset corrections
		calculatedBetaCorrection = 1.0f;
		calculatedTauCorrection = 1.0f;
		correctionsApplied = false;
	}
	
	void reset() {
		// Complete reset including statistics
		transitionToState(WwLearningState::IDLE, 0);
		resetTransientData();
		
		// Reset statistics
		completedLearningCycles = 0;
		incompleteTransients = 0;
		timeoutTransients = 0;
		errorStateCount = 0;
		
		// Reset buffers
		bufferIndex = 0;
		tpsBufferIndex = 0;
		lastLoad = 0;
		lastTps = 0;
	}
	
	// Debug helper
	const char* getStateName() const {
		switch (currentState) {
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
	
	const char* getTransientTypeName() const {
		switch (transientType) {
			case WwTransientType::NONE: return "NONE";
			case WwTransientType::ACCELERATION: return "ACCELERATION";
			case WwTransientType::DECELERATION: return "DECELERATION";
			case WwTransientType::MIXED: return "MIXED";
			default: return "UNKNOWN";
		}
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
	void performSettlingAnalysis();
	void applyCorrectionToTable(float betaCorrection, float tauCorrection, float rpm, float map);
	void onSlowCallback() override;

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
	
	// === PRIVATE METHODS ===
	void updateStateMachine(float currentTime);
	void processCurrentState(float currentTime);
	bool validateSensorData() const;
	
	// === TRANSIENT DETECTION ===
	void updateLoadDerivative(float currentLoad);
	void updateTpsDerivative(float currentTps);
	void detectTransients(float currentTime);
	WwTransientType classifyTransientType() const;
	
	// === DATA COLLECTION ===
	void collectLambdaData(float lambdaError, float currentTime);
	void captureOperatingConditions(bool isInitial);
	
	// === RESPONSE ANALYSIS ===
	void analyzeCollectedResponse();
	float calculateBetaCorrection();
	float calculateTauCorrection();
	
	// === CORRECTION APPLICATION ===
	void applyCorrections();
	void smoothCorrectionTable(int mapIdx, int rpmIdx, float betaCorrection, float tauCorrection);
	
	// === TIMEOUT & ERROR HANDLING ===
	void handleTransientTimeout(float currentTime);
	void handleErrorState(const char* errorReason);
	
	// === DEBUGGING & LOGGING ===
	void logStateTransition(WwLearningState oldState, WwLearningState newState, float currentTime);
	void logPeriodicStatus(float currentTime);
	const char* getStateName(WwLearningState state) const;
	
	// === LEGACY METHODS (to be removed) ===
	// Removidos os protótipos das funções legadas
	
	// Integration with injection system
	void onActualFuelInjection(float injectedMass, int cylinderIndex = 0) override;
};