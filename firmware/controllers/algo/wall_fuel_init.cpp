/**
 * @file wall_fuel_init.cpp
 *
 * Initialization code for the wall wetting model and adaptation.
 * 
 * @author Implementation based on SAE 2001-MECA01 paper
 * Copyright (c) 2025
 */

#include "pch.h"
#include "engine.h"
#include "wall_fuel.h"
#include "wall_fuel_adaptation.h"

void initWallFuel() {
    // Initialize the wall fuel controller
    engine->module<WallFuelController>()->reset();
}

void initWallFuelAdaptation() {
    // Initialize the wall fuel adaptation module
    engine->module<WallFuelAdaptation>()->reset();
} 