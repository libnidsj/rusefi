#pragma once

#include "neural_fuel_network.h"
#include "adaptive_wall_wetting.h"
#include "closed_loop_fuel.h"
#include "engine_module.h"
#include <rusefi/timer.h>

/**
 * Neural Fuel Controller - Main integration point for neural fuel system
 * 
 * This controller integrates the neural network with existing AWW and LTFT
 * systems, providing intelligent predictive fuel corrections based on
 * temporal patterns in lambda data.
 * 
 * Integration Strategy:
 * - Collects data at 200Hz (onFastCallback)
 * - Processes neural network at 50Hz (onSlowCallback)
 * - Provides predictions to AWW and LTFT systems
 * - Maintains robust failsafe mechanisms
 */
class NeuralFuelController : public EngineModule {
public:
    using interface_t = NeuralFuelController;
    
    // Constructor/Destructor
    NeuralFuelController();
    
    // EngineModule interface
    void onFastCallback() override;  // 200Hz - Data collection
    void onSlowCallback() override;  // 50Hz - Neural processing
    void onIgnitionStateChanged(bool ignitionOn) override;
    
    // Public interface for diagnostics and integration
    bool isEnabled() const { return m_config.enabled; }
    bool isActive() const { return m_telemetry.isActive; }
    bool isFallbackMode() const { return m_telemetry.fallbackMode; }
    
    float getConfidence() const { return m_telemetry.currentConfidence; }
    float getBetaPrediction() const { return m_telemetry.betaPrediction; }
    float getTauPrediction() const { return m_telemetry.tauPrediction; }
    float getAdaptiveLearningRate() const { return m_telemetry.adaptiveLearningRate; }
    
    // Integration with existing systems
    void integrateWithAdaptiveWW(AdaptiveWallWettingController* aww);
    void integrateWithLTFT(LongTermFuelTrim* ltft);
    
    // Configuration
    void setEnabled(bool enabled) { m_config.enabled = enabled; }
    void setConfidenceThreshold(float threshold) { 
        m_config.confidenceThreshold = clampF(0.1f, threshold, 1.0f);
    }
    void setLearningRate(float rate) {
        m_config.learningRate = clampF(0.0001f, rate, 0.01f);
    }
    
    // Telemetry access
    const neural_fuel_telemetry_s& getTelemetry() const { return m_telemetry; }
    
    // Emergency controls
    void resetNeuralState();
    void enterEmergencyMode();
    void attemptRecovery();

private:
    // =======================================================================
    // MEMBER VARIABLES
    // =======================================================================
    
    // Core neural system components
    neural_fuel_config_s m_config;
    lstm_state_s m_lstmState;
    temporal_buffer_s m_temporalBuffer;
    neural_fuel_telemetry_s m_telemetry;
    neural_weights_s m_weights;
    
    // Timers for different operations
    Timer m_updateTimer;            // Controls neural network updates (50Hz)
    Timer m_dataCollectionTimer;    // Controls data collection timing
    Timer m_ignitionOnTimer;        // Tracks time since ignition on
    Timer m_ignitionOffTimer;       // Tracks time since ignition off
    Timer m_recoveryTimer;          // Recovery attempt timing
    Timer m_stateResetTimer;        // Periodic state reset for stability
    
    // Integration with existing systems
    AdaptiveWallWettingController* m_adaptiveWW = nullptr;
    LongTermFuelTrim* m_ltft = nullptr;
    
    // Neural network working buffers
    float m_currentInputs[NEURAL_INPUT_COUNT];
    int16_t m_quantizedInputs[NEURAL_INPUT_COUNT];
    int16_t m_quantizedOutputs[NEURAL_OUTPUT_COUNT];
    float m_currentOutputs[NEURAL_OUTPUT_COUNT];
    
    // State tracking
    bool m_ignitionState = false;
    bool m_emergencyMode = false;
    uint32_t m_consecutiveFailures = 0;
    float m_lastValidConfidence = 0;
    
    // =======================================================================
    // CORE NEURAL PROCESSING METHODS
    // =======================================================================
    
    // Data collection and preprocessing
    void collectTemporalData();
    void updateTemporalFeatures();
    void prepareNeuralInputs();
    
    // Neural network forward pass
    void processNeuralInference();
    void runLSTMForward();
    void postProcessOutputs();
    
    // Feature computation
    void computeTemporalSlopes();
    void computeStatisticalFeatures();
    void updateContextualInputs();
    
    // =======================================================================
    // QUANTIZATION AND MATH UTILITIES
    // =======================================================================
    
    // Quantization functions
    void quantizeInputs(const float* inputs, int16_t* quantized);
    void dequantizeOutputs(const int16_t* quantized, float* outputs);
    int16_t quantizeFloat(float value, int16_t scale);
    float dequantizeInt(int16_t value, int16_t scale);
    
    // Mathematical utilities
    float computeLinearRegressionSlope(const float* y, const float* x, size_t n);
    float computeVariance(const float* data, size_t n);
    float computeMovingAverageTrend(const float* data, size_t n, size_t windowSize);
    float calculateConfidence(const float* outputs);
    
    // =======================================================================
    // INTEGRATION AND APPLICATION METHODS
    // =======================================================================
    
    // Apply predictions to existing systems
    void applyPredictionsToAWW();
    void applyPredictionsToLTFT();
    void blendWithClassicalSystems();
    
    // Integration helpers
    void collectAWWData();
    void collectLTFTData();
    void updateIntegrationWeights();
    
    // =======================================================================
    // VALIDATION AND SAFETY METHODS
    // =======================================================================
    
    // Operational condition checking
    bool checkOperationalConditions();
    bool validateSensorData();
    bool validateNeuralOutputs();
    bool checkSystemHealth();
    
    // Failsafe and recovery
    void handleNeuralFailure();
    void updateFailureTracking();
    bool shouldAttemptRecovery();
    void performStateReset();
    
    // =======================================================================
    // TELEMETRY AND DIAGNOSTICS
    // =======================================================================
    
    // Telemetry updates
    void updateTelemetry();
    void updateInputTelemetry();
    void updateOutputTelemetry();
    void updateStatusTelemetry();
    
    // Diagnostic helpers
    void logNeuralActivity();
    void trackPerformanceMetrics();
    void updateSuccessStatistics();
    
    // =======================================================================
    // INITIALIZATION AND CONFIGURATION
    // =======================================================================
    
    // Initialization
    void initializeNeuralWeights();
    void initializeConfiguration();
    void resetAllTimers();
    
    // Configuration validation
    bool validateConfiguration();
    void applyConfigurationLimits();
    
    // =======================================================================
    // CONSTANTS AND THRESHOLDS
    // =======================================================================
    
    // Timing constants
    static constexpr float NEURAL_UPDATE_INTERVAL = 0.02f;    // 50Hz
    static constexpr float DATA_COLLECTION_INTERVAL = 0.005f;  // 200Hz
    static constexpr float RECOVERY_ATTEMPT_INTERVAL = 30.0f;  // 30 seconds
    static constexpr float STATE_RESET_INTERVAL = 300.0f;     // 5 minutes
    
    // Safety thresholds
    static constexpr float MIN_LAMBDA_ERROR_THRESHOLD = 0.01f;
    static constexpr float MAX_LAMBDA_ERROR_THRESHOLD = 0.5f;
    static constexpr uint32_t MAX_CONSECUTIVE_FAILURES = 5;
    static constexpr float MIN_OPERATIONAL_CONFIDENCE = 0.2f;
    
    // Integration weights
    static constexpr float NEURAL_WEIGHT_RAMP_RATE = 0.1f;     // Per second
    static constexpr float MAX_NEURAL_INFLUENCE = 0.8f;       // Maximum neural weight
    static constexpr float FALLBACK_THRESHOLD = 0.3f;         // Switch to fallback
    
    // Input normalization ranges
    static constexpr float RPM_NORMALIZATION_MAX = 8000.0f;
    static constexpr float LOAD_NORMALIZATION_MAX = 120.0f;    // kPa
    static constexpr float LAMBDA_ERROR_NORMALIZATION_MAX = 0.3f;
    static constexpr float SLOPE_NORMALIZATION_MAX = 1.0f;
};

// =============================================================================
// LIVE DATA STRUCTURE FOR TUNERSTUDIO INTEGRATION
// =============================================================================

/**
 * Live data structure for neural fuel system telemetry
 * This provides real-time visibility into the neural network operation
 */
struct neural_fuel_live_data_s {
    // Neural network inputs
    float lambdaErrorT1 = 0;
    float lambdaErrorT2 = 0;
    float lambdaErrorT3 = 0;
    float lambdaErrorT4 = 0;
    float lambdaErrorT5 = 0;
    float lambdaSlopeShort = 0;
    float lambdaSlopeLong = 0;
    float lambdaVariance = 0;
    float lambdaTrend = 0;
    float loadNormalized = 0;
    float rpmNormalized = 0;
    float transientMagnitude = 0;
    
    // Neural network outputs
    float betaPrediction = 0;
    float tauPrediction = 0;
    float confidenceScore = 0;
    float learningRate = 0;
    
    // Internal states (first 4 hidden states for visualization)
    float hiddenState0 = 0;
    float hiddenState1 = 0;
    float hiddenState2 = 0;
    float hiddenState3 = 0;
    
    // Performance statistics
    uint32_t totalInferences = 0;
    uint32_t successfulPredictions = 0;
    float successRate = 0;
    uint32_t consecutiveFailures = 0;
    
    // Integration status
    float neuralWeightAWW = 0;      // Neural influence on AWW
    float neuralWeightLTFT = 0;     // Neural influence on LTFT
    float classicalBetaCorrection = 0;
    float classicalTauCorrection = 0;
    
    // System status
    bool isEnabled = false;
    bool isActive = false;
    bool fallbackMode = false;
    bool bufferReady = false;
    bool awwIntegrated = false;
    bool ltftIntegrated = false;
    bool emergencyMode = false;
    
    // Timing information
    float timeSinceIgnitionOn = 0;
    float timeSinceLastUpdate = 0;
    float timeSinceLastFailure = 0;
};