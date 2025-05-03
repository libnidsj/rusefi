#include "pch.h"
#include "long_term_idle_trim.h"
#include "engine.h"
#include "idle_thread.h"
#include "flash_main.h"
#include "table_helper.h"
#include "sensor.h"
#include "ac_control.h"
#include "fan_control.h"

// Define table dimensions again for safety (or include from a shared place?)
#ifndef LTIT_RPM_BINS
#define LTIT_RPM_BINS 8
#endif
#ifndef LTIT_CLT_BINS
#define LTIT_CLT_BINS 8
#endif

LongTermIdleTrim::LongTermIdleTrim() : EngineModule(true /*needsSlowCallback*/) {
    // Initialize helper table and timeInCell to zero
    memset(ltitTableHelper, 0, sizeof(ltitTableHelper));
    memset(timeInCell, 0, sizeof(timeInCell));
}

void LongTermIdleTrim::init() {
    // Load persistent table into helper table on startup
    copyTable(ltitTableHelper, config->ltit_config.ltitTable);
    updatedLtit = false; // Ensure flag is reset on init
    lastUpdateTimeUs = 0;
    memset(timeInCell, 0, sizeof(timeInCell)); // Reset time tracking
}

void LongTermIdleTrim::onSlowCallback() {
    ScopePerf perf(PE::LongTermIdleTrimCallback);

    // Check if LTIT is enabled
    if (!config->ltit_config.ltitEnabled) {
        // If disabled, maybe reset timeInCell?
        // memset(timeInCell, 0, sizeof(timeInCell));
        return;
    }

    // Get current engine state
    float rpm = Sensor::getOrZero(SensorType::Rpm);
    float clt = Sensor::get(SensorType::Clt).value_or(90.0f); // Default to 90C if sensor fails
    SensorResult tps = Sensor::get(SensorType::Tps1);
    float vss = Sensor::getOrZero(SensorType::VehicleSpeed);
    bool isAcOn = engine->module<AcController>().unmock().isAcEnabled();
    bool isFanOn = engine->module<FanController>().unmock().isFanOn();

    // Get idle controller state
    auto* idleController = engine->module<IdleController>();
    int targetRpm = idleController->getTargetRpm(clt);
    float crankingTaper = idleController->getCrankingTaperFraction();
    IIdleController::Phase currentPhase = idleController->determinePhase(rpm, targetRpm, tps, vss, crankingTaper);

    // --- Learning Conditions Check ---
    bool learningAllowed = true;

    // 1. Must be in Idling phase
    if (currentPhase != IIdleController::Phase::Idling) {
        learningAllowed = false;
    }

    // 2. RPM within learning range
    if (rpm < config->ltit_config.ltitMinRpm || rpm > config->ltit_config.ltitMaxRpm) {
        learningAllowed = false;
    }

    // 3. RPM close enough to target
    if (absF(rpm - targetRpm) > config->ltit_config.ltitMaxRpmError) {
        learningAllowed = false;
    }

    // 4. AC off (if configured)
    if (config->ltit_config.ltitDisableWithAc && isAcOn) {
        learningAllowed = false;
    }

    // 5. Fan off (if configured)
    if (config->ltit_config.ltitDisableWithFan && isFanOn) {
        learningAllowed = false;
    }

    // --- Update Logic ---
    efitick_t nowNt = getTimeNowNt();
    efitimeus_t nowUs = NT2US(nowNt);
    float timeDeltaS = (float)US2S(nowUs - engine->previousSlowCallbackTimeUs);

    // Determine current cell indices
    int idxRpm = findIndex(config->ltit_config.ltitRpmBins, LTIT_RPM_BINS, rpm);
    int idxClt = findIndex(config->ltit_config.ltitCltBins, LTIT_CLT_BINS, clt);

    if (learningAllowed) {
        // Increment time spent in the current cell
        timeInCell[idxRpm][idxClt] += timeDeltaS;

        // Check if enough time spent in cell and update interval passed
        bool timeInCellMet = timeInCell[idxRpm][idxClt] >= config->ltit_config.ltitMinTimeInCell;
        bool updateIntervalMet = (nowUs - lastUpdateTimeUs) >= S2US(config->ltit_config.ltitUpdateInterval);

        if (timeInCellMet && updateIntervalMet) {
            updateCell(idxRpm, idxClt, rpm, clt, targetRpm);
            // Reset time for this cell after update
            timeInCell[idxRpm][idxClt] = 0;
            lastUpdateTimeUs = nowUs;
        }
    } else {
        // If conditions not met, reset time counter for the current cell
        // This prevents accumulating time when conditions are fluctuating
        timeInCell[idxRpm][idxClt] = 0;
        // Optionally reset all cells? Probably not necessary.
        // memset(timeInCell, 0, sizeof(timeInCell));
    }

    // Update previous time for next delta calculation
    // engine->previousSlowCallbackTimeUs = nowUs; // This should be handled globally by the engine loop
}

void LongTermIdleTrim::updateCell(int idxRpm, int idxClt, float rpm, float clt, float targetRpm) {
    // Get the current short-term correction from the idle PID
    // We want the part of the PID output that's correcting for steady-state error.
    // Using the integral term is a common approach for LTFT.
    Pid* pid = engine->module<IdleController>()->getIdlePid();
    float shortTermCorrection = pid->getIntegralTerm(); // Get the I-term value (%)

    // Alternative: Use total PID output? Might be simpler.
    // float pidOutput = pid->getOutput();
    // float openLoopBase = engine->module<IdleController>()->getOpenLoop(...); // Need to recalculate base
    // float shortTermCorrection = pidOutput - openLoopBase; // Correction applied by PID

    // Calculate the desired change in LTIT
    // We want LTIT to absorb the steady-state correction being done by the I-term.
    // If I-term is positive (adding position), LTIT should increase.
    // If I-term is negative (subtracting position), LTIT should decrease.
    float correctionDelta = shortTermCorrection; // Directly use I-term as the delta target

    // Limit the rate of change
    float maxChangePerCycle = config->ltit_config.ltitMaxCorrectionRate * (float)US2S(getTimeNowUs() - lastUpdateTimeUs);
    float actualChange = clampF(correctionDelta, -maxChangePerCycle, maxChangePerCycle);

    // Get current LTIT value from helper table
    float currentCorrection = ltitTableHelper[idxRpm][idxClt];

    // Calculate new LTIT value
    float newCorrection = currentCorrection + actualChange;

    // Clamp the total correction within reasonable bounds (e.g., +/- 25%)
    // These bounds should ideally match the table definition in rusefi_config.txt
    newCorrection = clampF(-25.0f, newCorrection, 25.0f);

    // Update the helper table only if there's a significant change?
    if (absF(newCorrection - currentCorrection) > 0.01f) { // Avoid tiny updates due to float precision
        ltitTableHelper[idxRpm][idxClt] = newCorrection;
        updatedLtit = true;
        // Log update?
        // efiPrintf("LTIT Update: RPM=%d, CLT=%d, Idx=(%d,%d), ST=%f, Delta=%f, New=%f", (int)rpm, (int)clt, idxRpm, idxClt, shortTermCorrection, actualChange, newCorrection);
    }
}

float LongTermIdleTrim::getLtitCorrection(float rpm, float clt) {
    if (!config->ltit_config.ltitEnabled) {
        return 0.0f;
    }

    // Interpolate the correction value from the helper table
    return interpolate2d(rpm, clt,
                         config->ltit_config.ltitRpmBins,
                         config->ltit_config.ltitCltBins,
                         (float*)ltitTableHelper);
}

void LongTermIdleTrim::onIgnitionStateChanged(bool ignitionState) {
    if (!ignitionState && updatedLtit) {
        // Ignition turned off, and we have updates to save
        efiPrintf("Saving updated LTIT table...");
        copyTable(config->ltit_config.ltitTable, ltitTableHelper);
        setNeedToWriteConfiguration();
        updatedLtit = false; // Reset flag after saving
    }
}

void LongTermIdleTrim::resetTable() {
    efiPrintf("Resetting LTIT table...");
    // Reset both helper and config table to zero
    memset(ltitTableHelper, 0, sizeof(ltitTableHelper));
    memset(config->ltit_config.ltitTable, 0, sizeof(config->ltit_config.ltitTable));
    // Mark as updated so the reset gets saved on shutdown
    updatedLtit = true;
    // Trigger save immediately? Or wait for shutdown?
    // setNeedToWriteConfiguration(); // Optional: save immediately
}

