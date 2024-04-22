// generated by class com.rusefi.output.SdCardFieldsContent
static const LogField fields[] = {
{packedTime, GAUGE_NAME_TIME, "sec", 0},
	{engine->outputChannels.RPMValue, "RPM", "RPM", 0},
	{engine->outputChannels.rpmAcceleration, "dRPM", "RPM acceleration", 2},
	{engine->outputChannels.speedToRpmRatio, "Gearbox Ratio", "value", 2},
	{engine->outputChannels.internalMcuTemperature, "CPU Temperature", "deg C", 0},
	{engine->outputChannels.coolant, "CLT", "deg C", 1},
	{engine->outputChannels.intake, "IAT", "deg C", 1},
	{engine->outputChannels.auxTemp1, "auxTemp1", "deg C", 1},
	{engine->outputChannels.auxTemp2, "auxTemp2", "deg C", 1},
	{engine->outputChannels.TPSValue, "TPS", "%", 2},
	{engine->outputChannels.throttlePedalPosition, "Throttle pedal position", "%", 2},
	{engine->outputChannels.tpsADC, "tpsADC", "ADC", 0},
	{engine->outputChannels.rawMaf, "rawMaf", "V", 3},
	{engine->outputChannels.mafMeasured, "MAF", "kg/h", 1},
	{engine->outputChannels.MAPValue, "MAP", "kPa", 1},
	{engine->outputChannels.baroPressure, "baroPressure", "kPa", 1},
	{engine->outputChannels.lambdaValue, "Lambda", "", 3},
	{engine->outputChannels.VBatt, "VBatt", "V", 2},
	{engine->outputChannels.oilPressure, "Oil Pressure", "kPa", 0},
	{engine->outputChannels.vvtPositionB1I, "VVT: bank 1 intake", "deg", 1},
	{engine->outputChannels.actualLastInjection, "Fuel: Last inj pulse width", "ms", 3},
	{engine->outputChannels.injectorDutyCycle, "Fuel: injector duty cycle", "%", 0},
	{engine->outputChannels.injectionOffset, "Fuel: Injection timing SOI", "deg", 0},
	{engine->outputChannels.engineMakeCodeNameCrc16, "Engine CRC16", "crc16", 0},
	{engine->outputChannels.wallFuelAmount, "Fuel: wall amount", "mg", 3},
	{engine->outputChannels.wallFuelCorrectionValue, "Fuel: wall correction", "mg", 3},
	{engine->outputChannels.revolutionCounterSinceStart, "revolutionCounterSinceStart", "", 0},
	{engine->outputChannels.canReadCounter, "CAN: Rx", "", 0},
	{engine->outputChannels.tpsAccelFuel, "Fuel: TPS AE add fuel ms", "ms", 3},
	{engine->outputChannels.currentIgnitionMode, "Ignition: Mode", "", 0},
	{engine->outputChannels.currentInjectionMode, "Fuel: Injection mode", "", 0},
	{engine->outputChannels.coilDutyCycle, "Ignition: coil duty cycle", "%", 0},
	{engine->outputChannels.etbTarget, "ETB Target", "%", 2},
	{engine->outputChannels.etb1DutyCycle, "ETB: Duty", "%", 1},
	{engine->outputChannels.fuelTankLevel, "Fuel level", "%", 0},
	{engine->outputChannels.totalFuelConsumption, "Fuel: Total consumed", "grams", 1},
	{engine->outputChannels.fuelFlowRate, "Fuel: Flow rate", "gram/s", 3},
	{engine->outputChannels.TPS2Value, "TPS2", "%", 2},
	{engine->outputChannels.tuneCrc16, "Tune CRC16", "crc16", 0},
	{engine->outputChannels.veValue, "Fuel: VE", "ratio", 1},
	{engine->outputChannels.seconds, "Uptime", "sec", 0},
	{engine->outputChannels.engineMode, "Engine Mode", "em", 0},
	{engine->outputChannels.firmwareVersion, "firmware", "version_f", 0},
	{engine->outputChannels.rawIdlePositionSensor, "rawIdlePositionSensor", "V", 3},
	{engine->outputChannels.rawWastegatePosition, "rawWastegatePosition", "V", 3},
	{engine->outputChannels.accelerationLat, "Accel: Lateral", "G", 0},
	{engine->outputChannels.accelerationLon, "Accel: Longitudinal", "G", 0},
	{engine->outputChannels.detectedGear, "Detected Gear", "", 0},
	{engine->outputChannels.maxTriggerReentrant, "maxTriggerReentrant", "", 0},
	{engine->outputChannels.rawLowFuelPressure, "rawLowFuelPressure", "V", 3},
	{engine->outputChannels.rawHighFuelPressure, "rawHighFuelPressure", "V", 3},
	{engine->outputChannels.lowFuelPressure, "Fuel pressure (low)", "kpa", 0},
	{engine->outputChannels.tcuDesiredGear, "TCU: Desired Gear", "gear", 0},
	{engine->outputChannels.flexPercent, "Flex Ethanol %", "%", 1},
	{engine->outputChannels.wastegatePositionSensor, "Wastegate position sensor", "%", 2},
	{engine->outputChannels.highFuelPressure, "Fuel pressure (high)", "bar", 0},
	{engine->outputChannels.calibrationValue, "calibrationValue", "", 0},
	{engine->outputChannels.calibrationMode, "calibrationMode", "", 0},
	{engine->outputChannels.idleStepperTargetPosition, "Idle: Stepper target position", "", 0},
	{engine->outputChannels.tsConfigVersion, "tsConfigVersion", "", 0},
	{engine->outputChannels.totalTriggerErrorCounter, "Trigger Error Counter", "counter", 0},
	{engine->outputChannels.orderingErrorCounter, "orderingErrorCounter", "", 0},
	{engine->outputChannels.warningCounter, "Warning: counter", "count", 0},
	{engine->outputChannels.lastErrorCode, "Warning: last", "error", 0},
	{engine->outputChannels.recentErrorCode[0], "Warning code 1", "error", 0},
	{engine->outputChannels.recentErrorCode[1], "Warning code 2", "error", 0},
	{engine->outputChannels.recentErrorCode[2], "Warning code 3", "error", 0},
	{engine->outputChannels.recentErrorCode[3], "Warning code 4", "error", 0},
	{engine->outputChannels.recentErrorCode[4], "Warning code 5", "error", 0},
	{engine->outputChannels.recentErrorCode[5], "Warning code 6", "error", 0},
	{engine->outputChannels.recentErrorCode[6], "Warning code 7", "error", 0},
	{engine->outputChannels.recentErrorCode[7], "Warning code 8", "error", 0},
	{engine->outputChannels.debugFloatField1, "debugFloatField1", "val", 3},
	{engine->outputChannels.debugFloatField2, "debugFloatField2", "val", 3},
	{engine->outputChannels.debugFloatField3, "debugFloatField3", "val", 3},
	{engine->outputChannels.debugFloatField4, "debugFloatField4", "val", 3},
	{engine->outputChannels.debugFloatField5, "debugFloatField5", "val", 3},
	{engine->outputChannels.debugFloatField6, "debugFloatField6", "val", 3},
	{engine->outputChannels.debugFloatField7, "debugFloatField7", "val", 3},
	{engine->outputChannels.debugIntField1, "debugIntField1", "val", 0},
	{engine->outputChannels.debugIntField2, "debugIntField2", "val", 0},
	{engine->outputChannels.debugIntField3, "debugIntField3", "val", 0},
	{engine->outputChannels.debugIntField4, "debugIntField4", "val", 0},
	{engine->outputChannels.debugIntField5, "debugIntField5", "val", 0},
	{engine->outputChannels.egt[0], "EGT 1", "deg C", 2},
	{engine->outputChannels.egt[1], "EGT 2", "deg C", 2},
	{engine->outputChannels.egt[2], "EGT 3", "deg C", 2},
	{engine->outputChannels.egt[3], "EGT 4", "deg C", 2},
	{engine->outputChannels.egt[4], "EGT 5", "deg C", 2},
	{engine->outputChannels.egt[5], "EGT 6", "deg C", 2},
	{engine->outputChannels.egt[6], "EGT 7", "deg C", 2},
	{engine->outputChannels.egt[7], "EGT 8", "deg C", 2},
	{engine->outputChannels.rawTps1Primary, "rawTps1Primary", "V", 3},
	{engine->outputChannels.rawClt, "rawClt", "V", 3},
	{engine->outputChannels.rawIat, "rawIat", "V", 3},
	{engine->outputChannels.rawOilPressure, "rawOilPressure", "V", 3},
	{engine->outputChannels.fuelClosedLoopBinIdx, "fuelClosedLoopBinIdx", "", 0},
	{engine->outputChannels.rawPpsPrimary, "rawPpsPrimary", "V", 3},
	{engine->outputChannels.rawPpsSecondary, "rawPpsSecondary", "V", 3},
	{engine->outputChannels.rawRawPpsPrimary, "rawRawPpsPrimary", "V", 3},
	{engine->outputChannels.rawRawPpsSecondary, "rawRawPpsSecondary", "V", 3},
	{engine->outputChannels.idlePositionSensor, "Idle: Position sensor", "%", 2},
	{engine->outputChannels.AFRValue, "Air/Fuel Ratio", "AFR", 2},
	{engine->outputChannels.VssAcceleration, "Vss Accel", "m/s2", 2},
	{engine->outputChannels.lambdaValue2, "Lambda 2", "", 3},
	{engine->outputChannels.AFRValue2, "Air/Fuel Ratio 2", "AFR", 2},
	{engine->outputChannels.vvtPositionB1E, "VVT: bank 1 exhaust", "deg", 1},
	{engine->outputChannels.vvtPositionB2I, "VVT: bank 2 intake", "deg", 1},
	{engine->outputChannels.vvtPositionB2E, "VVT: bank 2 exhaust", "deg", 1},
	{engine->outputChannels.fuelPidCorrection[0], "Fuel: Trim bank 1", "%", 2},
	{engine->outputChannels.fuelPidCorrection[1], "Fuel: Trim bank 2", "%", 2},
	{engine->outputChannels.rawTps1Secondary, "rawTps1Secondary", "V", 3},
	{engine->outputChannels.rawTps2Primary, "rawTps2Primary", "V", 3},
	{engine->outputChannels.rawTps2Secondary, "rawTps2Secondary", "V", 3},
	{engine->outputChannels.accelerationVert, "Accel: Vertical", "G", 0},
	{engine->outputChannels.gyroYaw, "Gyro: Yaw rate", "deg/sec", 0},
	{engine->outputChannels.vvtTargets[0], "vvtTargets 1", "deg", 0},
	{engine->outputChannels.vvtTargets[1], "vvtTargets 2", "deg", 0},
	{engine->outputChannels.vvtTargets[2], "vvtTargets 3", "deg", 0},
	{engine->outputChannels.vvtTargets[3], "vvtTargets 4", "deg", 0},
	{engine->outputChannels.turboSpeed, "Turbocharger Speed", "hz", 0},
	{engine->outputChannels.ignitionAdvanceCyl[0], "Ign: Timing Cyl 1", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[1], "Ign: Timing Cyl 2", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[2], "Ign: Timing Cyl 3", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[3], "Ign: Timing Cyl 4", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[4], "Ign: Timing Cyl 5", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[5], "Ign: Timing Cyl 6", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[6], "Ign: Timing Cyl 7", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[7], "Ign: Timing Cyl 8", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[8], "Ign: Timing Cyl 9", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[9], "Ign: Timing Cyl 10", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[10], "Ign: Timing Cyl 11", "deg", 1},
	{engine->outputChannels.ignitionAdvanceCyl[11], "Ign: Timing Cyl 12", "deg", 1},
	{engine->outputChannels.tps1Split, "tps1Split", "%", 2},
	{engine->outputChannels.tps2Split, "tps2Split", "%", 2},
	{engine->outputChannels.tps12Split, "tps12Split", "%", 2},
	{engine->outputChannels.accPedalSplit, "accPedalSplit", "%", 2},
	{engine->outputChannels.sparkCutReason, "Ign: Cut Code", "code", 0},
	{engine->outputChannels.fuelCutReason, "Fuel: Cut Code", "code", 0},
	{engine->outputChannels.mafEstimate, "Air: Flow estimate", "kg/h", 0},
	{engine->outputChannels.instantRpm, "sync: instant RPM", "rpm", 0},
	{engine->outputChannels.rawMap, "rawMap", "V", 3},
	{engine->outputChannels.rawAfr, "rawAfr", "V", 3},
	{engine->outputChannels.calibrationValue2, "calibrationValue2", "", 0},
	{engine->outputChannels.luaInvocationCounter, "Lua: Tick counter", "count", 0},
	{engine->outputChannels.luaLastCycleDuration, "Lua: Last tick duration", "nt", 0},
	{engine->outputChannels.vssEdgeCounter, "vssEdgeCounter", "", 0},
	{engine->outputChannels.issEdgeCounter, "issEdgeCounter", "", 0},
	{engine->outputChannels.auxLinear1, "Aux linear #1", "", 2},
	{engine->outputChannels.auxLinear2, "Aux linear #2", "", 2},
	{engine->outputChannels.fallbackMap, "fallbackMap", "kPa", 1},
	{engine->outputChannels.instantMAPValue, "Instant MAP", "kPa", 2},
	{engine->outputChannels.maxLockedDuration, "maxLockedDuration", "us", 0},
	{engine->outputChannels.canWriteOk, "CAN: Tx OK", "", 0},
	{engine->outputChannels.canWriteNotOk, "CAN: Tx err", "", 0},
	{engine->outputChannels.triggerPrimaryFall, "triggerPrimaryFall", "", 3},
	{engine->outputChannels.triggerPrimaryRise, "triggerPrimaryRise", "", 3},
	{engine->outputChannels.triggerSecondaryFall, "triggerSecondaryFall", "", 3},
	{engine->outputChannels.triggerSecondaryRise, "triggerSecondaryRise", "", 3},
	{engine->outputChannels.triggerVvtFall, "triggerVvtFall", "", 0},
	{engine->outputChannels.triggerVvtRise, "triggerVvtRise", "", 0},
	{engine->outputChannels.triggerVvt2Fall, "triggerVvt2Fall", "", 0, "Sync"},
	{engine->outputChannels.triggerVvt2Rise, "triggerVvt2Rise", "", 0, "Sync"},
	{engine->outputChannels.starterState, "starterState", "", 3},
	{engine->outputChannels.starterRelayDisable, "starterRelayDisable", "", 3},
	{engine->outputChannels.multiSparkCounter, "Ign: Multispark count", "", 3},
	{engine->outputChannels.extiOverflowCount, "extiOverflowCount", "", 0},
	{engine->outputChannels.alternatorStatus.pTerm, "alternatorStatus.pTerm", "", 2},
	{engine->outputChannels.alternatorStatus.iTerm, "alternatorStatus.iTerm", "", 2},
	{engine->outputChannels.alternatorStatus.dTerm, "alternatorStatus.dTerm", "", 2},
	{engine->outputChannels.alternatorStatus.output, "alternatorStatus.output", "", 2},
	{engine->outputChannels.alternatorStatus.error, "alternatorStatus.error", "", 2},
	{engine->outputChannels.alternatorStatus.resetCounter, "alternatorStatus.resetCounter", "", 0},
	{engine->outputChannels.idleStatus.pTerm, "idleStatus.pTerm", "", 2},
	{engine->outputChannels.idleStatus.iTerm, "idleStatus.iTerm", "", 2},
	{engine->outputChannels.idleStatus.dTerm, "idleStatus.dTerm", "", 2},
	{engine->outputChannels.idleStatus.output, "idleStatus.output", "", 2},
	{engine->outputChannels.idleStatus.error, "idleStatus.error", "", 2},
	{engine->outputChannels.idleStatus.resetCounter, "idleStatus.resetCounter", "", 0},
	{engine->outputChannels.etbStatus.pTerm, "etbStatus.pTerm", "", 2},
	{engine->outputChannels.etbStatus.iTerm, "etbStatus.iTerm", "", 2},
	{engine->outputChannels.etbStatus.dTerm, "etbStatus.dTerm", "", 2},
	{engine->outputChannels.etbStatus.output, "etbStatus.output", "", 2},
	{engine->outputChannels.etbStatus.error, "etbStatus.error", "", 2},
	{engine->outputChannels.etbStatus.resetCounter, "etbStatus.resetCounter", "", 0},
	{engine->outputChannels.boostStatus.pTerm, "boostStatus.pTerm", "", 2},
	{engine->outputChannels.boostStatus.iTerm, "boostStatus.iTerm", "", 2},
	{engine->outputChannels.boostStatus.dTerm, "boostStatus.dTerm", "", 2},
	{engine->outputChannels.boostStatus.output, "boostStatus.output", "", 2},
	{engine->outputChannels.boostStatus.error, "boostStatus.error", "", 2},
	{engine->outputChannels.boostStatus.resetCounter, "boostStatus.resetCounter", "", 0},
	{engine->outputChannels.wastegateDcStatus.pTerm, "wastegateDcStatus.pTerm", "", 2},
	{engine->outputChannels.wastegateDcStatus.iTerm, "wastegateDcStatus.iTerm", "", 2},
	{engine->outputChannels.wastegateDcStatus.dTerm, "wastegateDcStatus.dTerm", "", 2},
	{engine->outputChannels.wastegateDcStatus.output, "wastegateDcStatus.output", "", 2},
	{engine->outputChannels.wastegateDcStatus.error, "wastegateDcStatus.error", "", 2},
	{engine->outputChannels.wastegateDcStatus.resetCounter, "wastegateDcStatus.resetCounter", "", 0},
	{engine->outputChannels.auxSpeed1, "Aux speed 1", "s", 0},
	{engine->outputChannels.auxSpeed2, "Aux speed 2", "s", 0},
	{engine->outputChannels.ISSValue, "TCU: Input Shaft Speed", "RPM", 0},
	{engine->outputChannels.rawAnalogInput[0], "rawAnalogInput 1", "V", 3},
	{engine->outputChannels.rawAnalogInput[1], "rawAnalogInput 2", "V", 3},
	{engine->outputChannels.rawAnalogInput[2], "rawAnalogInput 3", "V", 3},
	{engine->outputChannels.rawAnalogInput[3], "rawAnalogInput 4", "V", 3},
	{engine->outputChannels.rawAnalogInput[4], "rawAnalogInput 5", "V", 3},
	{engine->outputChannels.rawAnalogInput[5], "rawAnalogInput 6", "V", 3},
	{engine->outputChannels.rawAnalogInput[6], "rawAnalogInput 7", "V", 3},
	{engine->outputChannels.rawAnalogInput[7], "rawAnalogInput 8", "V", 3},
	{engine->outputChannels.gppwmOutput[0], "GPPWM Output 1", "%", 2},
	{engine->outputChannels.gppwmOutput[1], "GPPWM Output 2", "%", 2},
	{engine->outputChannels.gppwmOutput[2], "GPPWM Output 3", "%", 2},
	{engine->outputChannels.gppwmOutput[3], "GPPWM Output 4", "%", 2},
	{engine->outputChannels.gppwmXAxis[0], "gppwmXAxis 1", "", 0},
	{engine->outputChannels.gppwmXAxis[1], "gppwmXAxis 2", "", 0},
	{engine->outputChannels.gppwmXAxis[2], "gppwmXAxis 3", "", 0},
	{engine->outputChannels.gppwmXAxis[3], "gppwmXAxis 4", "", 0},
	{engine->outputChannels.gppwmYAxis[0], "gppwmYAxis 1", "", 1},
	{engine->outputChannels.gppwmYAxis[1], "gppwmYAxis 2", "", 1},
	{engine->outputChannels.gppwmYAxis[2], "gppwmYAxis 3", "", 1},
	{engine->outputChannels.gppwmYAxis[3], "gppwmYAxis 4", "", 1},
	{engine->outputChannels.rawBattery, "rawBattery", "V", 3},
	{engine->outputChannels.ignBlendParameter[0], "ignBlendParameter 1", "", 1},
	{engine->outputChannels.ignBlendParameter[1], "ignBlendParameter 2", "", 1},
	{engine->outputChannels.ignBlendParameter[2], "ignBlendParameter 3", "", 1},
	{engine->outputChannels.ignBlendParameter[3], "ignBlendParameter 4", "", 1},
	{engine->outputChannels.ignBlendBias[0], "ignBlendBias 1", "%", 1},
	{engine->outputChannels.ignBlendBias[1], "ignBlendBias 2", "%", 1},
	{engine->outputChannels.ignBlendBias[2], "ignBlendBias 3", "%", 1},
	{engine->outputChannels.ignBlendBias[3], "ignBlendBias 4", "%", 1},
	{engine->outputChannels.ignBlendOutput[0], "ignBlendOutput 1", "deg", 2},
	{engine->outputChannels.ignBlendOutput[1], "ignBlendOutput 2", "deg", 2},
	{engine->outputChannels.ignBlendOutput[2], "ignBlendOutput 3", "deg", 2},
	{engine->outputChannels.ignBlendOutput[3], "ignBlendOutput 4", "deg", 2},
	{engine->outputChannels.veBlendParameter[0], "veBlendParameter 1", "", 1},
	{engine->outputChannels.veBlendParameter[1], "veBlendParameter 2", "", 1},
	{engine->outputChannels.veBlendParameter[2], "veBlendParameter 3", "", 1},
	{engine->outputChannels.veBlendParameter[3], "veBlendParameter 4", "", 1},
	{engine->outputChannels.veBlendBias[0], "veBlendBias 1", "%", 1},
	{engine->outputChannels.veBlendBias[1], "veBlendBias 2", "%", 1},
	{engine->outputChannels.veBlendBias[2], "veBlendBias 3", "%", 1},
	{engine->outputChannels.veBlendBias[3], "veBlendBias 4", "%", 1},
	{engine->outputChannels.veBlendOutput[0], "veBlendOutput 1", "%", 2},
	{engine->outputChannels.veBlendOutput[1], "veBlendOutput 2", "%", 2},
	{engine->outputChannels.veBlendOutput[2], "veBlendOutput 3", "%", 2},
	{engine->outputChannels.veBlendOutput[3], "veBlendOutput 4", "%", 2},
	{engine->outputChannels.boostOpenLoopBlendParameter[0], "boostOpenLoopBlendParameter 1", "", 1},
	{engine->outputChannels.boostOpenLoopBlendParameter[1], "boostOpenLoopBlendParameter 2", "", 1},
	{engine->outputChannels.boostOpenLoopBlendBias[0], "boostOpenLoopBlendBias 1", "%", 1},
	{engine->outputChannels.boostOpenLoopBlendBias[1], "boostOpenLoopBlendBias 2", "%", 1},
	{engine->outputChannels.boostOpenLoopBlendOutput[0], "boostOpenLoopBlendOutput 1", "%", 0},
	{engine->outputChannels.boostOpenLoopBlendOutput[1], "boostOpenLoopBlendOutput 2", "%", 0},
	{engine->outputChannels.boostClosedLoopBlendParameter[0], "boostClosedLoopBlendParameter 1", "", 1},
	{engine->outputChannels.boostClosedLoopBlendParameter[1], "boostClosedLoopBlendParameter 2", "", 1},
	{engine->outputChannels.boostClosedLoopBlendBias[0], "boostClosedLoopBlendBias 1", "%", 1},
	{engine->outputChannels.boostClosedLoopBlendBias[1], "boostClosedLoopBlendBias 2", "%", 1},
	{engine->outputChannels.boostClosedLoopBlendOutput[0], "boostClosedLoopBlendOutput 1", "%", 1},
	{engine->outputChannels.boostClosedLoopBlendOutput[1], "boostClosedLoopBlendOutput 2", "%", 1},
	{engine->outputChannels.outputRequestPeriod, "outputRequestPeriod", "", 0},
	{engine->outputChannels.mapFast, "mapFast", "", 0},
	{engine->outputChannels.luaGauges[0], "Lua: Gauge 1", "value", 3},
	{engine->outputChannels.luaGauges[1], "Lua: Gauge 2", "value", 3},
	{engine->outputChannels.luaGauges[2], "Lua: Gauge 3", "value", 3},
	{engine->outputChannels.luaGauges[3], "Lua: Gauge 4", "value", 3},
	{engine->outputChannels.luaGauges[4], "Lua: Gauge 5", "value", 3},
	{engine->outputChannels.luaGauges[5], "Lua: Gauge 6", "value", 3},
	{engine->outputChannels.luaGauges[6], "Lua: Gauge 7", "value", 3},
	{engine->outputChannels.luaGauges[7], "Lua: Gauge 8", "value", 3},
	{engine->outputChannels.rawMaf2, "rawMaf2", "V", 3},
	{engine->outputChannels.mafMeasured2, "MAF #2", "kg/h", 1},
	{engine->outputChannels.schedulingUsedCount, "schedulingUsedCount", "", 0},
	{engine->outputChannels.vehicleSpeedKph, "Vehicle Speed", "kph", 2},
	{engine->outputChannels.Gego, "Gego", "%", 2},
	{engine->outputChannels.testBenchIter, "testBenchIter", "count", 0},
	{engine->outputChannels.oilTemp, "oilTemp", "deg C", 1},
	{engine->outputChannels.fuelTemp, "fuelTemp", "deg C", 1},
	{engine->outputChannels.ambientTemp, "ambientTemp", "deg C", 1},
	{engine->outputChannels.compressorDischargeTemp, "compressorDischargeTemp", "deg C", 1},
	{engine->outputChannels.compressorDischargePressure, "compressorDischargePressure", "kPa", 1},
	{engine->outputChannels.throttleInletPressure, "throttleInletPressure", "kPa", 1},
	{engine->outputChannels.ignitionOnTime, "ignitionOnTime", "sec", 1},
	{engine->outputChannels.engineRunTime, "engineRunTime", "sec", 1},
	{engine->outputChannels.distanceTraveled, "distanceTraveled", "km", 1},
	{engine->outputChannels.afrGasolineScale, "Air/Fuel Ratio (Gas Scale)", "AFR", 2},
	{engine->outputChannels.afr2GasolineScale, "Air/Fuel Ratio 2 (Gas Scale)", "AFR", 2},
	{engine->outputChannels.wheelSlipRatio, "wheelSlipRatio", "", 0},
	{engine->outputChannels.ignitorDiagnostic[0], "ignitorDiagnostic 1", "", 0},
	{engine->outputChannels.ignitorDiagnostic[1], "ignitorDiagnostic 2", "", 0},
	{engine->outputChannels.ignitorDiagnostic[2], "ignitorDiagnostic 3", "", 0},
	{engine->outputChannels.ignitorDiagnostic[3], "ignitorDiagnostic 4", "", 0},
	{engine->outputChannels.ignitorDiagnostic[4], "ignitorDiagnostic 5", "", 0},
	{engine->outputChannels.ignitorDiagnostic[5], "ignitorDiagnostic 6", "", 0},
	{engine->outputChannels.ignitorDiagnostic[6], "ignitorDiagnostic 7", "", 0},
	{engine->outputChannels.ignitorDiagnostic[7], "ignitorDiagnostic 8", "", 0},
	{engine->outputChannels.ignitorDiagnostic[8], "ignitorDiagnostic 9", "", 0},
	{engine->outputChannels.ignitorDiagnostic[9], "ignitorDiagnostic 10", "", 0},
	{engine->outputChannels.ignitorDiagnostic[10], "ignitorDiagnostic 11", "", 0},
	{engine->outputChannels.ignitorDiagnostic[11], "ignitorDiagnostic 12", "", 0},
	{engine->outputChannels.injectorDiagnostic[0], "injectorDiagnostic 1", "", 0},
	{engine->outputChannels.injectorDiagnostic[1], "injectorDiagnostic 2", "", 0},
	{engine->outputChannels.injectorDiagnostic[2], "injectorDiagnostic 3", "", 0},
	{engine->outputChannels.injectorDiagnostic[3], "injectorDiagnostic 4", "", 0},
	{engine->outputChannels.injectorDiagnostic[4], "injectorDiagnostic 5", "", 0},
	{engine->outputChannels.injectorDiagnostic[5], "injectorDiagnostic 6", "", 0},
	{engine->outputChannels.injectorDiagnostic[6], "injectorDiagnostic 7", "", 0},
	{engine->outputChannels.injectorDiagnostic[7], "injectorDiagnostic 8", "", 0},
	{engine->outputChannels.injectorDiagnostic[8], "injectorDiagnostic 9", "", 0},
	{engine->outputChannels.injectorDiagnostic[9], "injectorDiagnostic 10", "", 0},
	{engine->outputChannels.injectorDiagnostic[10], "injectorDiagnostic 11", "", 0},
	{engine->outputChannels.injectorDiagnostic[11], "injectorDiagnostic 12", "", 0},
	{engine->outputChannels.actualLastInjectionStage2, "Fuel: Last inj pulse width stg 2", "ms", 3},
	{engine->outputChannels.injectorDutyCycleStage2, "Fuel: injector duty cycle stage 2", "%", 0},
	{engine->outputChannels.adc13bitCounter, "adc13bitCounter", "", 0},
	{engine->outputChannels.fastAdcErrorsCount, "fastAdcErrorsCount", "", 0},
	{engine->outputChannels.unexpectedAdcSample, "unexpectedAdcSample", "", 0},
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.totalFuelCorrection, "Fuel: Total correction", "mult", 2, "Fuel: math"},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.postCrankingFuelCorrection, "Fuel: Post cranking mult", "", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.intakeTemperatureCoefficient, "Fuel: IAT correction", "", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.coolantTemperatureCoefficient, "Fuel: CLT correction", "", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.timeSinceCrankingInSecs, "running.timeSinceCrankingInSecs", "secs", 0, "Fuel: math"},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.baseFuel, "Fuel: base cycle mass", "mg", 3},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.running.fuel, "Fuel: running", "mg", 3},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.afrTableYAxis, "afrTableYAxis", "%", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.targetLambda, "Fuel: target lambda", "", 2, "Fuel: math"},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.targetAFR, "Fuel: target AFR", "ratio", 2, "Fuel: math"},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.stoichiometricRatio, "Fuel: Stoich ratio", "ratio", 2},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.sdTcharge_coff, "sdTcharge_coff", "", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.sdAirMassInOneCylinder, "Air: Cylinder airmass", "g", 3},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.normalizedCylinderFilling, "Air: Normalized cyl filling", "%", 1},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.brokenInjector, "brokenInjector", "", 0},
#endif
#if EFI_ENGINE_CONTROL
	{engine->fuelComputer.idealEngineTorque, "idealEngineTorque", "", 0},
#endif
	{engine->ignitionState.baseDwell, "Ignition: base dwell", "ms", 1, "Timing"},
	{engine->ignitionState.sparkDwell, "Ignition: coil charge time", "ms", 1, "Timing"},
	{engine->ignitionState.dwellDurationAngle, "Ignition: dwell duration", "deg", 1, "Timing"},
	{engine->ignitionState.cltTimingCorrection, "Ign: CLT correction", "deg", 2, "Timing"},
	{engine->ignitionState.timingIatCorrection, "Ign: IAT correction", "deg", 2, "Timing"},
	{engine->ignitionState.timingPidCorrection, "Idle: Timing adjustment", "deg", 2, "Timing"},
	{engine->ignitionState.baseIgnitionAdvance, "Ignition: Timing", "deg", 1},
	{engine->ignitionState.correctedIgnitionAdvance, "Timing: ignition", "deg", 1},
	{engine->ignitionState.tractionAdvanceDrop, "Traction: timing correction", "deg", 1},
	{engine->ignitionState.dwellVoltageCorrection, "Ign: Dwell voltage correction", "", 0},
	{engine->ignitionState.luaTimingAdd, "Ign: Lua timing add", "deg", 2, "Timing"},
	{engine->ignitionState.luaTimingMult, "Ign: Lua timing mult", "deg", 2, "Timing"},
	{___engine.module<KnockController>()->m_knockLevel, "Knock: Current level", "Volts", 2},
	{___engine.module<KnockController>()->m_knockCyl[0], "Knock: Cyl 1", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[1], "Knock: Cyl 2", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[2], "Knock: Cyl 3", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[3], "Knock: Cyl 4", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[4], "Knock: Cyl 5", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[5], "Knock: Cyl 6", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[6], "Knock: Cyl 7", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[7], "Knock: Cyl 8", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[8], "Knock: Cyl 9", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[9], "Knock: Cyl 10", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[10], "Knock: Cyl 11", "dBv", 0},
	{___engine.module<KnockController>()->m_knockCyl[11], "Knock: Cyl 12", "dBv", 0},
	{___engine.module<KnockController>()->m_knockRetard, "Knock: Retard", "deg", 1},
	{___engine.module<KnockController>()->m_knockThreshold, "Knock: Threshold", "", 0},
	{___engine.module<KnockController>()->m_knockCount, "Knock: Count", "", 0},
	{___engine.module<KnockController>()->m_maximumRetard, "Knock: Max retard", "", 0},
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<InjectorModelPrimary>()->m_deadtime, "Fuel: injector lag", "ms", 3},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<InjectorModelPrimary>()->pressureDelta, "Fuel: Injector pressure delta", "kPa", 1},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<InjectorModelPrimary>()->pressureRatio, "Fuel: Injector pressure ratio", "", 3},
#endif
#if EFI_LAUNCH_CONTROL
	{engine->launchController.retardThresholdRpm, "retardThresholdRpm", "", 0},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->luaTargetAdd, "Boost: Lua target add", "v", 1, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->boostOutput, "Boost: Output", "percent", 2, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->luaTargetMult, "Boost: Lua target mult", "v", 1, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->openLoopPart, "Boost: Open loop", "v", 1, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->luaOpenLoopAdd, "Boost: Lua open loop add", "v", 1, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->boostControllerClosedLoopPart, "Boost: Closed loop", "%", 1, "Boost Control"},
#endif
#if EFI_BOOST_CONTROL
	{___engine.module<BoostController>()->boostControlTarget, "Boost: Target", "kPa", 1, "Boost Control"},
#endif
	{___engine.module<FanControl1>()->tempAlive, "tempAlive", "", 0},
	{___engine.module<FanControl1>()->tempCode, "tempCode", "", 0},
	{engine->engineState.lua.fuelAdd, "Lua: Fuel add", "g", 3},
	{engine->engineState.lua.fuelMult, "Lua: Fuel mult", "", 0},
	{engine->engineState.sd.tCharge, "Air: Charge temperature estimate", "deg C", 1},
	{engine->engineState.sd.tChargeK, "Air: Charge temperature estimate K", "", 0},
	{engine->engineState.crankingFuel.coolantTemperatureCoefficient, "Fuel: cranking CLT mult", "", 0},
	{engine->engineState.crankingFuel.tpsCoefficient, "Fuel: cranking TPS mult", "", 0},
	{engine->engineState.crankingFuel.durationCoefficient, "Fuel: cranking duration mult", "", 0},
	{engine->engineState.crankingFuel.fuel, "Fuel: Cranking cycle mass", "mg", 2},
	{engine->engineState.baroCorrection, "Fuel: Barometric pressure mult", "", 0},
	{engine->engineState.hellenBoardId, "Detected Board ID", "id", 0},
	{engine->engineState.clutchUpState, "Clutch: up", "", 0},
	{engine->engineState.clutchDownState, "Clutch: down", "", 0},
	{engine->engineState.brakePedalState, "Brake switch", "", 0},
	{engine->engineState.startStopState, "startStopState", "", 0},
	{engine->engineState.smartChipState, "smartChipState", "", 0},
	{engine->engineState.smartChipRestartCounter, "smartChipRestartCounter", "", 0},
	{engine->engineState.smartChipAliveCounter, "smartChipAliveCounter", "", 0},
	{engine->engineState.startStopStateToggleCounter, "startStopStateToggleCounter", "", 0},
	{engine->engineState.luaSoftSparkSkip, "luaSoftSparkSkip", "", 0},
	{engine->engineState.luaHardSparkSkip, "luaHardSparkSkip", "", 0},
	{engine->engineState.tractionControlSparkSkip, "tractionControlSparkSkip", "", 0},
	{engine->engineState.desiredRpmLimit, "User-defined RPM hard limit", "rpm", 0},
	{engine->engineState.fuelInjectionCounter, "Fuel: Injection counter", "", 0},
	{engine->engineState.globalSparkCounter, "Ign: Spark counter", "", 0},
	{engine->engineState.fuelingLoad, "Fuel: Load", "", 0},
	{engine->engineState.ignitionLoad, "Ignition: load", "", 0},
	{engine->engineState.veTableYAxis, "veTableYAxis", "%", 0},
	{engine->engineState.overDwellCounter, "overDwellCounter", "", 0},
	{engine->engineState.overDwellNotScheduledCounter, "overDwellNotScheduledCounter", "", 0},
	{engine->engineState.sparkOutOfOrderCounter, "sparkOutOfOrderCounter", "", 0},
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[0], "Hardware events since boot 1", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[1], "Hardware events since boot 2", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[2], "Hardware events since boot 3", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[3], "Hardware events since boot 4", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[4], "Hardware events since boot 5", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.hwEventCounters[5], "Hardware events since boot 6", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.vvtCamCounter, "Sync: total cams front counter", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.mapVvt_MAP_AT_SPECIAL_POINT, "mapVvt_MAP_AT_SPECIAL_POINT", "", 3},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.mapVvt_MAP_AT_DIFF, "mapVvt_MAP_AT_DIFF", "", 3},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.mapVvt_MAP_AT_CYCLE_COUNT, "mapVvt_MAP_AT_CYCLE_COUNT", "", 3},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.mapVvt_map_peak, "mapVvt_map_peak", "", 3},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.currentEngineDecodedPhase, "Sync: Engine Phase", "deg", 0, "Sync"},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.triggerToothAngleError, "Sync: trigger angle error", "deg", 2},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.triggerIgnoredToothCount, "triggerIgnoredToothCount", "", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{engine->triggerCentral.mapCamPrevToothAngle, "Sync: MAP: prev angle", "deg", 2},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().currentIdlePosition, "Idle: Position", "%", 1},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().baseIdlePosition, "idle: base value", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().idleClosedLoop, "Idle: Closed loop", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().iacByTpsTaper, "idle: iacByTpsTaper portion", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().throttlePedalUpState, "idle: throttlePedalUpState", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().idleTarget, "Idle: Target RPM", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().targetRpmByClt, "Idle: Target RPM base", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().targetRpmAc, "Idle: Target A/C RPM", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().iacByRpmTaper, "idle: iacByRpmTaper portion", "", 0},
#endif
#if EFI_PROD_CODE && EFI_IDLE_CONTROL
	{___engine.module<IdleController>().unmock().luaAdd, "idle: Lua Adder", "", 0},
#endif
	{engine->dc_motors.dcOutput0, "DC: output0", "per", 2, "ETB more"},
	{engine->dc_motors.isEnabled0_int, "DC: en0", "per", 2, "ETB more"},
#if EFI_VVT_PID
	{___engine.module<VvtController1>()->vvtTarget, "vvtTarget", "deg", 1},
#endif
#if EFI_VVT_PID
	{___engine.module<VvtController1>()->vvtOutput, "vvtOutput", "%", 0},
#endif
#if EFI_SHAFT_POSITION_INPUT
	{___engine.lambdaMonitor.lambdaTimeSinceGood, "lambdaTimeSinceGood", "sec", 2},
#endif
};
