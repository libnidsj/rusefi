/**
 * @file wall_fuel_adaptation_generated.cpp
 * 
 * @date Created May 2025
 */

#include "pch.h"

void setDefaultWallWettingAdaptation() {
    // Initialize wall wetting adaptation settings
    engineConfiguration->wallWettingAdaptationMode = WALL_WETTING_ADAPTATION_MODE_DISABLED;
    engineConfiguration->wallWettingAdaptationCellMode = WALL_WETTING_ADAPTATION_CELL_MODE_GLOBAL;
    engineConfiguration->wallWettingAdaptationSaveToFlash = false;
    engineConfiguration->wallWettingAdaptationResetRequired = false;
    
    // Adaptation timing parameters
    engineConfiguration->wallWettingAdaptationTimeConstant = 10.0f;      // 10 seconds integration time constant
    engineConfiguration->wallWettingAdaptationThreshold = 0.03f;         // 3% lambda error threshold
    engineConfiguration->wallWettingAdaptationMaxLambdaError = 0.15f;    // 15% maximum error
    engineConfiguration->wallWettingAdaptationRateGain = 1.0f;           // Default gain
    
    // Adaptation limits
    engineConfiguration->wallWettingAdaptationTauLimit = 25.0f;          // 25% limit on tau adaptation
    engineConfiguration->wallWettingAdaptationBetaLimit = 25.0f;         // 25% limit on beta adaptation
    
    // Adaptation operating conditions
    engineConfiguration->wallWettingAdaptationMinClt = 70.0f;            // 70°C minimum coolant temp
    engineConfiguration->wallWettingAdaptationMinRpm = 800.0f;           // 800 RPM minimum
    engineConfiguration->wallWettingAdaptationMinMap = 20.0f;            // 20 kPa minimum MAP
    engineConfiguration->wallWettingAdaptationMinTps = 0.0f;             // 0% minimum TPS
    engineConfiguration->wallWettingAdaptationMaxTps = 50.0f;            // 50% maximum TPS
    engineConfiguration->wallWettingAdaptationMaxLoad = 80.0f;           // 80% maximum load
    engineConfiguration->wallWettingAdaptationDelay = 30.0f;             // 30 seconds delay after startup
    engineConfiguration->wallWettingAdaptationMaxMapRate = 5.0f;         // 5 kPa/s allowed MAP change
    engineConfiguration->wallWettingAdaptationMaxTpsRate = 2.0f;         // 2 %/s allowed TPS change
    
    // Lambda feedback validation
    engineConfiguration->wallWettingAdaptationMinFeedback = 5.0f;        // 5 seconds minimum O2 feedback time
    engineConfiguration->wallWettingAdaptationMinAfr = 12.0f;            // Minimum AFR for adaptation
    engineConfiguration->wallWettingAdaptationMaxAfr = 16.0f;            // Maximum AFR for adaptation
    
    // Initialize RPM bins for adaptation
    for (int i = 0; i < WWAE_ADAPT_RPM_SIZE; i++) {
        float percent = (float)i / (WWAE_ADAPT_RPM_SIZE - 1);
        float rpm = 800.0f + percent * 5200.0f; // 800-6000 RPM range
        engineConfiguration->wwaeRpmBins[i] = (int8_t)(rpm / 50.0f);
    }
    
    // Initialize load bins for adaptation 
    for (int i = 0; i < WWAE_ADAPT_LOAD_SIZE; i++) {
        float percent = (float)i / (WWAE_ADAPT_LOAD_SIZE - 1);
        float load = 20.0f + percent * 80.0f;  // 20-100 kPa range
        engineConfiguration->wwaeLoadBins[i] = (int8_t)load;
    }
    
    // Initialize adaptation tables with default values (100 = 1.0 multiplier)
    for (int r = 0; r < WWAE_ADAPT_RPM_SIZE; r++) {
        for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
            engineConfiguration->wwaeTauTable[r][l] = 100;
            engineConfiguration->wwaeBetaTable[r][l] = 100;
        }
    }
    
    // Initialize load-only adaptation tables
    for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
        engineConfiguration->wwaeTauLoadCorr[l] = 100;
        engineConfiguration->wwaeBetaLoadCorr[l] = 100;
    }
    
    // Initialize smoothing parameters
    engineConfiguration->wallWettingAdaptationSmoothingFactor = 0.7f;   // 70% weight to new value, 30% to neighbors
    
    // Initialize numerical stability parameters 
    engineConfiguration->wallWettingAdaptationMinCovarianceValue = 0.01f; // Minimum covariance to prevent numerical issues
    engineConfiguration->wallWettingAdaptationMaxCovarianceValue = 10.0f; // Maximum covariance to limit adaptation speed
} 