/**
 * @file neural_network_engine.h
 * 
 * Neural Network Engine for rusefi
 * Quantized LSTM-like architecture for Wall Wetting and LTFT learning
 * 
 * Key Features:
 * - Quantized weights and activations for MCU optimization
 * - Temporal memory with hidden/cell states (LSTM-like)
 * - Automatic slope analysis and temporal feature extraction
 * - Integration with existing Wall Wetting and LTFT systems
 * - Robust failsafe mechanisms with classical fallback
 * - Comprehensive telemetry via live data structures
 */

#pragma once

#include "engine_module.h"
#include "neural_network_state_generated.h"
#include <rusefi/timer.h>

// Neural network configuration constants
#define NN_MAX_TEMPORAL_INPUTS 32
#define NN_MAX_HIDDEN_SIZE 16
#define NN_MAX_OUTPUT_SIZE 8
#define NN_TEMPORAL_BUFFER_SIZE 200  // 1 second at 200Hz
#define NN_QUANTIZATION_BITS 8       // 8-bit quantization for MCU

// Quantized data types for neural network
using q8_t = int8_t;     // Quantized 8-bit signed integer
using q16_t = int16_t;   // Quantized 16-bit for intermediate calculations

/**
 * Neural Network Application Types
 */
enum class NeuralNetworkApp : uint8_t {
    WALL_WETTING_BETA = 0,    // Wall wetting beta parameter learning
    WALL_WETTING_TAU = 1,     // Wall wetting tau parameter learning  
    LTFT_CORRECTION = 2,      // Long term fuel trim learning
    DISABLED = 255
};

/**
 * Neural Network State for State Machine
 */
enum class NeuralNetworkState : uint8_t {
    IDLE = 0,                 // Waiting for conditions
    COLLECTING_DATA,          // Collecting temporal data
    PROCESSING_FEATURES,      // Computing temporal features
    NEURAL_INFERENCE,         // Running neural network
    APPLYING_CORRECTION,      // Applying learned correction
    VALIDATION,              // Validating correction
    COOLDOWN                 // Cooldown period
};

/**
 * Temporal Feature Types (extracted automatically)
 */
enum class TemporalFeature : uint8_t {
    LAMBDA_ERROR_T1 = 0,     // Lambda error at T-100ms
    LAMBDA_ERROR_T2,         // Lambda error at T-200ms
    LAMBDA_ERROR_T3,         // Lambda error at T-300ms
    LAMBDA_ERROR_T4,         // Lambda error at T-400ms
    LAMBDA_ERROR_T5,         // Lambda error at T-500ms
    LAMBDA_SLOPE_SHORT,      // Short-term slope (100ms window)
    LAMBDA_SLOPE_LONG,       // Long-term slope (500ms window)
    LAMBDA_VARIANCE,         // Lambda variance in window
    LAMBDA_MEAN,             // Lambda mean in window
    TPS_DERIVATIVE,          // TPS rate of change
    MAP_DERIVATIVE,          // MAP rate of change
    FEATURE_COUNT            // Total number of features
};

/**
 * Quantized Neural Network Layer
 */
struct QuantizedLayer {
    q8_t weights[NN_MAX_HIDDEN_SIZE][NN_MAX_TEMPORAL_INPUTS];
    q8_t biases[NN_MAX_HIDDEN_SIZE];
    q8_t scale_factor;        // Scale factor for dequantization
    uint8_t input_size;
    uint8_t output_size;
};

/**
 * LSTM-like Memory Cell
 */
struct LSTMCell {
    q8_t hidden_state[NN_MAX_HIDDEN_SIZE];      // h(t)
    q8_t cell_state[NN_MAX_HIDDEN_SIZE];        // c(t)
    q8_t forget_gate[NN_MAX_HIDDEN_SIZE];       // f(t) 
    q8_t input_gate[NN_MAX_HIDDEN_SIZE];        // i(t)
    q8_t output_gate[NN_MAX_HIDDEN_SIZE];       // o(t)
    bool initialized;
};

/**
 * Temporal Analysis Buffer for Slope Calculation
 */
struct TemporalBuffer {
    float lambda_errors[NN_TEMPORAL_BUFFER_SIZE];
    float timestamps[NN_TEMPORAL_BUFFER_SIZE];   // In seconds
    uint16_t index;
    uint16_t count;
    bool full;
    
    void addSample(float lambda_error, float timestamp);
    float calculateSlope(float window_seconds) const;
    float calculateVariance(float window_seconds) const;
    float calculateMean(float window_seconds) const;
    void reset();
};

/**
 * Neural Network Correction Results
 */
struct NeuralCorrection {
    float multiplier;         // Correction multiplier (e.g., 1.05 = +5%)
    float confidence;         // Confidence level [0.0-1.0]
    bool valid;              // Whether correction is valid
    uint32_t sample_count;   // Number of samples used
};

/**
 * Temporal Feature Extractor
 */
class TemporalAnalyzer {
public:
    void addSample(float lambda_error);
    void extractFeatures(float features[static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT)]);
    void reset();
    
    // Get specific temporal values
    float getLambdaErrorAtTime(float seconds_ago) const;
    float getShortTermSlope() const;
    float getLongTermSlope() const;
    
private:
    TemporalBuffer m_buffer;
    Timer m_timer;
    
    // Linear regression for slope calculation
    float calculateLinearRegression(const float* x, const float* y, uint16_t count) const;
};

/**
 * Quantized Neural Network Engine
 */
class NeuralNetworkEngine {
public:
    NeuralNetworkEngine();
    
    // Core neural network operations
    void initializeNetwork(NeuralNetworkApp app);
    NeuralCorrection processInputs(const float* features, uint8_t feature_count);
    void updateWeights(const float* features, float target_output, float learning_rate);
    
    // Quantization operations
    q8_t quantizeFloat(float value, float scale_factor) const;
    float dequantizeQ8(q8_t value, float scale_factor) const;
    
    // LSTM operations
    void forwardLSTM(const q8_t* inputs, uint8_t input_size);
    void resetLSTMState();
    
    // Validation and failsafe
    bool validateOutput(const NeuralCorrection& correction) const;
    void enableFailsafe() { m_failsafe_active = true; }
    void disableFailsafe() { m_failsafe_active = false; }
    bool isFailsafeActive() const { return m_failsafe_active; }
    
private:
    // Network architecture
    QuantizedLayer m_input_layer;
    QuantizedLayer m_hidden_layer;
    QuantizedLayer m_output_layer;
    LSTMCell m_lstm_cell;
    
    // Current application
    NeuralNetworkApp m_current_app;
    
    // Failsafe state
    bool m_failsafe_active;
    uint32_t m_consecutive_failures;
    
    // Internal operations
    void sigmoidActivation(q8_t* values, uint8_t size) const;
    void tanhActivation(q8_t* values, uint8_t size) const;
    void matrixMultiply(const q8_t* input, const q8_t weights[][NN_MAX_TEMPORAL_INPUTS], 
                       q8_t* output, uint8_t rows, uint8_t cols) const;
};

/**
 * Neural Wall Wetting Controller
 * Integrates with existing WallFuelController
 */
class NeuralWallWettingController : public EngineModule {
public:
    using interface_t = NeuralWallWettingController;
    
    void onFastCallback() override;
    void onSlowCallback() override;
    void onIgnitionStateChanged(bool ignitionOn) override;
    
    // Integration interface
    float getBetaCorrection(float rpm, float load) const;
    float getTauCorrection(float rpm, float load) const;
    
    // State queries
    NeuralNetworkState getState() const { return m_state; }
    bool isLearning() const { return m_state != NeuralNetworkState::IDLE; }
    uint32_t getLearningCycles() const { return m_learning_cycles; }
    
private:
    neural_network_state_s m_neural_state;
    NeuralNetworkState m_state;
    NeuralNetworkEngine m_beta_network;
    NeuralNetworkEngine m_tau_network;
    TemporalAnalyzer m_temporal_analyzer;
    
    Timer m_learning_timer;
    Timer m_cooldown_timer;
    uint32_t m_learning_cycles;
    bool m_processing_active;
    
    // State machine methods
    void processStateMachine();
    void processDataCollection();
    void processFeatureExtraction();
    void processNeuralInference();
    void processApplyCorrection();
    void processValidation();
    void enterCooldown();
    
    // Learning conditions
    bool canStartLearning() const;
    bool detectTransient() const;
    float getCurrentLambdaError() const;
    
    // Table updates
    void updateBetaCorrectionTable(float correction, float rpm, float load);
    void updateTauCorrectionTable(float correction, float rpm, float load);
};

/**
 * Neural Long Term Fuel Trim Enhancement
 * Integrates with existing LTFT system
 */
class NeuralLongTermFuelTrim : public EngineModule {
public:
    using interface_t = NeuralLongTermFuelTrim;
    
    void onFastCallback() override;
    void onSlowCallback() override;
    void onIgnitionStateChanged(bool ignitionOn) override;
    
    // Integration interface
    float getNeuralLTFTCorrection(float rpm, float load) const;
    float getPredictedSTFT(float rpm, float load) const;
    
    // State queries
    bool isLearning() const { return m_learning_active; }
    uint32_t getPredictionAccuracy() const { return m_prediction_accuracy; }
    
private:
    NeuralNetworkEngine m_ltft_network;
    TemporalAnalyzer m_stft_analyzer;
    
    Timer m_learning_timer;
    bool m_learning_active;
    uint32_t m_prediction_accuracy;
    
    // Learning methods
    void processSTFTLearning();
    void updateLTFTPredictin();
    bool validateSTFTPrediction(float predicted, float actual) const;
};

/**
 * Central Neural Network Coordinator
 * Main entry point for all neural network activities
 */
class NeuralNetworkCoordinator : public EngineModule {
public:
    using interface_t = NeuralNetworkCoordinator;
    
    void onFastCallback() override;
    void onSlowCallback() override;
    void onIgnitionStateChanged(bool ignitionOn) override;
    
    // Component access
    NeuralWallWettingController& getWallWettingController() { return m_wall_wetting; }
    NeuralLongTermFuelTrim& getLTFTController() { return m_ltft; }
    
    // System-wide control
    void enableNeuralLearning(bool enable);
    void resetAllNetworks();
    bool isSystemHealthy() const;
    
    // Statistics and diagnostics
    uint32_t getTotalLearningCycles() const;
    float getSystemPerformance() const;
    
private:
    NeuralWallWettingController m_wall_wetting;
    NeuralLongTermFuelTrim m_ltft;
    
    bool m_system_enabled;
    bool m_system_healthy;
    Timer m_health_check_timer;
    
    // System health monitoring
    void performHealthCheck();
    void handleSystemFailure();
};

// Global neural network coordinator instance
extern NeuralNetworkCoordinator* g_neural_coordinator;

// Utility functions
float calculateTargetLambdaError();
bool isEngineInStableCondition();
void logNeuralActivity(const char* component, const char* action, float value);