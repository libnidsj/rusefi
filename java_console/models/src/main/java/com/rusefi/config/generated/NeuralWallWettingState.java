package com.rusefi.config.generated;

// this file was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/neural_wall_wetting_state.txt

// by class com.rusefi.output.FileJavaFieldsConsumer
import com.rusefi.config.*;

public class NeuralWallWettingState {
	public static final Field NEURALBETACORRECTION = Field.create("NEURALBETACORRECTION", 0, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field NEURALTAUCORRECTION = Field.create("NEURALTAUCORRECTION", 4, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field NEURALINFERENCECOUNT = Field.create("NEURALINFERENCECOUNT", 8, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field NEURALTRAININGCOUNT = Field.create("NEURALTRAININGCOUNT", 12, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field PENDINGTRANSIENTSCOUNT = Field.create("PENDINGTRANSIENTSCOUNT", 16, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field LASTTRANSIENTSCORE = Field.create("LASTTRANSIENTSCORE", 20, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field AVERAGENEURALSCORE = Field.create("AVERAGENEURALSCORE", 24, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field AVERAGECLASSICSCORE = Field.create("AVERAGECLASSICSCORE", 28, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field LEARNINGPHASE = Field.create("LEARNINGPHASE", 32, FieldType.INT8).setScale(1.0).setBaseOffset(1640);
	public static final Field ALIGNMENTFILL_AT_33 = Field.create("ALIGNMENTFILL_AT_33", 33, FieldType.INT8).setScale(1.0).setBaseOffset(1640);
	public static final Field CURRENTLEARNINGRATE = Field.create("CURRENTLEARNINGRATE", 36, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field TOTALREWARDSAPPLIED = Field.create("TOTALREWARDSAPPLIED", 40, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field TOTALPENALTIESAPPLIED = Field.create("TOTALPENALTIESAPPLIED", 44, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field DEBUGLASTNEURALBETA = Field.create("DEBUGLASTNEURALBETA", 48, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field DEBUGLASTNEURALTAU = Field.create("DEBUGLASTNEURALTAU", 52, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field DEBUGLASTCLASSICBETA = Field.create("DEBUGLASTCLASSICBETA", 56, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field DEBUGLASTCLASSICTAU = Field.create("DEBUGLASTCLASSICTAU", 60, FieldType.FLOAT).setBaseOffset(1640);
	public static final Field DEBUGCONVERGENCECOUNTER = Field.create("DEBUGCONVERGENCECOUNTER", 64, FieldType.INT).setScale(1.0).setBaseOffset(1640);
	public static final Field[] VALUES = {
	NEURALBETACORRECTION,
	NEURALTAUCORRECTION,
	NEURALINFERENCECOUNT,
	NEURALTRAININGCOUNT,
	PENDINGTRANSIENTSCOUNT,
	LASTTRANSIENTSCORE,
	AVERAGENEURALSCORE,
	AVERAGECLASSICSCORE,
	LEARNINGPHASE,
	ALIGNMENTFILL_AT_33,
	CURRENTLEARNINGRATE,
	TOTALREWARDSAPPLIED,
	TOTALPENALTIESAPPLIED,
	DEBUGLASTNEURALBETA,
	DEBUGLASTNEURALTAU,
	DEBUGLASTCLASSICBETA,
	DEBUGLASTCLASSICTAU,
	DEBUGCONVERGENCECOUNTER,
	};
}
