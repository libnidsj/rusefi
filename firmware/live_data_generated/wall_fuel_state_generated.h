// this section was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/wall_fuel_state.txt
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of wall_fuel_state_s
struct wall_fuel_state_s {
	/**
	 * wall wetting fuel correction
	 * offset 0
	 */
	float wallFuelCorrection = (float)0;
	/**
	 * fuel on the wall
	 * offset 4
	 */
	floatms_t wallFuel = (floatms_t)0;
};
static_assert(sizeof(wall_fuel_state_s) == 8);

// end
// this section was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/wall_fuel_state.txt
