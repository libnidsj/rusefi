#pragma once

#include "engine_module.h"
#include "table_helper.h"
#include "engine_configuration.h"

// Define table dimensions (should match config structure)
#define LTIT_RPM_BINS 8
#define LTIT_CLT_BINS 8

class LongTermIdleTrim : public EngineModule {
public:
    LongTermIdleTrim();

    void init();

    // Called periodically to update the LTIT table
    void onSlowCallback() override;

    // Returns the interpolated LTIT correction value (% additive)
    float getLtitCorrection(float rpm, float clt);

    // Called when ignition state changes to handle persistence
    void onIgnitionStateChanged(bool ignitionState) override;

    // Method to reset the LTIT table (e.g., via TS command)
    void resetTable();

private:
    // Helper table in RAM for real-time updates
    float ltitTableHelper[LTIT_RPM_BINS][LTIT_CLT_BINS];

    // Tracks time spent in each cell for learning condition
    float timeInCell[LTIT_RPM_BINS][LTIT_CLT_BINS];

    // Timestamp of the last table update
    efitimeus_t lastUpdateTimeUs = 0;

    // Flag indicating if the helper table has been updated since last save
    bool updatedLtit = false;

    // Internal helper to update a specific cell
    void updateCell(int idxRpm, int idxClt, float rpm, float clt, float targetRpm);
};

