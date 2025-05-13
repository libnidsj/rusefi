/*
 * @file wall_fuel_adaptation.cpp
 *
 * Implementation of online adaptation for wall wetting model parameters.
 * 
 * @author Implementation based on SAE 2001-MECA01 paper
 * Copyright (c) 2025
 */

#include "pch.h"
#include "wall_fuel_adaptation.h"
#include "wall_fuel.h"
#include "sensor.h"
#include "fuel_computer.h"
#include "engine_state.h"
#include "interpolation.h"
#include "flash_main.h"

#if EFI_TUNER_STUDIO
#include "tunerstudio.h"
#endif

// Period in seconds for slow callback
constexpr float ADAPTATION_DT = SLOW_CALLBACK_PERIOD_MS * 0.001f;

// Minimum time in seconds before we consider the engine to be in "steady state"
constexpr float MIN_STEADY_STATE_TIME = 0.5f;

// Default adaptation settings if not configured
constexpr float DEFAULT_ADAPTATION_TIME_CONSTANT = 10.0f;
constexpr float DEFAULT_ADAPTATION_THRESHOLD = 0.03f;
constexpr float DEFAULT_ADAPTATION_RATE = 1.0f;

// These are used to protect against numerical issues
constexpr float MIN_TAU_CORRECTION = 0.5f;    // Tau can't be less than 50% of base
constexpr float MAX_TAU_CORRECTION = 2.0f;    // Tau can't be more than 200% of base
constexpr float MIN_BETA_CORRECTION = 0.5f;   // Beta can't be less than 50% of base
constexpr float MAX_BETA_CORRECTION = 2.0f;   // Beta can't be more than 200% of base

// RLS algorithm settings
constexpr float DEFAULT_FORGETTING_FACTOR = 0.99f;
constexpr float MIN_COVARIANCE = 0.01f;
constexpr float MAX_COVARIANCE = 10.0f;

WallFuelAdaptation::WallFuelAdaptation() {
    reset();
    
    // Initialize output channels
    engine->outputChannels.wallWettingAdaptationActive = false;
    engine->outputChannels.wallWettingAdaptationSteadyStateTime = 0;
    engine->outputChannels.wallWettingAdaptationTauCorrection = 1.0f;
    engine->outputChannels.wallWettingAdaptationBetaCorrection = 1.0f;
    engine->outputChannels.wallWettingAdaptationLambdaError = 0.0f;
    engine->outputChannels.wallWettingAdaptationLastSaveTime = 0;
}

void WallFuelAdaptation::reset() {
    m_steadyStateTime = 0;
    m_engineRunTime = 0;
    isAdaptationActive = false;
    
    // Reset adaptation integrals and parameters
    tauIntegral = 0;
    betaIntegral = 0;
    lastLambdaError = 0;
    lastTauCorrection = 1.0f;
    lastBetaCorrection = 1.0f;
    
    // Reset RLS state
    tauCovariance = 1.0f;
    betaCovariance = 1.0f;
    
    // Reset sensor validation
    m_lambdaFeedbackTime = 0;
    
    // Check if we need to reset the adaptation tables
    if (engineConfiguration->wallWettingAdaptationResetRequired) {
        resetAdaptationTables();
        // Clear the reset flag
        engineConfiguration->wallWettingAdaptationResetRequired = false;
        // Save to flash if enabled
        if (engineConfiguration->wallWettingAdaptationSaveToFlash) {
            setNeedToWriteConfiguration();
        }
    }
}

void WallFuelAdaptation::resetAdaptationTables() {
    // Reset all table values to 100 (meaning 1.0 or no correction)
    for (int r = 0; r < WWAE_ADAPT_RPM_SIZE; r++) {
        for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
            config->wwaeTauTable[r][l] = 100;
            config->wwaeBetaTable[r][l] = 100;
        }
    }
    
    // Also reset load-only tables
    for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
        config->wwaeTauLoadCorr[l] = 100;
        config->wwaeBetaLoadCorr[l] = 100;
    }
}

void WallFuelAdaptation::onFastCallback() {
    ScopePerf perf(PE::WallFuelAdaptation);
    
    // Update engine run time counter
    if (engine->rpmCalculator.isRunning()) {
        m_engineRunTime += FAST_CALLBACK_PERIOD_MS * 0.001f;
    } else {
        m_engineRunTime = 0; // Reset when engine stops
    }

    // If adaptation is disabled, reset and return
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_DISABLED) {
        if (isAdaptationActive) {
            reset();
        }
        return;
    }

    // Check if engine is running
    if (!engine->rpmCalculator.isRunning()) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }

    // Check if we're in closed loop fuel conditions (require closed loop for adaptation)
    if (!engine->stft.isAfrValueValid) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Get current sensor values
    auto map = Sensor::get(SensorType::Map).value_or(60);
    auto rpm = Sensor::get(SensorType::Rpm).value_or(0);
    auto clt = Sensor::get(SensorType::Clt).value_or(80);
    auto tps = Sensor::get(SensorType::Tps1).value_or(0);
    auto afr = Sensor::get(SensorType::Afr1).value_or(14.7f);
    
    // Check minimum engine warmup
    if (clt < engineConfiguration->wallWettingAdaptationMinClt) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check minimum RPM
    if (rpm < engineConfiguration->wallWettingAdaptationMinRpm) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check MAP range
    if (map < engineConfiguration->wallWettingAdaptationMinMap) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check TPS range
    if (tps < engineConfiguration->wallWettingAdaptationMinTps || 
        tps > engineConfiguration->wallWettingAdaptationMaxTps) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check engine load
    auto load = engine->engineState.fuelingLoad;
    if (load > engineConfiguration->wallWettingAdaptationMaxLoad) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check minimum engine run time
    if (m_engineRunTime < engineConfiguration->wallWettingAdaptationDelay) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Check AFR range to ensure we're in valid adaptation region
    if (afr < engineConfiguration->wallWettingAdaptationMinAfr || 
        afr > engineConfiguration->wallWettingAdaptationMaxAfr) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }
    
    // Track O2 feedback time to ensure we have stable lambda readings
    m_lambdaFeedbackTime += FAST_CALLBACK_PERIOD_MS * 0.001f;
    if (m_lambdaFeedbackTime < engineConfiguration->wallWettingAdaptationMinFeedback) {
        m_steadyStateTime = 0;
        isAdaptationActive = false;
        return;
    }

    // Need to know rate of change of manifold pressure to determine if we're in steady state
    static float lastMap = map;
    float mapRate = (map - lastMap) / FAST_CALLBACK_PERIOD_MS * 1000.0f; // kPa/sec
    lastMap = map;

    // Also check throttle rate
    static float lastTps = tps;
    float tpsRate = (tps - lastTps) / FAST_CALLBACK_PERIOD_MS * 1000.0f; // %/sec
    lastTps = tps;

    // Check if we're in "steady state" - both MAP rate and TPS rate should be small
    // Only adapt when conditions are stable
    bool isTransient = (fabsf(mapRate) > engineConfiguration->wallWettingAdaptationMaxMapRate) || 
                       (fabsf(tpsRate) > engineConfiguration->wallWettingAdaptationMaxTpsRate);

    if (isTransient) {
        // Reset steady state timer if we have a transient
        m_steadyStateTime = 0;
        isAdaptationActive = false;
    } else {
        // Increment steady state timer - we need a minimum amount of steady state time before adapting
        m_steadyStateTime += FAST_CALLBACK_PERIOD_MS * 0.001f;
        isAdaptationActive = m_steadyStateTime > MIN_STEADY_STATE_TIME;
    }
    
    // Update output channels for TunerStudio
    engine->outputChannels.wallWettingAdaptationActive = isAdaptationActive;
    engine->outputChannels.wallWettingAdaptationSteadyStateTime = m_steadyStateTime;
    engine->outputChannels.wallWettingAdaptationTauCorrection = lastTauCorrection;
    engine->outputChannels.wallWettingAdaptationBetaCorrection = lastBetaCorrection;
}

void WallFuelAdaptation::onSlowCallback() {
    // Only adapt if active and engine is running
    if (!isAdaptationActive || !engine->rpmCalculator.isRunning()) {
        return;
    }

    // Calculate lambda error and adapt parameters
    float lambdaError = calculateLambdaError();
    
    // Update TunerStudio output channel
    engine->outputChannels.wallWettingAdaptationLambdaError = lambdaError;
    
    // Only adapt if lambda error is larger than threshold and isn't extreme
    // (extreme error likely caused by other issues, not wall wetting)
    if (fabsf(lambdaError) > engineConfiguration->wallWettingAdaptationThreshold && 
        fabsf(lambdaError) < engineConfiguration->wallWettingAdaptationMaxLambdaError) {
        adaptParameters(lambdaError);
    }
    
    lastLambdaError = lambdaError;
}

void WallFuelAdaptation::onIgnitionStateChanged(bool ignitionState) {
    // When ignition turns off, save adaptation to flash if enabled
    if (!ignitionState) {
        efiPrintf(WALL_WETTING_ADAPTATION_LOGGING, "Wall Wetting: Engine stopping, adaptation state saved");
        
        // Only attempt to save if adaptations are enabled and we've been active
        if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_DISABLED &&
            isAdaptationActive) {
            
            // If user wants to save adaptations permanently to flash
            if (engineConfiguration->wallWettingAdaptationSaveToFlash) {
                efiPrintf(WALL_WETTING_ADAPTATION_LOGGING, "Wall Wetting: Saving adaptations to flash");
                
                // Trigger configuration write
                setNeedToWriteConfiguration();
                
                // Update output channel to indicate saved state
                engine->outputChannels.wallWettingAdaptationLastSaveTime = 
                    (int)(getTimeNowSeconds());
            }
        }
        
        // Always reset state when engine stops, even if not saving to flash
        reset();
    } else {
        // Engine is starting - check if we need to restore previous adaptations
        // (this is handled automatically since they're stored in configuration)
        efiPrintf(WALL_WETTING_ADAPTATION_LOGGING, "Wall Wetting: Engine starting, adaptation initialized");
    }
}

float WallFuelAdaptation::getTauCorrection(float rpm, float load, float clt) const {
    // Return 1.0 (no correction) if adaptation is disabled
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_DISABLED ||
        engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
        return 1.0f;
    }

    if (!engine->rpmCalculator.isRunning()) {
        return 1.0f;
    }

    auto mode = engineConfiguration->wallWettingAdaptationCellMode;

    // Handle different adaptation modes
    switch (mode) {
        case WALL_WETTING_ADAPTATION_CELL_MODE_GLOBAL: {
            // Single global correction factor
            return lastTauCorrection;
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_LOAD_INTERPOLATED: {
            // Use only load axis - useful for speed-density systems where load is MAP
            // Get load bin index only
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            if (loadIdx < 0 || loadIdx >= WWAE_ADAPT_LOAD_SIZE - 1) {
                return 1.0f;
            }
            
            // Interpolate between load points
            float loadBinLow = config->wwaeLoadBins[loadIdx];
            float loadBinHigh = config->wwaeLoadBins[loadIdx + 1];
            float tauCorrLow = config->wwaeTauLoadCorr[loadIdx] * 0.01f;
            float tauCorrHigh = config->wwaeTauLoadCorr[loadIdx + 1] * 0.01f;
            
            float loadRatio = (load - loadBinLow) / (loadBinHigh - loadBinLow);
            return interpolateCurve(loadRatio, tauCorrLow, tauCorrHigh);
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_FULL_3D: {
            // Use 3D interpolation between RPM and Load cells
            return interpolateCorrection(rpm, load, config->wwaeTauTable);
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_CELL_BASED: {
            // Find nearest cell and use its value
            int rpmIdx = findBinIndex(rpm, config->wwaeRpmBins, WWAE_ADAPT_RPM_SIZE);
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            
            if (rpmIdx < 0 || rpmIdx >= WWAE_ADAPT_RPM_SIZE || 
                loadIdx < 0 || loadIdx >= WWAE_ADAPT_LOAD_SIZE) {
                return 1.0f;
            }
            
            return config->wwaeTauTable[rpmIdx][loadIdx] * 0.01f;
        }
        default:
            return 1.0f;
    }
}

float WallFuelAdaptation::getBetaCorrection(float rpm, float load, float clt) const {
    // Return 1.0 (no correction) if adaptation is disabled
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_DISABLED ||
        engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
        return 1.0f;
    }

    if (!engine->rpmCalculator.isRunning()) {
        return 1.0f;
    }

    auto mode = engineConfiguration->wallWettingAdaptationCellMode;

    // Handle different adaptation modes
    switch (mode) {
        case WALL_WETTING_ADAPTATION_CELL_MODE_GLOBAL: {
            // Single global correction factor
            return lastBetaCorrection;
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_LOAD_INTERPOLATED: {
            // Use only load axis - useful for speed-density systems where load is MAP
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            if (loadIdx < 0 || loadIdx >= WWAE_ADAPT_LOAD_SIZE - 1) {
                return 1.0f;
            }
            
            // Interpolate between load points
            float loadBinLow = config->wwaeLoadBins[loadIdx];
            float loadBinHigh = config->wwaeLoadBins[loadIdx + 1];
            float betaCorrLow = config->wwaeBetaLoadCorr[loadIdx] * 0.01f;
            float betaCorrHigh = config->wwaeBetaLoadCorr[loadIdx + 1] * 0.01f;
            
            float loadRatio = (load - loadBinLow) / (loadBinHigh - loadBinLow);
            return interpolateCurve(loadRatio, betaCorrLow, betaCorrHigh);
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_FULL_3D: {
            // Use 3D interpolation between RPM and Load cells
            return interpolateCorrection(rpm, load, config->wwaeBetaTable);
        }
        case WALL_WETTING_ADAPTATION_CELL_MODE_CELL_BASED: {
            // Find nearest cell and use its value
            int rpmIdx = findBinIndex(rpm, config->wwaeRpmBins, WWAE_ADAPT_RPM_SIZE);
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            
            if (rpmIdx < 0 || rpmIdx >= WWAE_ADAPT_RPM_SIZE || 
                loadIdx < 0 || loadIdx >= WWAE_ADAPT_LOAD_SIZE) {
                return 1.0f;
            }
            
            return config->wwaeBetaTable[rpmIdx][loadIdx] * 0.01f;
        }
        default:
            return 1.0f;
    }
}

float WallFuelAdaptation::interpolateCorrection(float rpm, float load, const int8_t table[WWAE_ADAPT_RPM_SIZE][WWAE_ADAPT_LOAD_SIZE]) const {
    // 3D interpolation between RPM and Load cells
    int rpmIdx = findBinIndex(rpm, config->wwaeRpmBins, WWAE_ADAPT_RPM_SIZE);
    int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
    
    if (rpmIdx < 0 || rpmIdx >= WWAE_ADAPT_RPM_SIZE - 1 || 
        loadIdx < 0 || loadIdx >= WWAE_ADAPT_LOAD_SIZE - 1) {
        return 1.0f;
    }
    
    // Use interpolation for smoother transitions
    float rpmLow = config->wwaeRpmBins[rpmIdx];
    float rpmHigh = config->wwaeRpmBins[rpmIdx + 1];
    float loadLow = config->wwaeLoadBins[loadIdx];
    float loadHigh = config->wwaeLoadBins[loadIdx + 1];
    
    float rpmRatio = (rpm - rpmLow) / (rpmHigh - rpmLow);
    float loadRatio = (load - loadLow) / (loadHigh - loadLow);
    
    // Get all four corners
    float valueLowLow = table[rpmIdx][loadIdx] * 0.01f;
    float valueHighLow = table[rpmIdx + 1][loadIdx] * 0.01f;
    float valueLowHigh = table[rpmIdx][loadIdx + 1] * 0.01f;
    float valueHighHigh = table[rpmIdx + 1][loadIdx + 1] * 0.01f;
    
    // Bilinear interpolation
    float valueLow = interpolateCurve(rpmRatio, valueLowLow, valueHighLow);
    float valueHigh = interpolateCurve(rpmRatio, valueLowHigh, valueHighHigh);
    
    return interpolateCurve(loadRatio, valueLow, valueHigh);
}

float WallFuelAdaptation::calculateLambdaError() const {
    // Get current AFR values
    auto afr = Sensor::get(SensorType::Afr1).value_or(14.7f);
    auto targetAfr = engine->fuelComputer.targetAFR;
    
    // Convert AFR to lambda (assuming 14.7 stoichiometric ratio for gasoline)
    float stoichAFR = engine->fuelComputer.stoichiometricRatio;
    float lambda = afr / stoichAFR;
    float targetLambda = targetAfr / stoichAFR;
    
    // Calculate lambda error
    // Positive value means lean (actual > target), negative means rich (actual < target)
    float lambdaError = (lambda / targetLambda) - 1.0f;
    
    // Limit the error to reasonable values
    return clampf(lambdaError, -engineConfiguration->wallWettingAdaptationMaxLambdaError, 
                             engineConfiguration->wallWettingAdaptationMaxLambdaError);
}

float WallFuelAdaptation::applyAntiWindup(float integral, float limit) const {
    // Simple anti-windup: clamp the integral value to prevent excessive buildup
    if (integral > limit) {
        return limit;
    } else if (integral < -limit) {
        return -limit;
    }
    return integral;
}

void WallFuelAdaptation::adaptParameters(float lambdaError) {
    // Get adaptation configuration
    float timeConstant = engineConfiguration->wallWettingAdaptationTimeConstant;
    if (timeConstant < 0.1f) {
        timeConstant = DEFAULT_ADAPTATION_TIME_CONSTANT;
    }
    
    float adaptationRate = engineConfiguration->wallWettingAdaptationRateGain;
    if (adaptationRate < 0.01f) {
        adaptationRate = DEFAULT_ADAPTATION_RATE;
    }
    
    // Get engine parameters we need for adaptation
    auto rpm = Sensor::get(SensorType::Rpm).value_or(0);
    auto map = Sensor::get(SensorType::Map).value_or(60);
    auto clt = Sensor::get(SensorType::Clt).value_or(80);
    
    // Implementation of the Recursive Least Squares method
    
    // Calculate forgetting factor based on time constant
    // λ = exp(-ΔT/TC) where TC is time constant and ΔT is sampling period
    float forgettingFactor = expf(-ADAPTATION_DT / timeConstant);
    if (forgettingFactor < 0.9f || forgettingFactor > 0.999f) {
        forgettingFactor = DEFAULT_FORGETTING_FACTOR;
    }
    
    // Get current wall wetting model parameters
    float baseTau = engine->module<WallFuelController>()->getTau(rpm, map, clt);
    float baseBeta = engine->module<WallFuelController>()->getBeta();
    
    // Get derivatives of fuel flow with respect to tau and beta
    // These represent how sensitive the fuel flow is to each parameter
    // From the SAE paper 2001-MECA01, these are approximated based on engine operating conditions
    
    // Calculate partial derivative for tau (∂mf/∂τ)
    // The higher the RPM and the higher the rate of change of fuel flow,
    // the more sensitive the system is to tau changes
    float tauSensitivity = 0.05f * rpm / 1000.0f; // Simplified from the paper
    
    // Calculate partial derivative for beta (∂mf/∂β)
    // This is affected by the base fuel flow and inversely by temperature
    float betaSensitivity = 0.1f * map / 100.0f; // Simplified from the paper
    
    // Apply adaptation gain to sensitivities
    tauSensitivity *= adaptationRate;
    betaSensitivity *= adaptationRate;
    
    // RLS algorithm: Update the state estimation and covariance matrix
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_TAU_ONLY ||
        engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_BOTH) {
        
        // Update tau estimation using RLS with variable forgetting factor
        // Kalman gain = P * H / (H^T * P * H + R)
        // Where P is covariance, H is sensitivity, and R is measurement noise
        float tauKalmanGain = tauCovariance * tauSensitivity / 
                              (tauSensitivity * tauSensitivity * tauCovariance + 1.0f);
        
        // Update parameter estimate: θ_new = θ_old + K * (y - h*θ_old)
        tauIntegral += tauKalmanGain * lambdaError;
        
        // Update covariance matrix: P_new = (I - K*H^T)*P_old/λ
        tauCovariance = (tauCovariance - tauKalmanGain * tauSensitivity * tauCovariance) / forgettingFactor;
        
        // Apply anti-windup to limit the integral based on user-defined limits
        float tauLimit = engineConfiguration->wallWettingAdaptationTauLimit * 0.01f;
        tauIntegral = applyAntiWindup(tauIntegral, tauLimit);
        
        // Ensure covariance stays in reasonable bounds to maintain numerical stability
        tauCovariance = clampf(tauCovariance, MIN_COVARIANCE, MAX_COVARIANCE);
    }
    
    // Update Beta estimation if we should adapt beta
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_BETA_ONLY ||
        engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_BOTH) {
        
        // For beta, the effect can be opposite of tau depending on engine conditions
        float betaKalmanGain = betaCovariance * betaSensitivity / 
                              (betaSensitivity * betaSensitivity * betaCovariance + 1.0f);
        
        // Note the sign for beta can be opposite from tau in some conditions
        // This is because increasing beta means more fuel on walls (richer mixture)
        // but increasing tau means slower evaporation (leaner mixture)
        if (rpm < 1500) {
            // At low RPM, beta dominant effect is richer (opposite sign as tau)
            betaIntegral -= betaKalmanGain * lambdaError;
        } else {
            // At higher RPM, effect becomes similar direction as tau
            betaIntegral += betaKalmanGain * lambdaError * 0.5f;
        }
        
        // Update covariance matrix
        betaCovariance = (betaCovariance - betaKalmanGain * betaSensitivity * betaCovariance) / forgettingFactor;
        
        // Apply anti-windup to limit the integral
        float betaLimit = engineConfiguration->wallWettingAdaptationBetaLimit * 0.01f;
        betaIntegral = applyAntiWindup(betaIntegral, betaLimit);
        
        // Ensure covariance stays in reasonable bounds
        betaCovariance = clampf(betaCovariance, MIN_COVARIANCE, MAX_COVARIANCE);
    }
    
    // Convert integrals to correction factors (e^integral)
    // This gives us multiplicative corrections that start at 1.0
    float tauCorrection = expf(tauIntegral);
    float betaCorrection = expf(betaIntegral);
    
    // Apply limits to the correction factors to avoid numerical issues
    tauCorrection = clampf(tauCorrection, MIN_TAU_CORRECTION, MAX_TAU_CORRECTION);
    betaCorrection = clampf(betaCorrection, MIN_BETA_CORRECTION, MAX_BETA_CORRECTION);
    
    // Store the latest correction factors for monitoring
    lastTauCorrection = tauCorrection;
    lastBetaCorrection = betaCorrection;
    
    // Update the TunerStudio output channels for monitoring
    engine->outputChannels.wallWettingAdaptationTauCorrection = tauCorrection;
    engine->outputChannels.wallWettingAdaptationBetaCorrection = betaCorrection;
    
    // Update the correction tables in configuration
    updateCorrectionTables(tauCorrection, betaCorrection);
}

void WallFuelAdaptation::updateCorrectionTables(float tauCorrection, float betaCorrection) {
    // Don't update tables if adaptation is disabled
    if (engineConfiguration->wallWettingAdaptationMode == WALL_WETTING_ADAPTATION_MODE_DISABLED) {
        return;
    }
    
    // Get current RPM and load
    auto rpm = Sensor::get(SensorType::Rpm).value_or(0);
    auto load = Sensor::get(SensorType::Map).value_or(60);
    
    auto mode = engineConfiguration->wallWettingAdaptationCellMode;
    
    // Handle different adaptation modes
    switch (mode) {
        case WALL_WETTING_ADAPTATION_CELL_MODE_GLOBAL: {
            // Single global correction factor - update all cells with the same value
            int8_t tauCorrScaled = static_cast<int8_t>(tauCorrection * 100);
            int8_t betaCorrScaled = static_cast<int8_t>(betaCorrection * 100);
            
            // Update all cells
            for (int r = 0; r < WWAE_ADAPT_RPM_SIZE; r++) {
                for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
                    if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
                        config->wwaeTauTable[r][l] = tauCorrScaled;
                    }
                    if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
                        config->wwaeBetaTable[r][l] = betaCorrScaled;
                    }
                }
            }
            
            // Also update load-only tables
            for (int l = 0; l < WWAE_ADAPT_LOAD_SIZE; l++) {
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
                    config->wwaeTauLoadCorr[l] = tauCorrScaled;
                    
                    // Apply smoothing to adjacent cells to prevent abrupt changes
                    // Simple linear smoothing to adjacent cells
                    if (l > 0) {
                        float currTauCorr = config->wwaeTauLoadCorr[l - 1];
                        float targetTauCorr = tauCorrScaled * 0.7f + currTauCorr * 0.3f;
                        config->wwaeTauLoadCorr[l - 1] = static_cast<int8_t>(targetTauCorr);
                    }
                    if (l < WWAE_ADAPT_LOAD_SIZE - 1) {
                        float currTauCorr = config->wwaeTauLoadCorr[l + 1];
                        float targetTauCorr = tauCorrScaled * 0.7f + currTauCorr * 0.3f;
                        config->wwaeTauLoadCorr[l + 1] = static_cast<int8_t>(targetTauCorr);
                    }
                }
                
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
                    config->wwaeBetaLoadCorr[l] = betaCorrScaled;
                    
                    // Apply smoothing to adjacent cells to prevent abrupt changes
                    if (l > 0) {
                        float currBetaCorr = config->wwaeBetaLoadCorr[l - 1];
                        float targetBetaCorr = betaCorrScaled * 0.7f + currBetaCorr * 0.3f;
                        config->wwaeBetaLoadCorr[l - 1] = static_cast<int8_t>(targetBetaCorr);
                    }
                    if (l < WWAE_ADAPT_LOAD_SIZE - 1) {
                        float currBetaCorr = config->wwaeBetaLoadCorr[l + 1];
                        float targetBetaCorr = betaCorrScaled * 0.7f + currBetaCorr * 0.3f;
                        config->wwaeBetaLoadCorr[l + 1] = static_cast<int8_t>(targetBetaCorr);
                    }
                }
            }
            break;
        }
        
        case WALL_WETTING_ADAPTATION_CELL_MODE_LOAD_INTERPOLATED: {
            // Find the nearest load bin
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            if (loadIdx >= 0 && loadIdx < WWAE_ADAPT_LOAD_SIZE) {
                int8_t tauCorrScaled = static_cast<int8_t>(tauCorrection * 100);
                int8_t betaCorrScaled = static_cast<int8_t>(betaCorrection * 100);
                
                // Update the one-dimensional load table
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
                    config->wwaeTauLoadCorr[loadIdx] = tauCorrScaled;
                    
                    // Apply smoothing to adjacent cells to prevent abrupt changes
                    if (loadIdx > 0) {
                        float currTauCorr = config->wwaeTauLoadCorr[loadIdx - 1];
                        float targetTauCorr = tauCorrScaled * 0.7f + currTauCorr * 0.3f;
                        config->wwaeTauLoadCorr[loadIdx - 1] = static_cast<int8_t>(targetTauCorr);
                    }
                    if (loadIdx < WWAE_ADAPT_LOAD_SIZE - 1) {
                        float currTauCorr = config->wwaeTauLoadCorr[loadIdx + 1];
                        float targetTauCorr = tauCorrScaled * 0.7f + currTauCorr * 0.3f;
                        config->wwaeTauLoadCorr[loadIdx + 1] = static_cast<int8_t>(targetTauCorr);
                    }
                }
                
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
                    config->wwaeBetaLoadCorr[loadIdx] = betaCorrScaled;
                    
                    // Apply smoothing to adjacent cells to prevent abrupt changes
                    if (loadIdx > 0) {
                        float currBetaCorr = config->wwaeBetaLoadCorr[loadIdx - 1];
                        float targetBetaCorr = betaCorrScaled * 0.7f + currBetaCorr * 0.3f;
                        config->wwaeBetaLoadCorr[loadIdx - 1] = static_cast<int8_t>(targetBetaCorr);
                    }
                    if (loadIdx < WWAE_ADAPT_LOAD_SIZE - 1) {
                        float currBetaCorr = config->wwaeBetaLoadCorr[loadIdx + 1];
                        float targetBetaCorr = betaCorrScaled * 0.7f + currBetaCorr * 0.3f;
                        config->wwaeBetaLoadCorr[loadIdx + 1] = static_cast<int8_t>(targetBetaCorr);
                    }
                }
            }
            break;
        }
        
        case WALL_WETTING_ADAPTATION_CELL_MODE_FULL_3D: {
            // Find RPM and load bin indices
            int rpmIdx = findBinIndex(rpm, config->wwaeRpmBins, WWAE_ADAPT_RPM_SIZE);
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            
            if (rpmIdx >= 0 && rpmIdx < WWAE_ADAPT_RPM_SIZE && 
                loadIdx >= 0 && loadIdx < WWAE_ADAPT_LOAD_SIZE) {
                
                int8_t tauCorrScaled = static_cast<int8_t>(tauCorrection * 100);
                int8_t betaCorrScaled = static_cast<int8_t>(betaCorrection * 100);
                
                // Update the current cell
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
                    config->wwaeTauTable[rpmIdx][loadIdx] = tauCorrScaled;
                    
                    // Apply smoothing to adjacent cells
                    applySmoothingTo3DTable(config->wwaeTauTable, rpmIdx, loadIdx, tauCorrScaled);
                }
                
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
                    config->wwaeBetaTable[rpmIdx][loadIdx] = betaCorrScaled;
                    
                    // Apply smoothing to adjacent cells
                    applySmoothingTo3DTable(config->wwaeBetaTable, rpmIdx, loadIdx, betaCorrScaled);
                }
            }
            break;
        }
        
        case WALL_WETTING_ADAPTATION_CELL_MODE_CELL_BASED: {
            // Find nearest cell without interpolation
            int rpmIdx = findBinIndex(rpm, config->wwaeRpmBins, WWAE_ADAPT_RPM_SIZE);
            int loadIdx = findBinIndex(load, config->wwaeLoadBins, WWAE_ADAPT_LOAD_SIZE);
            
            if (rpmIdx >= 0 && rpmIdx < WWAE_ADAPT_RPM_SIZE && 
                loadIdx >= 0 && loadIdx < WWAE_ADAPT_LOAD_SIZE) {
                
                int8_t tauCorrScaled = static_cast<int8_t>(tauCorrection * 100);
                int8_t betaCorrScaled = static_cast<int8_t>(betaCorrection * 100);
                
                // Only update this single cell without interpolation
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_BETA_ONLY) {
                    config->wwaeTauTable[rpmIdx][loadIdx] = tauCorrScaled;
                }
                
                if (engineConfiguration->wallWettingAdaptationMode != WALL_WETTING_ADAPTATION_MODE_TAU_ONLY) {
                    config->wwaeBetaTable[rpmIdx][loadIdx] = betaCorrScaled;
                }
            }
            break;
        }
    }
    
    // If save to flash is enabled, schedule write to flash
    // Only do this occasionally to avoid excessive flash writes
    if (engineConfiguration->wallWettingAdaptationSaveToFlash && 
        (getTimeNowUs() % 10000000) < 100000) { // Roughly every 10 seconds
        setNeedToWriteConfiguration();
    }
}

void WallFuelAdaptation::applySmoothingTo3DTable(int8_t table[WWAE_ADAPT_RPM_SIZE][WWAE_ADAPT_LOAD_SIZE], int centerRpmIdx, int centerLoadIdx, int8_t newValue) {
    // Apply smoothing to adjacent cells to prevent abrupt changes between cells
    // This helps avoid "potholes" in the correction surface
    
    // Smoother transition coefficient (0.3 = 30% of new value blended into neighbors)
    const float smoothingCoeff = 0.3f;
    
    // Apply to surrounding cells in a 3x3 grid
    for (int rOffset = -1; rOffset <= 1; rOffset++) {
        int rIdx = centerRpmIdx + rOffset;
        if (rIdx < 0 || rIdx >= WWAE_ADAPT_RPM_SIZE) {
            continue; // Skip if outside bounds
        }
        
        for (int lOffset = -1; lOffset <= 1; lOffset++) {
            // Skip the center cell (that's already been updated)
            if (rOffset == 0 && lOffset == 0) {
                continue;
            }
            
            int lIdx = centerLoadIdx + lOffset;
            if (lIdx < 0 || lIdx >= WWAE_ADAPT_LOAD_SIZE) {
                continue; // Skip if outside bounds
            }
            
            // Calculate distance from center (diagonal cells get less effect)
            float distance = sqrtf(rOffset * rOffset + lOffset * lOffset);
            float weight = smoothingCoeff / distance;
            
            // Current value
            float currValue = table[rIdx][lIdx];
            
            // Blend with new value, weight diminishes with distance
            float targetValue = (1.0f - weight) * currValue + weight * newValue;
            
            // Apply the smoothed value
            table[rIdx][lIdx] = static_cast<int8_t>(targetValue);
        }
    }
}

int WallFuelAdaptation::findBinIndex(float value, const int8_t* bins, int size) const {
    // Handle edge cases
    if (size <= 0) {
        return -1;
    }
    
    if (value <= bins[0]) {
        return 0;
    }
    
    if (value >= bins[size - 1]) {
        return size - 1;
    }
    
    // Find the bin using linear search
    // We could use binary search for more efficiency, but the arrays are small
    for (int i = 0; i < size - 1; i++) {
        if (value >= bins[i] && value < bins[i + 1]) {
            return i;
        }
    }
    
    // Shouldn't reach here if input is within range
    return -1;
}

// Helper function for interpolation
float WallFuelAdaptation::interpolateCurve(float x, float y0, float y1) const {
    return y0 * (1 - x) + y1 * x;
} 