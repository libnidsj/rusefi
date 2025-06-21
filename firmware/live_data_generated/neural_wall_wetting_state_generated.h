// this section was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/neural_wall_wetting_state.txt
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of neural_wall_wetting_state_s
struct neural_wall_wetting_state_s {
	/**
	 * Neural beta correction applied
	 * offset 0
	 */
	float neuralBetaCorrection = (float)0;
	/**
	 * Neural tau correction applied
	 * offset 4
	 */
	float neuralTauCorrection = (float)0;
	/**
	 * Neural network inference count
	 * offset 8
	 */
	uint32_t neuralInferenceCount = (uint32_t)0;
	/**
	 * Neural network training steps
	 * offset 12
	 */
	uint32_t neuralTrainingCount = (uint32_t)0;
	/**
	 * Pending transients count
	 * offset 16
	 */
	uint32_t pendingTransientsCount = (uint32_t)0;
	/**
	 * Last transient performance score
	 * offset 20
	 */
	float lastTransientScore = (float)0;
	/**
	 * Average neural performance
	 * offset 24
	 */
	float averageNeuralScore = (float)0;
	/**
	 * Average classic performance
	 * offset 28
	 */
	float averageClassicScore = (float)0;
	/**
	 * Learning phase (0-3)
	 * offset 32
	 */
	uint8_t learningPhase = (uint8_t)0;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 33
	 */
	uint8_t alignmentFill_at_33[3] = {};
	/**
	 * Current learning rate
	 * offset 36
	 */
	float currentLearningRate = (float)0;
	/**
	 * Total rewards applied
	 * offset 40
	 */
	uint32_t totalRewardsApplied = (uint32_t)0;
	/**
	 * Total penalties applied
	 * offset 44
	 */
	uint32_t totalPenaltiesApplied = (uint32_t)0;
	/**
	 * Last neural beta used
	 * offset 48
	 */
	float debugLastNeuralBeta = (float)0;
	/**
	 * Last neural tau used
	 * offset 52
	 */
	float debugLastNeuralTau = (float)0;
	/**
	 * Last classic beta used
	 * offset 56
	 */
	float debugLastClassicBeta = (float)0;
	/**
	 * Last classic tau used
	 * offset 60
	 */
	float debugLastClassicTau = (float)0;
	/**
	 * Convergence counter
	 * offset 64
	 */
	uint32_t debugConvergenceCounter = (uint32_t)0;
};
static_assert(sizeof(neural_wall_wetting_state_s) == 68);

// end
// this section was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/neural_wall_wetting_state.txt
