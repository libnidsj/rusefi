// this section was generated automatically by rusEFI tool ConfigDefinition.jar based on (unknown script) controllers/algo/ignition_state.txt Sat Apr 16 23:31:02 UTC 2022
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of ignition_state_s
struct ignition_state_s {
	/**
	 * offset 0
	 */
	float baseDwell = (float)0;
	/**
	 * offset 4
	 */
	float dwellVoltageCorrection = (float)0;
	/**
	 * offset 8
	 */
	float luaTimingAdd = (float)0;
	/**
	 * offset 12
	 */
	float luaTimingMult = (float)0;
	/** total size 16*/
};

// end
// this section was generated automatically by rusEFI tool ConfigDefinition.jar based on (unknown script) controllers/algo/ignition_state.txt Sat Apr 16 23:31:02 UTC 2022
