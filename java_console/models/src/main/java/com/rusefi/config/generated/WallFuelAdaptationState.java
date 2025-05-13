package com.rusefi.config.generated;

// this file was generated automatically by rusEFI tool config_definition_base-all.jar based on (unknown script) controllers/algo/wall_fuel_adaptation_state.txt

// by class com.rusefi.output.FileJavaFieldsConsumer
import com.rusefi.config.*;

public class WallFuelAdaptationState {
	public static final Field ISADAPTATIONACTIVE = Field.create("ISADAPTATIONACTIVE", 0, FieldType.BIT, 0).setBaseOffset(1624);
	public static final Field LASTLAMBDAERROR = Field.create("LASTLAMBDAERROR", 4, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field TAUINTEGRAL = Field.create("TAUINTEGRAL", 8, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field BETAINTEGRAL = Field.create("BETAINTEGRAL", 12, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field LASTTAUCORRECTION = Field.create("LASTTAUCORRECTION", 16, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field LASTBETACORRECTION = Field.create("LASTBETACORRECTION", 20, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field M_STEADYSTATETIME = Field.create("M_STEADYSTATETIME", 24, FieldType.FLOAT).setBaseOffset(1624);
	public static final Field[] VALUES = {
	ISADAPTATIONACTIVE,
	LASTLAMBDAERROR,
	TAUINTEGRAL,
	BETAINTEGRAL,
	LASTTAUCORRECTION,
	LASTBETACORRECTION,
	M_STEADYSTATETIME,
	};
}
