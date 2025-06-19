/**
 * @file neural_network_config.cpp
 * 
 * Neural Network Configuration Implementation for rusefi
 */

#include "pch.h"
#include "neural_network_config.h"
#include "engine.h"

// Global neural network configuration instance
neural_network_config_s g_neural_config;

bool validateNeuralConfig() {
    // Validate learning parameters
    if (g_neural_config.neuralLearningRate <= 0.0f || g_neural_config.neuralLearningRate > 1.0f) {
        return false;
    }
    
    if (g_neural_config.neuralMinConfidence < 0.0f || g_neural_config.neuralMinConfidence > 1.0f) {
        return false;
    }
    
    // Validate correction bounds
    if (g_neural_config.neuralMaxCorrection <= g_neural_config.neuralMinCorrection) {
        return false;
    }
    
    // Validate temporal parameters
    if (g_neural_config.neuralTemporalBufferSize < 50 || g_neural_config.neuralTemporalBufferSize > 1000) {
        return false;
    }
    
    if (g_neural_config.neuralShortTermWindow <= 0.0f || g_neural_config.neuralShortTermWindow > 1.0f) {
        return false;
    }
    
    if (g_neural_config.neuralLongTermWindow <= g_neural_config.neuralShortTermWindow || g_neural_config.neuralLongTermWindow > 2.0f) {
        return false;
    }
    
    // Validate operational ranges
    if (g_neural_config.neuralMinRpm >= g_neural_config.neuralMaxRpm) {
        return false;
    }
    
    if (g_neural_config.neuralMinClt >= g_neural_config.neuralMaxClt) {
        return false;
    }
    
    if (g_neural_config.neuralMinLambda >= g_neural_config.neuralMaxLambda) {
        return false;
    }
    
    // Validate performance parameters
    if (g_neural_config.neuralMaxInferenceTimeUs == 0 || g_neural_config.neuralMaxInferenceTimeUs > 10000) {
        return false;
    }
    
    if (g_neural_config.neuralMaxMemoryUsage == 0 || g_neural_config.neuralMaxMemoryUsage > 100) {
        return false;
    }
    
    return true;
}

void resetNeuralConfigToDefaults() {
    // System enable flags
    g_neural_config.neuralLearningEnabled = false;
    g_neural_config.neuralWallWettingEnabled = true;
    g_neural_config.neuralLTFTEnabled = true;
    g_neural_config.neuralFailsafeEnabled = true;
    
    // Learning parameters
    g_neural_config.neuralLearningRate = 0.01f;
    g_neural_config.neuralMinConfidence = 0.1f;
    g_neural_config.neuralMaxCorrection = 50.0f;
    g_neural_config.neuralMinCorrection = -50.0f;
    
    // Temporal analysis parameters
    g_neural_config.neuralTemporalBufferSize = 200;
    g_neural_config.neuralShortTermWindow = 0.1f;
    g_neural_config.neuralLongTermWindow = 0.5f;
    g_neural_config.neuralVarianceThreshold = 0.02f;
    
    // Wall Wetting Neural Parameters
    g_neural_config.neuralWWBetaGain = 0.05f;
    g_neural_config.neuralWWTauGain = 0.05f;
    g_neural_config.neuralWWMinTransient = 0.1f;
    g_neural_config.neuralWWCooldownTime = 10.0f;
    
    // LTFT Neural Parameters
    g_neural_config.neuralLTFTBlendFactor = 0.3f;
    g_neural_config.neuralLTFTConfidenceGain = 0.2f;
    g_neural_config.neuralLTFTPredictionRate = 0.2f;
    
    // Quantization parameters
    g_neural_config.neuralQuantizationBits = 8;
    g_neural_config.neuralScaleFactor = 64.0f;
    
    // Performance parameters
    g_neural_config.neuralMaxInferenceTimeUs = 1000;
    g_neural_config.neuralMaxMemoryUsage = 80;
    
    // Failsafe parameters
    g_neural_config.neuralMaxConsecutiveFailures = 5;
    g_neural_config.neuralValidationThreshold = 0.05f;
    g_neural_config.neuralHealthCheckInterval = 5.0f;
    
    // Operational conditions
    g_neural_config.neuralMinRpm = 1000.0f;
    g_neural_config.neuralMaxRpm = 6000.0f;
    g_neural_config.neuralMinClt = 60.0f;
    g_neural_config.neuralMaxClt = 110.0f;
    g_neural_config.neuralMinLambda = 0.7f;
    g_neural_config.neuralMaxLambda = 1.3f;
    
    // CRC for configuration validation - will be calculated later
    g_neural_config.neuralConfigCRC = 0;
}

void initializeNeuralConfig() {
    // Reset to defaults first
    resetNeuralConfigToDefaults();
    
    // Check if we have valid configuration from engine config
    if (engineConfiguration) {
        // Copy values from engine configuration if available
        g_neural_config.neuralLearningEnabled = engineConfiguration->neuralLearningEnabled;
        
        // For now, use defaults unless specific neural config is added to engineConfiguration
        // In the future, these could be exposed in TunerStudio
    }
    
    // Validate the configuration
    if (!validateNeuralConfig()) {
        efiPrintf("Neural Config: Invalid configuration, using defaults");
        resetNeuralConfigToDefaults();
    }
    
    // Calculate CRC for validation
    g_neural_config.neuralConfigCRC = crc32(&g_neural_config, sizeof(neural_network_config_s) - sizeof(uint32_t));
    
    efiPrintf("Neural Config: Initialized successfully");
}