/**
 * @file neural_network_config.h
 * 
 * Neural Network Configuration for rusefi
 * Configuration parameters and tuning constants
 */

#pragma once

// Neural Network System Configuration
struct neural_network_config_s {
    // System enable flags
    bool neuralLearningEnabled = false;          // Master enable for neural learning
    bool neuralWallWettingEnabled = true;        // Enable neural wall wetting
    bool neuralLTFTEnabled = true;               // Enable neural LTFT
    bool neuralFailsafeEnabled = true;           // Enable failsafe mechanisms
    
    // Learning parameters
    float neuralLearningRate = 0.01f;            // Base learning rate
    float neuralMinConfidence = 0.1f;            // Minimum confidence threshold
    float neuralMaxCorrection = 50.0f;           // Maximum correction percentage
    float neuralMinCorrection = -50.0f;          // Minimum correction percentage
    
    // Temporal analysis parameters
    uint16_t neuralTemporalBufferSize = 200;     // Temporal buffer size (samples)
    float neuralShortTermWindow = 0.1f;          // Short-term analysis window (seconds)
    float neuralLongTermWindow = 0.5f;           // Long-term analysis window (seconds)
    float neuralVarianceThreshold = 0.02f;      // Lambda variance threshold
    
    // Wall Wetting Neural Parameters
    float neuralWWBetaGain = 0.05f;              // Beta learning gain
    float neuralWWTauGain = 0.05f;               // Tau learning gain
    float neuralWWMinTransient = 0.1f;           // Minimum transient magnitude
    float neuralWWCooldownTime = 10.0f;          // Cooldown between learning cycles (seconds)
    
    // LTFT Neural Parameters
    float neuralLTFTBlendFactor = 0.3f;          // Neural/classical blend ratio
    float neuralLTFTConfidenceGain = 0.2f;       // Confidence-based learning gain
    float neuralLTFTPredictionRate = 0.2f;       // Prediction update rate (seconds)
    
    // Quantization parameters
    uint8_t neuralQuantizationBits = 8;          // Quantization bit depth
    float neuralScaleFactor = 64.0f;             // Quantization scale factor
    
    // Performance parameters
    uint16_t neuralMaxInferenceTimeUs = 1000;    // Maximum inference time (microseconds)
    uint8_t neuralMaxMemoryUsage = 80;           // Maximum memory usage percentage
    
    // Failsafe parameters
    uint8_t neuralMaxConsecutiveFailures = 5;    // Max failures before failsafe
    float neuralValidationThreshold = 0.05f;    // Validation error threshold
    float neuralHealthCheckInterval = 5.0f;     // Health check interval (seconds)
    
    // Operational conditions
    float neuralMinRpm = 1000.0f;                // Minimum RPM for learning
    float neuralMaxRpm = 6000.0f;                // Maximum RPM for learning
    float neuralMinClt = 60.0f;                  // Minimum CLT for learning
    float neuralMaxClt = 110.0f;                 // Maximum CLT for learning
    float neuralMinLambda = 0.7f;                // Minimum lambda for learning
    float neuralMaxLambda = 1.3f;                // Maximum lambda for learning
    
    // CRC for configuration validation
    uint32_t neuralConfigCRC = 0;
};

// Global neural network configuration
extern neural_network_config_s g_neural_config;

// Configuration validation
bool validateNeuralConfig();
void initializeNeuralConfig();
void resetNeuralConfigToDefaults();