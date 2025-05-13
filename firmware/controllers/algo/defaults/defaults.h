/**
 * file defaults.h
 */

#pragma once

void setLeftRightBanksNeedBetterName();
void setDefaultBaseEngine();
void setDefaultFuel();
void setDefaultIgnition();
void setDefaultIdle();
void setDefaultSpark();
void setDefaultWallWettingAdaptation();
void setDefaultVeTable();
void setDefaultEngineParameters();
bool isEngineOptionAvailable(engine_type_e type);
void setTimingMode(engine_type_e type);
void setStartupFuelConfiguration();
void prepareOutputSignals();
void setDefaultVrThresholds();
void prepareVvtActuators(int phaseCount);
void setDefaultCranking();
void setDefaultIacTable();
void setDefaultFan();
void setDefaultETB();
void setDefaultHpfpParameters();
void setDefaultParametersChryslerNak();
void setDefaultBoardParameters();
void setDefaultBatteryCorrection();
void setPPSInputs(adc_channel_e channel1, adc_channel_e channel2);
void setDefaultCranking();
void setGDIFueling();
void setHpfpLobeProfileAngle(int lobes);
void setGdiWallWetting();
void setInline4();
void setDynoDefaults();
void setupTLE9201(Gpio controlPin, Gpio direction, Gpio disable, int dcIndex = 0);
void setupTLE9201IncludingStepper(Gpio controlPin, Gpio direction, Gpio disable, int dcIndex = 0);

#define DEFAULT_CRANKING_DWELL_MS 6
#define DEFAULT_CRANKING_ANGLE 6
#define HpfpLobeProfile_SIZE 16

	// most of rusEFI installations are still port injected, for GDI see 'setGDIFueling'
	// -400 will close the injector just before TDC at the end of the exhaust stroke,
	// around the time the intake valve opens.
#define PORT_INJECTION_OFFSET -400.0f

void setPPSCalibration(float primaryUp, float primaryDown, float secondaryUp, float secondaryDown);

void setTPS1Inputs(adc_channel_e tps1, adc_channel_e tps2);
// legacy since channel TPS still exists
void setTPS1Calibration(uint16_t tpsMin, uint16_t tpsMax);
// Most ETB require redundant dual channel TPS
void setTPS1Calibration(uint16_t tpsMin, uint16_t tpsMax, uint16_t tps1SecondaryMin, uint16_t tps1SecondaryMax);

void setCustomMap(float lowValue, float mapLowValueVoltage, float highValue, float mapHighValueVoltage);

void setEtbPID(float p, float i, float d);
