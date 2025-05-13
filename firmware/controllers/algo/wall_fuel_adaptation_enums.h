/**
 * @file wall_fuel_adaptation_enums.h
 * 
 * Enumeration definitions for wall fuel adaptation module
 */

#pragma once

/**
 * Adaptation mode for wall wetting parameters
 */
typedef enum {
    WW_ADAPT_DISABLED = 0, // Adaptation disabled 
    WW_ADAPT_TAU_ONLY = 1, // Adapt tau parameter only
    WW_ADAPT_BETA_ONLY = 2, // Adapt beta parameter only
    WW_ADAPT_BOTH = 3,     // Adapt both tau and beta parameters
} wall_wetting_adaptation_mode_e;

/**
 * Cell mode for wall wetting adaptation
 */
typedef enum {
    WW_CELL_BASED = 0,     // Each cell adapts independently
    WW_GLOBAL = 1,         // One global adaptation for all cells
    WW_LOAD_INTERPOLATED = 2, // Adaptation interpolated based on load
    WW_FULL_3D = 3,        // Full 3D interpolation between cells
} wall_wetting_adaptation_cell_mode_e; 