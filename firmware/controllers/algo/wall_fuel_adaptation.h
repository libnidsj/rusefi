/*
 * @file wall_fuel_adaptation.h
 *
 * Online adaptation of wall wetting model parameters.
 * Based on SAE 2001-MECA01 paper "On-Line Adaptation of Wall-Wetting Model Parameters"
 * Uses recursive least squares method to adapt tau and beta parameters based on lambda feedback.
 *
 * @author Implementation based on SAE 2001-MECA01 paper
 * Copyright (c) 2025
 */

#pragma once

#include "engine_module.h"
#include "wall_fuel_adaptation_state_generated.h"

// Logging levels for wall wetting adaptation
#define WALL_WETTING_ADAPTATION_LOGGING_LEVEL 2
#define WALL_WETTING_ADAPTATION_LOGGING (loggingLevel == WALL_WETTING_ADAPTATION_LOGGING_LEVEL)

class WallFuelAdaptation : public EngineModule, public wall_fuel_adaptation_state_s {
public:
    WallFuelAdaptation();
    void reset();

    // EngineModule implementation
    void onSlowCallback() override;
    void onFastCallback() override;
    void onIgnitionStateChanged(bool ignitionState) override;

    /**
     * Get the current tau correction factor
     * @param rpm Current engine RPM
     * @param load Current engine load (typically MAP)
     * @param clt Current coolant temperature
     * @return Tau correction factor (multiplier)
     */
    float getTauCorrection(float rpm, float load, float clt) const;

    /**
     * Get the current beta correction factor
     * @param rpm Current engine RPM
     * @param load Current engine load (typically MAP)
     * @param clt Current coolant temperature
     * @return Beta correction factor (multiplier)
     */
    float getBetaCorrection(float rpm, float load, float clt) const;

private:
    /**
     * Reset adaptation tables to default values
     */
    void resetAdaptationTables();

    /**
     * Calculate the lambda error for adaptation
     * Lambda error = measured lambda / target lambda - 1
     * Positive value means lean, negative means rich
     */
    float calculateLambdaError() const;

    /**
     * Adapt parameters based on lambda error using recursive least squares
     * @param lambdaError Current lambda error
     */
    void adaptParameters(float lambdaError);

    /**
     * Update correction tables with new adaptations
     * @param tauCorrection New tau correction factor
     * @param betaCorrection New beta correction factor
     */
    void updateCorrectionTables(float tauCorrection, float betaCorrection);

    /**
     * Apply antiwindup to limit integral accumulation
     * @param integral Current integral value
     * @param limit Maximum allowed value (positive)
     * @return Limited integral value
     */
    float applyAntiWindup(float integral, float limit) const;

    /**
     * Interpolate correction value from 3D table
     * @param rpm Current engine RPM
     * @param load Current engine load
     * @param table Table to interpolate from
     * @return Interpolated correction value
     */
    float interpolateCorrection(float rpm, float load, const int8_t table[WWAE_ADAPT_RPM_SIZE][WWAE_ADAPT_LOAD_SIZE]) const;

    /**
     * Apply smoothing to adjacent cells in 3D table
     * This prevents abrupt changes between cells
     * @param table Table to smooth
     * @param centerRpmIdx Center cell RPM index
     * @param centerLoadIdx Center cell load index
     * @param newValue New value at the center cell
     */
    void applySmoothingTo3DTable(int8_t table[WWAE_ADAPT_RPM_SIZE][WWAE_ADAPT_LOAD_SIZE], int centerRpmIdx, int centerLoadIdx, int8_t newValue);

    // RLS algorithm state variables
    float tauCovariance = 1.0f;    // Covariance for tau parameter estimation
    float betaCovariance = 1.0f;   // Covariance for beta parameter estimation

    // Engine state tracking
    float m_engineRunTime = 0;     // Total engine run time since start
    float m_lambdaFeedbackTime = 0; // Time with valid lambda feedback
};

/**
 * Find the bin index for a value in an array
 * @param value Value to search for
 * @param bins Array of bin values
 * @param size Size of the array
 * @return Index of the bin, or -1 if not found
 */
int findBinIndex(float value, const int8_t* bins, int size); 