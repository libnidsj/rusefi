#pragma once

#include "engine_module.h"
#include <rusefi/timer.h>

// Forward declarations
class AdaptiveWallWettingController;
class LongTermFuelTrim;

/**
 * Neural Fuel Network - LSTM-based predictive fuel correction system
 * 
 * This system complements existing Adaptive Wall Wetting and LTFT systems
 * by adding temporal learning capabilities through a quantized LSTM network
 * optimized for MCU execution.
 * 
 * Key Features:
 * - 12 temporal inputs (lambda errors T1-T5, slopes, statistics, context)
 * - 4 outputs (beta/tau predictions, confidence, adaptive learning rate)
 * - Quantized LSTM (int16) for efficient MCU execution
 * - Integration with existing AWW and LTFT systems
 * - Robust failsafe mechanisms
 */

// =============================================================================
// NEURAL NETWORK CONFIGURATION
// =============================================================================

// Network architecture constants
static constexpr size_t NEURAL_INPUT_COUNT = 12;
static constexpr size_t NEURAL_HIDDEN_SIZE = 8;
static constexpr size_t NEURAL_OUTPUT_COUNT = 4;

// Buffer sizes
static constexpr size_t TEMPORAL_BUFFER_SIZE = 50;  // 1s at 50Hz
static constexpr size_t NEURAL_WEIGHT_COUNT = 200;  // Total weights in network
static constexpr size_t NEURAL_BIAS_COUNT = 20;     // Total biases in network

// Quantization parameters
static constexpr int16_t WEIGHT_SCALE = 4096;        // 2^12 for weights
static constexpr int16_t ACTIVATION_SCALE = 32767;   // 2^15 for activations
static constexpr int32_t BIAS_SCALE = 4096;          // 2^12 for biases

// =============================================================================
// DATA STRUCTURES
// =============================================================================

/**
 * Configuration for the neural fuel system
 */
struct neural_fuel_config_s {
    bool enabled = false;
    float learningRate = 0.001f;
    float confidenceThreshold = 0.7f;
    uint16_t updateIntervalMs = 20; // 50Hz
    
    // Quantization parameters
    int16_t weightScale = WEIGHT_SCALE;
    int16_t activationScale = ACTIVATION_SCALE;
    
    // Safety limits
    float maxBetaCorrection = 0.5f;
    float maxTauCorrection = 0.5f;
    float minConfidenceThreshold = 0.3f;
};

/**
 * LSTM internal states (quantized for efficiency)
 */
struct lstm_state_s {
    int16_t hiddenState[NEURAL_HIDDEN_SIZE];
    int16_t cellState[NEURAL_HIDDEN_SIZE];
    bool initialized = false;
    
    void reset() {
        for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
            hiddenState[i] = 0;
            cellState[i] = 0;
        }
        initialized = false;
    }
};

/**
 * Temporal buffer for collecting lambda data and computing features
 */
struct temporal_buffer_s {
    // Raw data buffers
    float lambdaErrors[TEMPORAL_BUFFER_SIZE];
    float timestamps[TEMPORAL_BUFFER_SIZE];
    uint8_t currentIndex = 0;
    bool bufferFull = false;
    
    // Computed features (cached for efficiency)
    float lambdaVariance = 0;
    float lambdaTrend = 0;
    float shortSlope = 0;
    float longSlope = 0;
    
    // AWW integration data
    float awwTransientMagnitude = 0;
    uint8_t awwState = 0;
    
    void addSample(float lambdaError, float timestamp) {
        lambdaErrors[currentIndex] = lambdaError;
        timestamps[currentIndex] = timestamp;
        currentIndex = (currentIndex + 1) % TEMPORAL_BUFFER_SIZE;
        if (currentIndex == 0) {
            bufferFull = true;
        }
    }
    
    bool isReady() const {
        return bufferFull;
    }
    
    void reset() {
        currentIndex = 0;
        bufferFull = false;
        lambdaVariance = 0;
        lambdaTrend = 0;
        shortSlope = 0;
        longSlope = 0;
    }
};

/**
 * Neural network weights (quantized)
 */
struct neural_weights_s {
    // LSTM forget gate weights
    int16_t forgetW[NEURAL_HIDDEN_SIZE][NEURAL_HIDDEN_SIZE];  // Hidden to forget
    int16_t forgetU[NEURAL_HIDDEN_SIZE][NEURAL_INPUT_COUNT];  // Input to forget
    int16_t forgetBias[NEURAL_HIDDEN_SIZE];
    
    // LSTM input gate weights
    int16_t inputW[NEURAL_HIDDEN_SIZE][NEURAL_HIDDEN_SIZE];   // Hidden to input
    int16_t inputU[NEURAL_HIDDEN_SIZE][NEURAL_INPUT_COUNT];   // Input to input
    int16_t inputBias[NEURAL_HIDDEN_SIZE];
    
    // LSTM candidate gate weights
    int16_t candidateW[NEURAL_HIDDEN_SIZE][NEURAL_HIDDEN_SIZE]; // Hidden to candidate
    int16_t candidateU[NEURAL_HIDDEN_SIZE][NEURAL_INPUT_COUNT]; // Input to candidate
    int16_t candidateBias[NEURAL_HIDDEN_SIZE];
    
    // LSTM output gate weights
    int16_t outputW[NEURAL_HIDDEN_SIZE][NEURAL_HIDDEN_SIZE];  // Hidden to output
    int16_t outputU[NEURAL_HIDDEN_SIZE][NEURAL_INPUT_COUNT];  // Input to output
    int16_t outputBias[NEURAL_HIDDEN_SIZE];
    
    // Output layer weights
    int16_t outputLayerW[NEURAL_OUTPUT_COUNT][NEURAL_HIDDEN_SIZE];
    int16_t outputLayerBias[NEURAL_OUTPUT_COUNT];
    
    // Initialize with small random values (to be replaced with trained weights)
    void initializeDefault() {
        // For now, initialize with small values
        // In production, these would be loaded from trained model
        for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
            for (size_t j = 0; j < NEURAL_HIDDEN_SIZE; j++) {
                forgetW[i][j] = 100;    // Small positive values
                inputW[i][j] = 100;
                candidateW[i][j] = 100;
                outputW[i][j] = 100;
            }
            for (size_t j = 0; j < NEURAL_INPUT_COUNT; j++) {
                forgetU[i][j] = 50;
                inputU[i][j] = 50;
                candidateU[i][j] = 50;
                outputU[i][j] = 50;
            }
            forgetBias[i] = 1000;      // Slight forget bias
            inputBias[i] = 0;
            candidateBias[i] = 0;
            outputBias[i] = 0;
        }
        
        for (size_t i = 0; i < NEURAL_OUTPUT_COUNT; i++) {
            for (size_t j = 0; j < NEURAL_HIDDEN_SIZE; j++) {
                outputLayerW[i][j] = 50;
            }
            outputLayerBias[i] = 0;
        }
    }
};

/**
 * Telemetry and diagnostic data
 */
struct neural_fuel_telemetry_s {
    // Current predictions
    float currentConfidence = 0;
    float betaPrediction = 0;
    float tauPrediction = 0;
    float adaptiveLearningRate = 0;
    
    // Input features (for diagnostics)
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
    
    // Internal states (for diagnostics)
    float hiddenState0 = 0;
    float hiddenState1 = 0;
    float hiddenState2 = 0;
    float hiddenState3 = 0;
    
    // Statistics
    uint32_t totalInferences = 0;
    uint32_t successfulPredictions = 0;
    float successRate = 0;
    
    // Status flags
    bool isEnabled = false;
    bool isActive = false;
    bool fallbackMode = false;
    bool bufferReady = false;
    
    void updateSuccessRate() {
        if (totalInferences > 0) {
            successRate = (float)successfulPredictions / (float)totalInferences;
        } else {
            successRate = 0;
        }
    }
};

// =============================================================================
// ACTIVATION FUNCTIONS (QUANTIZED)
// =============================================================================

/**
 * Quantized sigmoid function using lookup table for efficiency
 */
inline int16_t quantizedSigmoid(int16_t x) {
    // Lookup table for sigmoid approximation
    // Input range: [-32768, 32767] mapped to sigmoid output [0, 32767]
    static const int16_t sigmoidLUT[] = {
        0, 820, 1638, 2458, 3277, 4096, 4915, 5734, 6553, 7372,
        8192, 9011, 9830, 10649, 11468, 12287, 13107, 13926, 14745, 15564,
        16384, 17203, 18022, 18841, 19660, 20479, 21299, 22118, 22937, 23756,
        24576, 25395, 26214, 27033, 27852, 28671, 29491, 30310, 31129, 31948,
        32767
    };
    
    // Clamp input to lookup table range
    if (x < -20480) return 0;
    if (x > 20480) return 32767;
    
    // Map input to lookup table index
    int index = (x + 20480) / 1024;
    if (index < 0) index = 0;
    if (index >= 41) index = 40;
    
    return sigmoidLUT[index];
}

/**
 * Quantized tanh function using lookup table
 */
inline int16_t quantizedTanh(int16_t x) {
    // Lookup table for tanh approximation
    // Input range: [-32768, 32767] mapped to tanh output [-32767, 32767]
    static const int16_t tanhLUT[] = {
        -32767, -32440, -32113, -31786, -31459, -31132, -30805, -30478, -30151, -29824,
        -29497, -29170, -28843, -28516, -28189, -27862, -27535, -27208, -26881, -26554,
        -26227, -25900, -25573, -25246, -24919, -24592, -24265, -23938, -23611, -23284,
        -22957, -22630, -22303, -21976, -21649, -21322, -20995, -20668, -20341, -20014,
        0,
        20014, 20341, 20668, 20995, 21322, 21649, 21976, 22303, 22630, 22957,
        23284, 23611, 23938, 24265, 24592, 24919, 25246, 25573, 25900, 26227,
        26554, 26881, 27208, 27535, 27862, 28189, 28516, 28843, 29170, 29497,
        29824, 30151, 30478, 30805, 31132, 31459, 31786, 32113, 32440, 32767
    };
    
    // Clamp input to lookup table range
    if (x < -20480) return -32767;
    if (x > 20480) return 32767;
    
    // Map input to lookup table index
    int index = (x + 20480) / 512;
    if (index < 0) index = 0;
    if (index >= 81) index = 80;
    
    return tanhLUT[index];
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * Clamp integer value to range using existing rusefi patterns
 */
inline int32_t clampI(int32_t min_val, int32_t value, int32_t max_val) {
    return maxI(min_val, minI(value, max_val));
}

/**
 * Compute linear regression slope for temporal analysis
 */
float computeLinearRegressionSlope(const float* y, const float* x, size_t n);

/**
 * Compute variance of float array
 */
float computeVariance(const float* data, size_t n);

/**
 * Compute moving average trend
 */
float computeMovingAverageTrend(const float* data, size_t n, size_t windowSize);