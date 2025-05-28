// this section was generated automatically by rusEFI tool config_definition-all.jar based on gen_config.sh integration/evotech_config.txt
// by class com.rusefi.output.CHeaderConsumer
// begin
#pragma once
#include "rusefi_types.h"
// start of stft_cell_cfg_s
struct stft_cell_cfg_s {
	/**
	 * Porcentagem maxima que o trim de combustivel de curto prazo pode adicionar
	 * units: %
	 * offset 0
	 */
	int8_t maxAdd;
	/**
	 * Porcentagem maxima que o trim de combustivel de curto prazo pode remover
	 * units: %
	 * offset 1
	 */
	int8_t maxRemove;
	/**
	 * Comumente referenciado como ganho integral.
	 * Constante de tempo para correcao nesta celula: define a responsividade da correcao em malha fechada. Um valor de 5.0 significa que tentara fazer a maior parte da correcao em 5 segundos, e um valor de 1.0 tentara corrigir em 1 segundo.
	 * Valores menores tornam a correcao mais sensivel, valores maiores desaceleram a correcao.
	 * units: sec
	 * offset 2
	 */
	scaled_channel<uint16_t, 10, 1> timeConstant;
};
static_assert(sizeof(stft_cell_cfg_s) == 4);

// start of stft_s
struct stft_s {
	/**
	 * Abaixo desta RPM, a regiao de marcha lenta esta ativa, marcha lenta+300 seria um bom valor
	 * units: RPM
	 * offset 0
	 */
	scaled_channel<uint8_t, 1, 50> maxIdleRegionRpm;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1
	 */
	uint8_t alignmentFill_at_1[1] = {};
	/**
	 * Abaixo desta carga do motor, a regiao de desaceleracao esta ativa
	 * Quando ajustando por MAP as unidades sao kPa, ex. 30 significaria 30kPa. Quando ajustando TPS, 30 seria 30%
	 * units: load
	 * offset 2
	 */
	uint16_t maxOverrunLoad;
	/**
	 * Acima desta carga do motor, a regiao de potencia esta ativa
	 * Quando ajustando por MAP as unidades sao kPa
	 * units: load
	 * offset 4
	 */
	uint16_t minPowerLoad;
	/**
	 * Quando proximo do AFR correto, pausar correcao. Isso pode melhorar a estabilidade nao alterando o ajuste se o erro for extremamente pequeno, mas nao e obrigatorio.
	 * units: %
	 * offset 6
	 */
	scaled_channel<uint8_t, 10, 1> deadband;
	/**
	 * Abaixo desta temperatura, a correcao e desabilitada.
	 * units: C
	 * offset 7
	 */
	int8_t minClt;
	/**
	 * Abaixo deste AFR, a correcao e pausada
	 * units: afr
	 * offset 8
	 */
	scaled_channel<uint8_t, 10, 1> minAfr;
	/**
	 * Acima deste AFR, a correcao e pausada
	 * units: afr
	 * offset 9
	 */
	scaled_channel<uint8_t, 10, 1> maxAfr;
	/**
	 * Atraso apos ligar o motor antes de iniciar a correcao em malha fechada.
	 * units: seconds
	 * offset 10
	 */
	uint8_t startupDelay;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 11
	 */
	uint8_t alignmentFill_at_11[1] = {};
	/**
	 * offset 12
	 */
	stft_cell_cfg_s cellCfgs[STFT_CELL_COUNT] = {};
};
static_assert(sizeof(stft_s) == 28);

// start of pid_s
struct pid_s {
	/**
	 * offset 0
	 */
	float pFactor;
	/**
	 * offset 4
	 */
	float iFactor;
	/**
	 * offset 8
	 */
	float dFactor;
	/**
	 * Adicao linear a logica PID
	 * offset 12
	 */
	int16_t offset;
	/**
	 * Tempo PID dTime
	 * units: ms
	 * offset 14
	 */
	int16_t periodMs;
	/**
	 * Ciclo de Trabalho Minimo de Saida
	 * offset 16
	 */
	int16_t minValue;
	/**
	 * Ciclo de Trabalho Maximo de Saida
	 * offset 18
	 */
	int16_t maxValue;
};
static_assert(sizeof(pid_s) == 20);

// start of MsIoBox_config_s
struct MsIoBox_config_s {
	/**
	 * offset 0
	 */
	MsIoBoxId id;
	/**
	 * offset 1
	 */
	MsIoBoxVss vss;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2
	 */
	uint8_t alignmentFill_at_2[2] = {};
};
static_assert(sizeof(MsIoBox_config_s) == 4);

// start of cranking_parameters_s
struct cranking_parameters_s {
	/**
	 * Massa base do combustivel por cilindro injetado durante a partida. Isso e modificado pelos multiplicadores para CLT, IAT, TPS etc, para dar a largura final do pulso de partida.
	 * Um ponto de partida razoavel e 60mg por litro por cilindro.
	 * ex: 2 litros 4 cil = 500cc/cil, entao 30mg combustivel de partida.
	 * units: mg
	 * offset 0
	 */
	float baseFuel;
	/**
	 * Isso define o limite de RPM abaixo do qual a ECU usara combustivel e logica de ignicao de partida, tipicamente isso e cerca de 350-450rpm. 
	 * set cranking_rpm X
	 * units: RPM
	 * offset 4
	 */
	int16_t rpm;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 6
	 */
	uint8_t alignmentFill_at_6[2] = {};
};
static_assert(sizeof(cranking_parameters_s) == 8);

// start of gppwm_channel
struct gppwm_channel {
	/**
	 * Selecione um pino para usar para saida PWM ou liga-desliga.
	 * offset 0
	 */
	output_pin_e pin;
	/**
	 * Se um erro (com um sensor, etc) for detectado, este valor e usado em vez de ler da tabela.
	 * Este deve ser um valor seguro para qualquer hardware conectado para prevenir danos.
	 * units: %
	 * offset 2
	 */
	uint8_t dutyIfError;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3
	 */
	uint8_t alignmentFill_at_3[1] = {};
	/**
	 * Selecione uma frequencia para executar PWM.
	 * Defina como 0hz para habilitar modo liga-desliga.
	 * units: hz
	 * offset 4
	 */
	uint16_t pwmFrequency;
	/**
	 * Histerese: no modo liga-desliga, ligar a saida quando o valor da tabela estiver acima deste ciclo de trabalho.
	 * units: %
	 * offset 6
	 */
	uint8_t onAboveDuty;
	/**
	 * Histerese: no modo liga-desliga, desligar a saida quando o valor da tabela estiver abaixo deste ciclo de trabalho.
	 * units: %
	 * offset 7
	 */
	uint8_t offBelowDuty;
	/**
	 * Seleciona o eixo Y para usar na tabela.
	 * offset 8
	 */
	gppwm_channel_e loadAxis;
	/**
	 * Seleciona o eixo X para usar na tabela.
	 * offset 9
	 */
	gppwm_channel_e rpmAxis;
	/**
	 * offset 10
	 */
	scaled_channel<int16_t, 2, 1> loadBins[GPPWM_LOAD_COUNT] = {};
	/**
	 * offset 26
	 */
	int16_t rpmBins[GPPWM_RPM_COUNT] = {};
	/**
	 * units: duty
	 * offset 42
	 */
	scaled_channel<uint8_t, 2, 1> table[GPPWM_LOAD_COUNT][GPPWM_RPM_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 106
	 */
	uint8_t alignmentFill_at_106[2] = {};
};
static_assert(sizeof(gppwm_channel) == 108);

// start of air_pressure_sensor_config_s
struct air_pressure_sensor_config_s {
	/**
	 * Valor kPa em voltagem baixa
	 * units: kpa
	 * offset 0
	 */
	float lowValue;
	/**
	 * Valor kPa em voltagem alta
	 * units: kpa
	 * offset 4
	 */
	float highValue;
	/**
	 * offset 8
	 */
	air_pressure_sensor_type_e type;
	/**
	 * offset 9
	 */
	adc_channel_e hwChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 10
	 */
	uint8_t alignmentFill_at_10[2] = {};
};
static_assert(sizeof(air_pressure_sensor_config_s) == 12);

// start of MAP_sensor_config_s
struct MAP_sensor_config_s {
	/**
	 * offset 0
	 */
	float samplingAngleBins[MAP_ANGLE_SIZE] = {};
	/**
	 * Angulo em graus do virabrequim de inicio de amostragem de media MAP
	 * units: deg
	 * offset 32
	 */
	float samplingAngle[MAP_ANGLE_SIZE] = {};
	/**
	 * offset 64
	 */
	float samplingWindowBins[MAP_WINDOW_SIZE] = {};
	/**
	 * Duracao do angulo em graus do virabrequim para media MAP
	 * units: deg
	 * offset 96
	 */
	float samplingWindow[MAP_WINDOW_SIZE] = {};
	/**
	 * offset 128
	 */
	air_pressure_sensor_config_s sensor;
};
static_assert(sizeof(MAP_sensor_config_s) == 140);

/**
 * @brief Thermistor known values

*/
// start of thermistor_conf_s
struct thermistor_conf_s {
	/**
	 * estes valores estao em Celsius
	 * units: *C
	 * offset 0
	 */
	float tempC_1;
	/**
	 * units: *C
	 * offset 4
	 */
	float tempC_2;
	/**
	 * units: *C
	 * offset 8
	 */
	float tempC_3;
	/**
	 * units: Ohm
	 * offset 12
	 */
	float resistance_1;
	/**
	 * units: Ohm
	 * offset 16
	 */
	float resistance_2;
	/**
	 * units: Ohm
	 * offset 20
	 */
	float resistance_3;
	/**
	 * Valor do resistor pull-up em sua placa
	 * units: Ohm
	 * offset 24
	 */
	float bias_resistor;
};
static_assert(sizeof(thermistor_conf_s) == 28);

// start of linear_sensor_s
struct linear_sensor_s {
	/**
	 * offset 0
	 */
	adc_channel_e hwChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1
	 */
	uint8_t alignmentFill_at_1[3] = {};
	/**
	 * units: volts
	 * offset 4
	 */
	float v1;
	/**
	 * offset 8
	 */
	float value1;
	/**
	 * units: volts
	 * offset 12
	 */
	float v2;
	/**
	 * offset 16
	 */
	float value2;
};
static_assert(sizeof(linear_sensor_s) == 20);

// start of ThermistorConf
struct ThermistorConf {
	/**
	 * offset 0
	 */
	thermistor_conf_s config;
	/**
	 * offset 28
	 */
	adc_channel_e adcChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 29
	 */
	uint8_t alignmentFill_at_29[3] = {};
};
static_assert(sizeof(ThermistorConf) == 32);

// start of injector_s
struct injector_s {
	/**
	 * Este e o fluxo do seu injetor na pressao de combustivel usada no veiculo
	 * Veja configuracao de unidades abaixo
	 * offset 0
	 */
	float flow;
	/**
	 * units: volts
	 * offset 4
	 */
	scaled_channel<int16_t, 100, 1> battLagCorrBattBins[VBAT_INJECTOR_CURVE_SIZE] = {};
	/**
	 * Pressao de correcao do injetor
	 * units: kPa
	 * offset 20
	 */
	scaled_channel<uint32_t, 10, 1> battLagCorrPressBins[VBAT_INJECTOR_CURVE_PRESSURE_SIZE] = {};
	/**
	 * Atraso em ms entre os tempos mortos de abertura e fechamento do injetor
	 * units: ms
	 * offset 28
	 */
	scaled_channel<int16_t, 100, 1> battLagCorrTable[VBAT_INJECTOR_CURVE_PRESSURE_SIZE][VBAT_INJECTOR_CURVE_SIZE] = {};
};
static_assert(sizeof(injector_s) == 60);

// start of trigger_config_s
struct trigger_config_s {
	/**
	 * https://github.com/rusefi/rusefi/wiki/All-Supported-Triggers
	 * offset 0
	 */
	trigger_type_e type;
	/**
	 * units: number
	 * offset 4
	 */
	int customTotalToothCount;
	/**
	 * units: number
	 * offset 8
	 */
	int customSkippedToothCount;
};
static_assert(sizeof(trigger_config_s) == 12);

// start of afr_sensor_s
struct afr_sensor_s {
	/**
	 * offset 0
	 */
	adc_channel_e hwChannel;
	/**
	 * offset 1
	 */
	adc_channel_e hwChannel2;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2
	 */
	uint8_t alignmentFill_at_2[2] = {};
	/**
	 * units: volts
	 * offset 4
	 */
	float v1;
	/**
	 * units: AFR
	 * offset 8
	 */
	float value1;
	/**
	 * units: volts
	 * offset 12
	 */
	float v2;
	/**
	 * units: AFR
	 * offset 16
	 */
	float value2;
};
static_assert(sizeof(afr_sensor_s) == 20);

// start of idle_hardware_s
struct idle_hardware_s {
	/**
	 * units: Hz
	 * offset 0
	 */
	int solenoidFrequency;
	/**
	 * offset 4
	 */
	output_pin_e solenoidPin;
	/**
	 * offset 6
	 */
	Gpio stepperDirectionPin;
	/**
	 * offset 8
	 */
	Gpio stepperStepPin;
	/**
	 * offset 10
	 */
	pin_output_mode_e solenoidPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 11
	 */
	uint8_t alignmentFill_at_11[1] = {};
};
static_assert(sizeof(idle_hardware_s) == 12);

// start of dc_io
struct dc_io {
	/**
	 * offset 0
	 */
	Gpio directionPin1;
	/**
	 * offset 2
	 */
	Gpio directionPin2;
	/**
	 * Acts as EN pin in two-wire mode
	 * offset 4
	 */
	Gpio controlPin;
	/**
	 * offset 6
	 */
	Gpio disablePin;
};
static_assert(sizeof(dc_io) == 8);

// start of vr_threshold_s
struct vr_threshold_s {
	/**
	 * units: rpm
	 * offset 0
	 */
	scaled_channel<uint8_t, 1, 50> rpmBins[6] = {};
	/**
	 * units: volts
	 * offset 6
	 */
	scaled_channel<uint8_t, 100, 1> values[6] = {};
	/**
	 * offset 12
	 */
	Gpio pin;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 14
	 */
	uint8_t alignmentFill_at_14[2] = {};
};
static_assert(sizeof(vr_threshold_s) == 16);

// start of engine_configuration_s
struct engine_configuration_s {
	/**
	 * http://rusefi.com/wiki/index.php?title=Manual:Engine_Type
	 * set engine_type X
	 * offset 0
	 */
	engine_type_e engineType;
	/**
	 * offset 2
	 */
	uint16_t startButtonSuppressOnStartUpMs;
	/**
	 * Desabilitar sensor sniffer acima desta rpm
	 * units: RPM
	 * offset 4
	 */
	uint16_t sensorSnifferRpmThreshold;
	/**
	 * Um limitador de giros secundario acionado pelo piloto para ajudar o veiculo a acelerar mais rapido
	 * units: rpm
	 * offset 6
	 */
	uint16_t launchRpm;
	/**
	 * set rpm_hard_limit X
	 * units: rpm
	 * offset 8
	 */
	uint16_t rpmHardLimit;
	/**
	 * Engine sniffer seria desabilitado acima desta rpm
	 * set engineSnifferRpmThreshold X
	 * units: RPM
	 * offset 10
	 */
	uint16_t engineSnifferRpmThreshold;
	/**
	 * Desabilitar multispark acima desta velocidade do motor.
	 * units: rpm
	 * offset 12
	 */
	scaled_channel<uint8_t, 1, 50> multisparkMaxRpm;
	/**
	 * Acima desta RPM, desabilitar ar condicionado. Configure 0 para desabilitar verificacao.
	 * units: rpm
	 * offset 13
	 */
	scaled_channel<uint8_t, 1, 50> maxAcRpm;
	/**
	 * Acima deste TPS, desabilitar ar condicionado. Configure 0 para desabilitar verificacao.
	 * units: %
	 * offset 14
	 */
	uint8_t maxAcTps;
	/**
	 * Acima desta CLT, desabilitar ar condicionado para prevenir superaquecimento do motor. Configure 0 para desabilitar verificacao.
	 * units: deg C
	 * offset 15
	 */
	uint8_t maxAcClt;
	/**
	 * Apenas para referencia, nao levado em conta por nenhuma logica neste ponto
	 * units: CR
	 * offset 16
	 */
	float compressionRatio;
	/**
	 * Voltagem quando a wastegate esta fechada.
	 * Voce provavelmente nao tem uma dessas!
	 * units: mv
	 * offset 20
	 */
	uint16_t wastegatePositionMin;
	/**
	 * Voltagem quando a wastegate esta totalmente aberta.
	 * Voce provavelmente nao tem uma dessas!
	 * 1 volt = 1000 unidades
	 * units: mv
	 * offset 22
	 */
	uint16_t wastegatePositionMax;
	/**
	 * Voltagem quando a valvula de marcha lenta esta fechada.
	 * Voce provavelmente nao tem uma dessas!
	 * units: mv
	 * offset 24
	 */
	uint16_t idlePositionMin;
	/**
	 * Voltagem quando a valvula de marcha lenta esta aberta.
	 * Voce provavelmente nao tem uma dessas!
	 * 1 volt = 1000 unidades
	 * units: mv
	 * offset 26
	 */
	uint16_t idlePositionMax;
	/**
	 * Enable LTIT (Long Term Idle Trim) learning
	offset 28 bit 0 */
	bool ltitEnabled : 1 {};
	/**
	offset 28 bit 1 */
	bool unusedBit_16_1 : 1 {};
	/**
	offset 28 bit 2 */
	bool unusedBit_16_2 : 1 {};
	/**
	offset 28 bit 3 */
	bool unusedBit_16_3 : 1 {};
	/**
	offset 28 bit 4 */
	bool unusedBit_16_4 : 1 {};
	/**
	offset 28 bit 5 */
	bool unusedBit_16_5 : 1 {};
	/**
	offset 28 bit 6 */
	bool unusedBit_16_6 : 1 {};
	/**
	offset 28 bit 7 */
	bool unusedBit_16_7 : 1 {};
	/**
	offset 28 bit 8 */
	bool unusedBit_16_8 : 1 {};
	/**
	offset 28 bit 9 */
	bool unusedBit_16_9 : 1 {};
	/**
	offset 28 bit 10 */
	bool unusedBit_16_10 : 1 {};
	/**
	offset 28 bit 11 */
	bool unusedBit_16_11 : 1 {};
	/**
	offset 28 bit 12 */
	bool unusedBit_16_12 : 1 {};
	/**
	offset 28 bit 13 */
	bool unusedBit_16_13 : 1 {};
	/**
	offset 28 bit 14 */
	bool unusedBit_16_14 : 1 {};
	/**
	offset 28 bit 15 */
	bool unusedBit_16_15 : 1 {};
	/**
	offset 28 bit 16 */
	bool unusedBit_16_16 : 1 {};
	/**
	offset 28 bit 17 */
	bool unusedBit_16_17 : 1 {};
	/**
	offset 28 bit 18 */
	bool unusedBit_16_18 : 1 {};
	/**
	offset 28 bit 19 */
	bool unusedBit_16_19 : 1 {};
	/**
	offset 28 bit 20 */
	bool unusedBit_16_20 : 1 {};
	/**
	offset 28 bit 21 */
	bool unusedBit_16_21 : 1 {};
	/**
	offset 28 bit 22 */
	bool unusedBit_16_22 : 1 {};
	/**
	offset 28 bit 23 */
	bool unusedBit_16_23 : 1 {};
	/**
	offset 28 bit 24 */
	bool unusedBit_16_24 : 1 {};
	/**
	offset 28 bit 25 */
	bool unusedBit_16_25 : 1 {};
	/**
	offset 28 bit 26 */
	bool unusedBit_16_26 : 1 {};
	/**
	offset 28 bit 27 */
	bool unusedBit_16_27 : 1 {};
	/**
	offset 28 bit 28 */
	bool unusedBit_16_28 : 1 {};
	/**
	offset 28 bit 29 */
	bool unusedBit_16_29 : 1 {};
	/**
	offset 28 bit 30 */
	bool unusedBit_16_30 : 1 {};
	/**
	offset 28 bit 31 */
	bool unusedBit_16_31 : 1 {};
	/**
	 * Constante de filtro EMA para LTIT (0-255)
	 * units: 0-255
	 * offset 32
	 */
	uint8_t ltitEmaAlpha;
	/**
	 * Faixa de RPM para considerar marcha lenta estavel
	 * units: rpm
	 * offset 33
	 */
	uint8_t ltitStableRpmThreshold;
	/**
	 * Tempo minimo de marcha lenta estavel antes do aprendizado
	 * units: s
	 * offset 34
	 */
	uint8_t ltitStableTime;
	/**
	 * Taxa de aprendizado LTIT
	 * units: %/s
	 * offset 35
	 */
	uint8_t ltitCorrectionRate;
	/**
	 * Atraso apos ignicao ligada antes do aprendizado/aplicacao LTIT
	 * units: s
	 * offset 36
	 */
	uint8_t ltitIgnitionOnDelay;
	/**
	 * Atraso apos ignicao desligada antes de salvar LTIT
	 * units: s
	 * offset 37
	 */
	uint8_t ltitIgnitionOffSaveDelay;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 38
	 */
	uint8_t alignmentFill_at_38[2] = {};
	/**
	 * Valor minimo de correcao multiplicativa LTIT
	 * units: %
	 * offset 40
	 */
	float ltitClampMin;
	/**
	 * Valor maximo de correcao multiplicativa LTIT
	 * units: %
	 * offset 44
	 */
	float ltitClampMax;
	/**
	 * Intensidade de suavizacao regional da tabela LTIT (0=sem suavizacao)
	 * units: ratio
	 * offset 48
	 */
	scaled_channel<uint8_t, 100, 1> ltitSmoothingIntensity;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 49
	 */
	uint8_t alignmentFill_at_49[1] = {};
	/**
	 * offset 50
	 */
	output_pin_e mainRelayPin;
	/**
	 * offset 52
	 */
	Gpio sdCardCsPin;
	/**
	 * offset 54
	 */
	Gpio canTxPin;
	/**
	 * offset 56
	 */
	Gpio canRxPin;
	/**
	 * Pin that activates the reduction/cut for shifting. Sometimes shared with the Launch Control pin
	 * offset 58
	 */
	switch_input_pin_e torqueReductionTriggerPin;
	/**
	 * units: %
	 * offset 60
	 */
	int8_t launchFuelAdderPercent;
	/**
	 * Tempo apos o qual o acelerador e considerado travado.
	 * units: sec
	 * offset 61
	 */
	scaled_channel<uint8_t, 50, 1> etbJamTimeout;
	/**
	 * offset 62
	 */
	output_pin_e tachOutputPin;
	/**
	 * offset 64
	 */
	pin_output_mode_e tachOutputPinMode;
	/**
	 * Porcentagem adicional de marcha lenta enquanto ar condicionado estiver ativo
	 * units: %
	 * offset 65
	 */
	uint8_t acIdleExtraOffset;
	/**
	 * Este parametro define o mais tardio que o ultimo multispark pode ocorrer apos o evento principal de ignicao. Por exemplo, se o tempo de ignicao e 30 graus BTDC, e este parametro e definido como 45, nenhum multispark sera disparado apos 15 graus ATDC.
	 * units: deg
	 * offset 66
	 */
	uint8_t multisparkMaxSparkingAngle;
	/**
	 * Configura o numero maximo de fagulhas extras para disparar (nao inclui fagulha principal)
	 * units: count
	 * offset 67
	 */
	uint8_t multisparkMaxExtraSparkCount;
	/**
	 * units: RPM
	 * offset 68
	 */
	int16_t vvtControlMinRpm;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 70
	 */
	uint8_t alignmentFill_at_70[2] = {};
	/**
	 * offset 72
	 */
	injector_s injector;
	/**
	 * offset 132
	 */
	injector_s injectorSecondary;
	/**
	 * Does the vehicle have a turbo or supercharger?
	offset 192 bit 0 */
	bool isForcedInduction : 1 {};
	/**
	 * On some Ford and Toyota vehicles one of the throttle sensors is not linear on the full range, i.e. in the specific range of the positions we effectively have only one sensor.
	offset 192 bit 1 */
	bool useFordRedundantTps : 1 {};
	/**
	offset 192 bit 2 */
	bool enableKline : 1 {};
	/**
	offset 192 bit 3 */
	bool overrideTriggerGaps : 1 {};
	/**
	 * Turn on this fan when AC is on.
	offset 192 bit 4 */
	bool enableFan1WithAc : 1 {};
	/**
	 * Turn on this fan when AC is on.
	offset 192 bit 5 */
	bool enableFan2WithAc : 1 {};
	/**
	 * Inhibit operation of this fan while the engine is not running.
	offset 192 bit 6 */
	bool disableFan1WhenStopped : 1 {};
	/**
	 * Inhibit operation of this fan while the engine is not running.
	offset 192 bit 7 */
	bool disableFan2WhenStopped : 1 {};
	/**
	 * Enable secondary spark outputs that fire after the primary (rotaries, twin plug engines).
	offset 192 bit 8 */
	bool enableTrailingSparks : 1 {};
	/**
	 * TLE7209 and L6205 use two-wire mode. TLE9201 and VNH2SP30 do NOT use two wire mode.
	offset 192 bit 9 */
	bool etb_use_two_wires : 1 {};
	/**
	 * Subaru/BMW style where default valve position is somewhere in the middle. First solenoid opens it more while second can close it more than default position.
	offset 192 bit 10 */
	bool isDoubleSolenoidIdle : 1 {};
	/**
	offset 192 bit 11 */
	bool useEeprom : 1 {};
	/**
	 * Switch between Industrial and Cic PID implementation
	offset 192 bit 12 */
	bool useCicPidForIdle : 1 {};
	/**
	offset 192 bit 13 */
	bool useTLE8888_cranking_hack : 1 {};
	/**
	offset 192 bit 14 */
	bool kickStartCranking : 1 {};
	/**
	 * This uses separate ignition timing and VE tables not only for idle conditions, also during the postcranking-to-idle taper transition (See also afterCrankingIACtaperDuration).
	offset 192 bit 15 */
	bool useSeparateIdleTablesForCrankingTaper : 1 {};
	/**
	offset 192 bit 16 */
	bool launchControlEnabled : 1 {};
	/**
	 * "Detect double trigger edges"
	offset 192 bit 17 */
	bool doNotFilterTriggerEdgeNoise : 1 {};
	/**
	offset 192 bit 18 */
	bool antiLagEnabled : 1 {};
	/**
	 * For cranking either use the specified fixed base fuel mass, or use the normal running math (VE table).
	offset 192 bit 19 */
	bool useRunningMathForCranking : 1 {};
	/**
	 * Shall we display real life signal or just the part consumed by trigger decoder.
	 * Applies to both trigger and cam/vvt input.
	 * 
	 * enable logic_level_trigger
	offset 192 bit 20 */
	bool displayLogicLevelsInEngineSniffer : 1 {};
	/**
	offset 192 bit 21 */
	bool useTLE8888_stepper : 1 {};
	/**
	offset 192 bit 22 */
	bool usescriptTableForCanSniffingFiltering : 1 {};
	/**
	 * Print incoming and outgoing first bus CAN messages in evoTech console
	offset 192 bit 23 */
	bool verboseCan : 1 {};
	/**
	 * Experimental setting that will cause a misfire
	 * DO NOT ENABLE.
	offset 192 bit 24 */
	bool artificialTestMisfire : 1 {};
	/**
	 * On some Ford and Toyota vehicles one of the pedal sensors is not linear on the full range, i.e. in the specific range of the positions we effectively have only one sensor.
	offset 192 bit 25 */
	bool useFordRedundantPps : 1 {};
	/**
	offset 192 bit 26 */
	bool cltSensorPulldown : 1 {};
	/**
	offset 192 bit 27 */
	bool iatSensorPulldown : 1 {};
	/**
	offset 192 bit 28 */
	bool allowIdenticalPps : 1 {};
	/**
	offset 192 bit 29 */
	bool overrideVvtTriggerGaps : 1 {};
	/**
	 * If enabled - use onboard SPI Accelerometer, otherwise listen for CAN messages
	offset 192 bit 30 */
	bool useSpiImu : 1 {};
	/**
	offset 192 bit 31 */
	bool enableStagedInjection : 1 {};
	/**
	 * Theshold to enable fan 1 after ac turned on
	 * units: sec
	 * offset 196
	 */
	uint8_t Fan1AcThresholdOn;
	/**
	 * Theshold to disable fan 1 after ac tuned off
	 * units: sec
	 * offset 197
	 */
	uint8_t Fan1AcThresholdOff;
	/**
	 * Theshold to enable fan 2 after ac turned on
	 * units: sec
	 * offset 198
	 */
	uint8_t Fan2AcThresholdOn;
	/**
	 * Theshold to disable fan 2 after ac tuned off
	 * units: sec
	 * offset 199
	 */
	uint8_t Fan2AcThresholdOff;
	/**
	 * Use idle ignition table while coasting;
	offset 200 bit 0 */
	bool useIdleAdvanceWhileCoasting : 1 {};
	/**
	offset 200 bit 1 */
	bool unusedBit_111_1 : 1 {};
	/**
	offset 200 bit 2 */
	bool unusedBit_111_2 : 1 {};
	/**
	offset 200 bit 3 */
	bool unusedBit_111_3 : 1 {};
	/**
	offset 200 bit 4 */
	bool unusedBit_111_4 : 1 {};
	/**
	offset 200 bit 5 */
	bool unusedBit_111_5 : 1 {};
	/**
	offset 200 bit 6 */
	bool unusedBit_111_6 : 1 {};
	/**
	offset 200 bit 7 */
	bool unusedBit_111_7 : 1 {};
	/**
	offset 200 bit 8 */
	bool unusedBit_111_8 : 1 {};
	/**
	offset 200 bit 9 */
	bool unusedBit_111_9 : 1 {};
	/**
	offset 200 bit 10 */
	bool unusedBit_111_10 : 1 {};
	/**
	offset 200 bit 11 */
	bool unusedBit_111_11 : 1 {};
	/**
	offset 200 bit 12 */
	bool unusedBit_111_12 : 1 {};
	/**
	offset 200 bit 13 */
	bool unusedBit_111_13 : 1 {};
	/**
	offset 200 bit 14 */
	bool unusedBit_111_14 : 1 {};
	/**
	offset 200 bit 15 */
	bool unusedBit_111_15 : 1 {};
	/**
	offset 200 bit 16 */
	bool unusedBit_111_16 : 1 {};
	/**
	offset 200 bit 17 */
	bool unusedBit_111_17 : 1 {};
	/**
	offset 200 bit 18 */
	bool unusedBit_111_18 : 1 {};
	/**
	offset 200 bit 19 */
	bool unusedBit_111_19 : 1 {};
	/**
	offset 200 bit 20 */
	bool unusedBit_111_20 : 1 {};
	/**
	offset 200 bit 21 */
	bool unusedBit_111_21 : 1 {};
	/**
	offset 200 bit 22 */
	bool unusedBit_111_22 : 1 {};
	/**
	offset 200 bit 23 */
	bool unusedBit_111_23 : 1 {};
	/**
	offset 200 bit 24 */
	bool unusedBit_111_24 : 1 {};
	/**
	offset 200 bit 25 */
	bool unusedBit_111_25 : 1 {};
	/**
	offset 200 bit 26 */
	bool unusedBit_111_26 : 1 {};
	/**
	offset 200 bit 27 */
	bool unusedBit_111_27 : 1 {};
	/**
	offset 200 bit 28 */
	bool unusedBit_111_28 : 1 {};
	/**
	offset 200 bit 29 */
	bool unusedBit_111_29 : 1 {};
	/**
	offset 200 bit 30 */
	bool unusedBit_111_30 : 1 {};
	/**
	offset 200 bit 31 */
	bool unusedBit_111_31 : 1 {};
	/**
	 * Closed throttle, 1 volt = 200 units.
	 * See also tps1_1AdcChannel
	 * set tps_min X
	 * units: ADC
	 * offset 204
	 */
	int16_t tpsMin;
	/**
	 * Full throttle.
	 * See also tps1_1AdcChannel
	 * set tps_max X
	 * units: ADC
	 * offset 206
	 */
	int16_t tpsMax;
	/**
	 * TPS error detection: what throttle % is unrealistically low?
	 * Also used for accelerator pedal error detection if so equipped.
	 * units: %
	 * offset 208
	 */
	int16_t tpsErrorDetectionTooLow;
	/**
	 * TPS error detection: what throttle % is unrealistically high?
	 * Also used for accelerator pedal error detection if so equipped.
	 * units: %
	 * offset 210
	 */
	int16_t tpsErrorDetectionTooHigh;
	/**
	 * offset 212
	 */
	cranking_parameters_s cranking;
	/**
	 * Duracao do dwell durante a partida
	 * units: ms
	 * offset 220
	 */
	float ignitionDwellForCrankingMs;
	/**
	 * Uma vez que a velocidade do motor passe este valor, comece a reduzir o angulo ETB.
	 * units: rpm
	 * offset 224
	 */
	uint16_t etbRevLimitStart;
	/**
	 * Esta distancia acima do 'Inicio do limitador suave', feche totalmente a borboleta. Na parte inferior da faixa, o controle da borboleta e normal. Na parte superior da faixa, a borboleta esta totalmente fechada.
	 * units: rpm
	 * offset 226
	 */
	uint16_t etbRevLimitRange;
	/**
	 * @see isMapAveragingEnabled
	 * offset 228
	 */
	MAP_sensor_config_s map;
	/**
	 * todo: merge with channel settings, use full-scale Thermistor here!
	 * offset 368
	 */
	ThermistorConf clt;
	/**
	 * offset 400
	 */
	ThermistorConf iat;
	/**
	 * units: deg
	 * offset 432
	 */
	float launchTimingRetard;
	/**
	 * value '6' for 8MHz hw osc
	 * read hip9011 datasheet for details
	 * todo split into two bit fields
	 * units: integer
	 * offset 436
	 */
	uint8_t hip9011Prescaler;
	/**
	 * offset 437
	 */
	uint8_t unusedHip0;
	/**
	 * offset 438
	 */
	uint8_t unusedHip1;
	/**
	 * Valor zero significa nao detectar ajuste
	 * units: seconds
	 * offset 439
	 */
	uint8_t tuningDetector;
	/**
	 * Valor minimo iTerm
	 * offset 440
	 */
	int16_t alternator_iTermMin;
	/**
	 * Valor maximo iTerm
	 * offset 442
	 */
	int16_t alternator_iTermMax;
	/**
	 * @@DISPLACEMENT_TOOLTIP@@
	 * units: L
	 * offset 444
	 */
	scaled_channel<uint16_t, 1000, 1> displacement;
	/**
	 * units: RPM
	 * offset 446
	 */
	uint16_t triggerSimulatorRpm;
	/**
	 * Number of cylinder the engine has.
	 * offset 448
	 */
	uint32_t cylindersCount;
	/**
	 * offset 452
	 */
	firing_order_e firingOrder;
	/**
	 * offset 453
	 */
	uint8_t justATempTest;
	/**
	 * Delta kPa para sincronizacao MAP
	 * units: kPa
	 * offset 454
	 */
	uint8_t mapSyncThreshold;
	/**
	 * Quantos % dos eventos de ignicao serao cortados
	 * units: %
	 * offset 455
	 */
	int8_t torqueReductionIgnitionCut;
	/**
	 * @@CYLINDER_BORE_TOOLTIP@@
	 * units: mm
	 * offset 456
	 */
	float cylinderBore;
	/**
	 * This setting controls which fuel quantity control algorithm is used.
	 * Alpha-N means drive by TPS commonly only used for NA engines
	 * Speed Density requires MAP sensor and is the default choice for may installs
	 * MAF air charge is a cylinder filling based method that uses a mass air flow sensor.
	 * offset 460
	 */
	engine_load_mode_e fuelAlgorithm;
	/**
	 * units: %
	 * offset 461
	 */
	uint8_t ALSMaxTPS;
	/**
	 * This is the injection strategy during engine start. See Fuel/Injection settings for more detail. It is suggested to use "Simultaneous".
	 * offset 462
	 */
	injection_mode_e crankingInjectionMode;
	/**
	 * This is where the fuel injection type is defined: "Simultaneous" means all injectors will fire together at once. "Sequential" fires the injectors on a per cylinder basis, which requires individually wired injectors. "Batched" will fire the injectors in groups.
	 * offset 463
	 */
	injection_mode_e injectionMode;
	/**
	 * Minimum RPM to enable boost control. Use this to avoid solenoid noise at idle, and help spool in some cases.
	 * offset 464
	 */
	uint16_t boostControlMinRpm;
	/**
	 * Minimum TPS to enable boost control. Use this to avoid solenoid noise at idle, and help spool in some cases.
	 * offset 466
	 */
	uint8_t boostControlMinTps;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 467
	 */
	uint8_t alignmentFill_at_467[1] = {};
	/**
	 * Minimum MAP to enable boost control. Use this to avoid solenoid noise at idle, and help spool in some cases.
	 * offset 468
	 */
	uint16_t boostControlMinMap;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 470
	 */
	uint8_t alignmentFill_at_470[2] = {};
	/**
	 * Ignition advance angle used during engine cranking, 5-10 degrees will work as a base setting for most engines.
	 * There is tapering towards running timing advance
	 * set cranking_timing_angle X
	 * units: deg
	 * offset 472
	 */
	angle_t crankingTimingAngle;
	/**
	 * Single coil = distributor
	 * Individual coils = one coil per cylinder (COP, coil-near-plug), requires sequential mode
	 * Wasted spark = Fires pairs of cylinders together, either one coil per pair of cylinders or one coil per cylinder
	 * Two distributors = A pair of distributors, found on some BMW, Toyota and other engines
	 * set ignition_mode X
	 * offset 476
	 */
	ignition_mode_e ignitionMode;
	/**
	 * Quantas razoes de lacuna consecutivas tem que corresponder aos intervalos esperados para que a sincronizacao aconteca
	 * units: count
	 * offset 477
	 */
	int8_t gapTrackingLengthOverride;
	/**
	 * Acima desta velocidade, desabilitar controle de marcha lenta em malha fechada. Configure 0 para desabilitar (permitir marcha lenta em malha fechada em qualquer velocidade).
	 * units: kph
	 * offset 478
	 */
	uint8_t maxIdleVss;
	/**
	 * offset 479
	 */
	uint8_t camDecoder2jzPrecision;
	/**
	 * Pressao de oleo esperada apos ligar o motor. Se a pressao de oleo nao atingir este nivel dentro de 5 segundos apos a partida do motor, o combustivel sera cortado. Configure 0 para desabilitar e sempre permitir a partida.
	 * units: kPa
	 * offset 480
	 */
	uint16_t minOilPressureAfterStart;
	/**
	 * Dynamic uses the timing map to decide the ignition timing
	 * Static timing fixes the timing to the value set below (only use for checking static timing with a timing light).
	 * offset 482
	 */
	timing_mode_e timingMode;
	/**
	 * offset 483
	 */
	can_nbc_e canNbcType;
	/**
	 * This option enables sandelo eletrohydraulic steering pump to be CAN controller by the ECU;
	offset 484 bit 0 */
	bool enableSanderoPump : 1 {};
	/**
	offset 484 bit 1 */
	bool unusedBit_186_1 : 1 {};
	/**
	offset 484 bit 2 */
	bool unusedBit_186_2 : 1 {};
	/**
	offset 484 bit 3 */
	bool unusedBit_186_3 : 1 {};
	/**
	offset 484 bit 4 */
	bool unusedBit_186_4 : 1 {};
	/**
	offset 484 bit 5 */
	bool unusedBit_186_5 : 1 {};
	/**
	offset 484 bit 6 */
	bool unusedBit_186_6 : 1 {};
	/**
	offset 484 bit 7 */
	bool unusedBit_186_7 : 1 {};
	/**
	offset 484 bit 8 */
	bool unusedBit_186_8 : 1 {};
	/**
	offset 484 bit 9 */
	bool unusedBit_186_9 : 1 {};
	/**
	offset 484 bit 10 */
	bool unusedBit_186_10 : 1 {};
	/**
	offset 484 bit 11 */
	bool unusedBit_186_11 : 1 {};
	/**
	offset 484 bit 12 */
	bool unusedBit_186_12 : 1 {};
	/**
	offset 484 bit 13 */
	bool unusedBit_186_13 : 1 {};
	/**
	offset 484 bit 14 */
	bool unusedBit_186_14 : 1 {};
	/**
	offset 484 bit 15 */
	bool unusedBit_186_15 : 1 {};
	/**
	offset 484 bit 16 */
	bool unusedBit_186_16 : 1 {};
	/**
	offset 484 bit 17 */
	bool unusedBit_186_17 : 1 {};
	/**
	offset 484 bit 18 */
	bool unusedBit_186_18 : 1 {};
	/**
	offset 484 bit 19 */
	bool unusedBit_186_19 : 1 {};
	/**
	offset 484 bit 20 */
	bool unusedBit_186_20 : 1 {};
	/**
	offset 484 bit 21 */
	bool unusedBit_186_21 : 1 {};
	/**
	offset 484 bit 22 */
	bool unusedBit_186_22 : 1 {};
	/**
	offset 484 bit 23 */
	bool unusedBit_186_23 : 1 {};
	/**
	offset 484 bit 24 */
	bool unusedBit_186_24 : 1 {};
	/**
	offset 484 bit 25 */
	bool unusedBit_186_25 : 1 {};
	/**
	offset 484 bit 26 */
	bool unusedBit_186_26 : 1 {};
	/**
	offset 484 bit 27 */
	bool unusedBit_186_27 : 1 {};
	/**
	offset 484 bit 28 */
	bool unusedBit_186_28 : 1 {};
	/**
	offset 484 bit 29 */
	bool unusedBit_186_29 : 1 {};
	/**
	offset 484 bit 30 */
	bool unusedBit_186_30 : 1 {};
	/**
	offset 484 bit 31 */
	bool unusedBit_186_31 : 1 {};
	/**
	 * This value is the ignition timing used when in 'fixed timing' mode, i.e. constant timing
	 * This mode is useful when adjusting distributor location.
	 * units: RPM
	 * offset 488
	 */
	angle_t fixedModeTiming;
	/**
	 * Angulo entre o Ponto Morto Superior (TDC) e o primeiro evento de trigger.
	 * Valor positivo no caso de ponto de sincronizacao antes do TDC e negativo no caso de ponto de sincronizacao apos TDC
	 * .Conhecer este angulo nos permite controlar tempo e outros angulos em referencia ao TDC.
	 * COMO FAZER:
	 * 1: Mude para modo de tempo fixo no dialogo de configuracao de ignicao
	 * 2: use uma pistola de ponto real para calibrar
	 * 3: adicione/subtraia ate que a pistola de ponto confirme o valor de tempo fixo desejado!'
	 * units: deg btdc
	 * offset 492
	 */
	angle_t globalTriggerAngleOffset;
	/**
	 * Razao/coeficiente dos divisores de voltagem de entrada em sua PCB. Por exemplo, use '2' se sua placa dividir 5v em 2.5v. Use '1.66' se sua placa dividir 5v em 3v.
	 * units: coef
	 * offset 496
	 */
	float analogInputDividerCoefficient;
	/**
	 * Esta e a razao dos resistores para a voltagem da bateria, meca a voltagem na bateria e entao ajuste este numero ate que o medidor corresponda a leitura.
	 * units: coef
	 * offset 500
	 */
	float vbattDividerCoeff;
	/**
	 * Limite de temperatura para ligar o ventilador de resfriamento, em Celsius
	 * units: deg C
	 * offset 504
	 */
	float fanOnTemperature;
	/**
	 * Limite de temperatura para desligar o ventilador de resfriamento, em Celsius
	 * units: deg C
	 * offset 508
	 */
	float fanOffTemperature;
	/**
	 * offset 512
	 */
	output_pin_e acrPin;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 514
	 */
	uint8_t alignmentFill_at_514[2] = {};
	/**
	 * Numero de revolucoes por quilometro para as rodas onde seu sensor de velocidade do veiculo esta conectado. Use uma calculadora online para determinar isso baseado no tamanho do seu pneu.
	 * units: revs/km
	 * offset 516
	 */
	float driveWheelRevPerKm;
	/**
	 * Periodo da thread CANbus em ms
	 * units: ms
	 * offset 520
	 */
	int canSleepPeriodMs;
	/**
	 * units: index
	 * offset 524
	 */
	int byFirmwareVersion;
	/**
	 * Primeiro corpo de borboleta, primeiro sensor. Veja tambem pedalPositionAdcChannel
	 * offset 528
	 */
	adc_channel_e tps1_1AdcChannel;
	/**
	 * Este e o pino de entrada do processador ao qual o circuito de voltagem da bateria esta conectado, se voce nao tem certeza de qual pino usar, verifique o esquematico que corresponde a sua PCB.
	 * offset 529
	 */
	adc_channel_e vbattAdcChannel;
	/**
	 * Este e o pino do processador ao qual seu sensor de nivel de combustivel esta conectado. Esta e uma entrada nao padrao entao precisara ser definida pelo usuario.
	 * offset 530
	 */
	adc_channel_e fuelLevelSensor;
	/**
	 * Segundo sensor de posicao do corpo de borboleta, canal unico ate agora
	 * offset 531
	 */
	adc_channel_e tps2_1AdcChannel;
	/**
	 * 0.1 is a good default value
	 * units: x
	 * offset 532
	 */
	float idle_derivativeFilterLoss;
	/**
	 * offset 536
	 */
	trigger_config_s trigger;
	/**
	 * Quantidade de afunilamento de ar extra
	 * units: %
	 * offset 548
	 */
	float airByRpmTaper;
	/**
	 * offset 552
	 */
	spi_device_e hip9011SpiDevice;
	/**
	 * Ciclo de trabalho para usar em caso de falha do sensor. Este ciclo de trabalho deve produzir a menor quantidade possivel de boost. Este ciclo tambem e usado caso qualquer das condicoes minimas de RPM/TPS/MAP nao sejam atendidas.
	 * units: %
	 * offset 553
	 */
	uint8_t boostControlSafeDutyCycle;
	/**
	 * offset 554
	 */
	adc_channel_e mafAdcChannel;
	/**
	 * offset 555
	 */
	uint8_t acrRevolutions;
	/**
	 * offset 556
	 */
	int calibrationBirthday;
	/**
	 * units: volts
	 * offset 560
	 */
	float adcVcc;
	/**
	 * Fase magica do motor: comparamos MAP instantaneo em X com MAP instantaneo em x+360 graus em um ciclo completo
	 * units: Deg
	 * offset 564
	 */
	float mapCamDetectionAnglePosition;
	/**
	 * A entrada do comando de valvulas pode ser usada apenas para deteccao de fase do motor se sua forma de trigger nao incluir sensor de comando como canal 'primario', ou pode ser usada para Variacao de Tempo de Valvulas em um dos comandos.
	 * offset 568
	 */
	brain_input_pin_e camInputs[CAM_INPUTS_COUNT] = {};
	/**
	 * offset 576
	 */
	afr_sensor_s afr;
	/**
	 * Primeiro canal de posicao do pedal do acelerador eletronico
	 * Veja throttlePedalPositionSecondAdcChannel para segundo canal
	 * Veja tambem tps1_1AdcChannel
	 * Veja throttlePedalUpVoltage e throttlePedalWOTVoltage
	 * offset 596
	 */
	adc_channel_e throttlePedalPositionAdcChannel;
	/**
	 * Limite de erro TPS/PPS
	 * units: %
	 * offset 597
	 */
	scaled_channel<uint8_t, 10, 1> etbSplit;
	/**
	 * offset 598
	 */
	Gpio tle6240_cs;
	/**
	 * offset 600
	 */
	pin_output_mode_e tle6240_csPinMode;
	/**
	 * offset 601
	 */
	pin_output_mode_e mc33810_csPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 602
	 */
	uint8_t alignmentFill_at_602[2] = {};
	/**
	 * @see hasBaroSensor
	 * offset 604
	 */
	air_pressure_sensor_config_s baroSensor;
	/**
	 * offset 616
	 */
	idle_hardware_s idle;
	/**
	 * Tempo de ignicao para remover quando um evento de detonacao ocorrer. Conselho: 5% (leve), 10% (turbo/alta compressao), 15% (alta detonacao, ex. GDI), 20% (bem agressivo)
	 * units: %
	 * offset 628
	 */
	scaled_channel<uint8_t, 10, 1> knockRetardAggression;
	/**
	 * Apos um evento de detonacao, reaplicar tempo nesta taxa.
	 * units: deg/s
	 * offset 629
	 */
	scaled_channel<uint8_t, 10, 1> knockRetardReapplyRate;
	/**
	 * Select which cam is used for engine sync. Other cams will be used only for VVT measurement, but not engine sync.
	 * offset 630
	 */
	engineSyncCam_e engineSyncCam;
	/**
	 * offset 631
	 */
	pin_output_mode_e sdCardCsPinMode;
	/**
	 * Number of turns of your vehicle speed sensor per turn of the wheels. For example if your sensor is on the transmission output, enter your axle/differential ratio. If you are using a hub-mounted sensor, enter a value of 1.0.
	 * units: ratio
	 * offset 632
	 */
	scaled_channel<uint16_t, 1000, 1> vssGearRatio;
	/**
	 * Set this so your vehicle speed signal is responsive, but not noisy. Larger value give smoother but slower response.
	 * offset 634
	 */
	uint8_t vssFilterReciprocal;
	/**
	 * Number of pulses output per revolution of the shaft where your VSS is mounted. For example, GM applications of the T56 output 17 pulses per revolution of the transmission output shaft.
	 * units: count
	 * offset 635
	 */
	uint8_t vssToothCount;
	/**
	 * Override the Y axis (load) value used for only the Idle VE table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 636
	 */
	ve_override_e idleVeOverrideMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 637
	 */
	uint8_t alignmentFill_at_637[1] = {};
	/**
	 * offset 638
	 */
	Gpio l9779_cs;
	/**
	 * offset 640
	 */
	output_pin_e injectionPins[MAX_CYLINDER_COUNT] = {};
	/**
	 * offset 664
	 */
	output_pin_e ignitionPins[MAX_CYLINDER_COUNT] = {};
	/**
	 * offset 688
	 */
	pin_output_mode_e injectionPinMode;
	/**
	 * offset 689
	 */
	pin_output_mode_e ignitionPinMode;
	/**
	 * offset 690
	 */
	output_pin_e fuelPumpPin;
	/**
	 * offset 692
	 */
	pin_output_mode_e fuelPumpPinMode;
	/**
	 * Quantas razoes de lacuna VVT consecutivas tem que corresponder aos intervalos esperados para que a sincronizacao aconteca
	 * units: count
	 * offset 693
	 */
	int8_t gapVvtTrackingLengthOverride;
	/**
	 * Check engine light, also malfunction indicator light. Always blinks once on boot.
	 * offset 694
	 */
	output_pin_e malfunctionIndicatorPin;
	/**
	 * offset 696
	 */
	pin_output_mode_e malfunctionIndicatorPinMode;
	/**
	 * offset 697
	 */
	pin_output_mode_e fanPinMode;
	/**
	 * offset 698
	 */
	output_pin_e fanPin;
	/**
	 * Some cars have a switch to indicate that clutch pedal is all the way down
	 * offset 700
	 */
	switch_input_pin_e clutchDownPin;
	/**
	 * offset 702
	 */
	output_pin_e alternatorControlPin;
	/**
	 * offset 704
	 */
	pin_output_mode_e alternatorControlPinMode;
	/**
	 * offset 705
	 */
	pin_input_mode_e clutchDownPinMode;
	/**
	 * offset 706
	 */
	Gpio digitalPotentiometerChipSelect[DIGIPOT_COUNT] = {};
	/**
	 * offset 714
	 */
	pin_output_mode_e electronicThrottlePin1Mode;
	/**
	 * offset 715
	 */
	spi_device_e max31855spiDevice;
	/**
	 * offset 716
	 */
	Gpio debugTriggerSync;
	/**
	 * Digital Potentiometer is used by stock ECU stimulation code
	 * offset 718
	 */
	spi_device_e digitalPotentiometerSpiDevice;
	/**
	 * offset 719
	 */
	pin_input_mode_e brakePedalPinMode;
	/**
	 * offset 720
	 */
	Gpio mc33972_cs;
	/**
	 * offset 722
	 */
	pin_output_mode_e mc33972_csPinMode;
	/**
	 * Useful in Research&Development phase
	 * offset 723
	 */
	adc_channel_e auxFastSensor1_adcChannel;
	/**
	 * First throttle body, second sensor.
	 * offset 724
	 */
	adc_channel_e tps1_2AdcChannel;
	/**
	 * Second throttle body, second sensor.
	 * offset 725
	 */
	adc_channel_e tps2_2AdcChannel;
	/**
	 * Electronic throttle pedal position input
	 * Second channel
	 * See also tps1_1AdcChannel
	 * See throttlePedalSecondaryUpVoltage and throttlePedalSecondaryWOTVoltage
	 * offset 726
	 */
	adc_channel_e throttlePedalPositionSecondAdcChannel;
	/**
	 * AFR, WBO, EGO - whatever you like to call it
	 * offset 727
	 */
	ego_sensor_e afr_type;
	/**
	 * offset 728
	 */
	Gpio mc33810_cs[C_MC33810_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 730
	 */
	uint8_t alignmentFill_at_730[2] = {};
	/**
	 * 0.1 is a good default value
	 * units: x
	 * offset 732
	 */
	float idle_antiwindupFreq;
	/**
	 * offset 736
	 */
	brain_input_pin_e triggerInputPins[TRIGGER_INPUT_PIN_COUNT] = {};
	/**
	 * Minimum allowed time for the boost phase. If the boost target current is reached before this time elapses, it is assumed that the injector has failed short circuit.
	 * units: us
	 * offset 740
	 */
	uint16_t mc33_t_min_boost;
	/**
	 * offset 742
	 */
	pin_output_mode_e hip9011CsPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 743
	 */
	uint8_t alignmentFill_at_743[1] = {};
	/**
	 * Ratio between the wheels and your transmission output.
	 * units: ratio
	 * offset 744
	 */
	scaled_channel<uint16_t, 100, 1> finalGearRatio;
	/**
	 * offset 746
	 */
	brain_input_pin_e tcuInputSpeedSensorPin;
	/**
	 * offset 748
	 */
	uint8_t tcuInputSpeedSensorTeeth;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 749
	 */
	uint8_t alignmentFill_at_749[3] = {};
	/**
	 * Enable adaptive learning for wall wetting parameters
	offset 752 bit 0 */
	bool wwEnableAdaptiveLearning : 1 {};
	/**
	offset 752 bit 1 */
	bool unusedBit_302_1 : 1 {};
	/**
	offset 752 bit 2 */
	bool unusedBit_302_2 : 1 {};
	/**
	offset 752 bit 3 */
	bool unusedBit_302_3 : 1 {};
	/**
	offset 752 bit 4 */
	bool unusedBit_302_4 : 1 {};
	/**
	offset 752 bit 5 */
	bool unusedBit_302_5 : 1 {};
	/**
	offset 752 bit 6 */
	bool unusedBit_302_6 : 1 {};
	/**
	offset 752 bit 7 */
	bool unusedBit_302_7 : 1 {};
	/**
	offset 752 bit 8 */
	bool unusedBit_302_8 : 1 {};
	/**
	offset 752 bit 9 */
	bool unusedBit_302_9 : 1 {};
	/**
	offset 752 bit 10 */
	bool unusedBit_302_10 : 1 {};
	/**
	offset 752 bit 11 */
	bool unusedBit_302_11 : 1 {};
	/**
	offset 752 bit 12 */
	bool unusedBit_302_12 : 1 {};
	/**
	offset 752 bit 13 */
	bool unusedBit_302_13 : 1 {};
	/**
	offset 752 bit 14 */
	bool unusedBit_302_14 : 1 {};
	/**
	offset 752 bit 15 */
	bool unusedBit_302_15 : 1 {};
	/**
	offset 752 bit 16 */
	bool unusedBit_302_16 : 1 {};
	/**
	offset 752 bit 17 */
	bool unusedBit_302_17 : 1 {};
	/**
	offset 752 bit 18 */
	bool unusedBit_302_18 : 1 {};
	/**
	offset 752 bit 19 */
	bool unusedBit_302_19 : 1 {};
	/**
	offset 752 bit 20 */
	bool unusedBit_302_20 : 1 {};
	/**
	offset 752 bit 21 */
	bool unusedBit_302_21 : 1 {};
	/**
	offset 752 bit 22 */
	bool unusedBit_302_22 : 1 {};
	/**
	offset 752 bit 23 */
	bool unusedBit_302_23 : 1 {};
	/**
	offset 752 bit 24 */
	bool unusedBit_302_24 : 1 {};
	/**
	offset 752 bit 25 */
	bool unusedBit_302_25 : 1 {};
	/**
	offset 752 bit 26 */
	bool unusedBit_302_26 : 1 {};
	/**
	offset 752 bit 27 */
	bool unusedBit_302_27 : 1 {};
	/**
	offset 752 bit 28 */
	bool unusedBit_302_28 : 1 {};
	/**
	offset 752 bit 29 */
	bool unusedBit_302_29 : 1 {};
	/**
	offset 752 bit 30 */
	bool unusedBit_302_30 : 1 {};
	/**
	offset 752 bit 31 */
	bool unusedBit_302_31 : 1 {};
	/**
	 * TPS change threshold for transient detection (per callback)
	 * units: %%
	 * offset 756
	 */
	float wwTpsThreshold;
	/**
	 * MAP change threshold for transient detection (per callback)
	 * units: kPa
	 * offset 760
	 */
	float wwMapThreshold;
	/**
	 * Learning rate for adaptive corrections
	 * offset 764
	 */
	float wwBetaLearningRate;
	/**
	 * Learning rate for adaptive corrections
	 * offset 768
	 */
	float wwTauLearningRate;
	/**
	 * Minimum coolant temperature for learning
	 * units: °C
	 * offset 772
	 */
	float wwMinCoolantTemp;
	/**
	 * Maximum coolant temperature for learning
	 * units: °C
	 * offset 776
	 */
	float wwMaxCoolantTemp;
	/**
	 * Minimum MAP pressure for learning
	 * units: kPa
	 * offset 780
	 */
	float wwMinMapForLearning;
	/**
	 * Each evoTech piece can provide synthetic trigger signal for external ECU. Sometimes these wires are routed back into trigger inputs of the same evoTech board.
	 * See also directSelfStimulation which is different.
	 * offset 784
	 */
	Gpio triggerSimulatorPins[TRIGGER_SIMULATOR_PIN_COUNT] = {};
	/**
	 * units: g/s
	 * offset 788
	 */
	scaled_channel<uint16_t, 1000, 1> fordInjectorSmallPulseSlope;
	/**
	 * offset 790
	 */
	pin_output_mode_e triggerSimulatorPinModes[TRIGGER_SIMULATOR_PIN_COUNT] = {};
	/**
	 * offset 792
	 */
	adc_channel_e maf2AdcChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 793
	 */
	uint8_t alignmentFill_at_793[1] = {};
	/**
	 * On-off O2 sensor heater control. 'ON' if engine is running, 'OFF' if stopped or cranking.
	 * offset 794
	 */
	output_pin_e o2heaterPin;
	/**
	 * offset 796
	 */
	pin_output_mode_e o2heaterPinModeTodO;
	/**
	 * units: RPM
	 * offset 797
	 */
	scaled_channel<uint8_t, 1, 100> lambdaProtectionMinRpm;
	/**
	 * units: %
	 * offset 798
	 */
	scaled_channel<uint8_t, 1, 10> lambdaProtectionMinLoad;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 799
	 */
	uint8_t alignmentFill_at_799[1] = {};
	/**
	offset 800 bit 0 */
	bool is_enabled_spi_1 : 1 {};
	/**
	offset 800 bit 1 */
	bool is_enabled_spi_2 : 1 {};
	/**
	offset 800 bit 2 */
	bool is_enabled_spi_3 : 1 {};
	/**
	 * enable sd/disable sd
	offset 800 bit 3 */
	bool isSdCardEnabled : 1 {};
	/**
	 * Use 11 bit (standard) or 29 bit (extended) IDs for evoTech verbose CAN format.
	offset 800 bit 4 */
	bool rusefiVerbose29b : 1 {};
	/**
	offset 800 bit 5 */
	bool rethrowHardFault : 1 {};
	/**
	offset 800 bit 6 */
	bool isHip9011Enabled : 1 {};
	/**
	offset 800 bit 7 */
	bool requireFootOnBrakeToCrank : 1 {};
	/**
	offset 800 bit 8 */
	bool verboseQuad : 1 {};
	/**
	 * This setting should only be used if you have a stepper motor idle valve and a stepper motor driver installed.
	offset 800 bit 9 */
	bool useStepperIdle : 1 {};
	/**
	offset 800 bit 10 */
	bool enabledStep1Limiter : 1 {};
	/**
	offset 800 bit 11 */
	bool useTpicAdvancedMode : 1 {};
	/**
	offset 800 bit 12 */
	bool lambdaProtectionEnable : 1 {};
	/**
	offset 800 bit 13 */
	bool verboseTLE8888 : 1 {};
	/**
	 * CAN broadcast using custom evoTech protocol
	 * enable can_broadcast/disable can_broadcast
	offset 800 bit 14 */
	bool enableVerboseCanTx : 1 {};
	/**
	offset 800 bit 15 */
	bool externalRusEfiGdiModule : 1 {};
	/**
	offset 800 bit 16 */
	bool flipWboChannels : 1 {};
	/**
	 * Useful for individual intakes
	offset 800 bit 17 */
	bool measureMapOnlyInOneCylinder : 1 {};
	/**
	offset 800 bit 18 */
	bool stepperForceParkingEveryRestart : 1 {};
	/**
	 * If enabled, try to fire the engine before a full engine cycle has been completed using RPM estimated from the last 90 degrees of engine rotation. As soon as the trigger syncs plus 90 degrees rotation, fuel and ignition events will occur. If disabled, worst case may require up to 4 full crank rotations before any events are scheduled.
	offset 800 bit 19 */
	bool isFasterEngineSpinUpEnabled : 1 {};
	/**
	 * This setting disables fuel injection while the engine is in overrun, this is useful as a fuel saving measure and to prevent back firing.
	offset 800 bit 20 */
	bool coastingFuelCutEnabled : 1 {};
	/**
	offset 800 bit 21 */
	bool useNoiselessTriggerDecoder : 1 {};
	/**
	offset 800 bit 22 */
	bool useIdleTimingPidControl : 1 {};
	/**
	 * Allows disabling the ETB when the engine is stopped. You may not like the power draw or PWM noise from the motor, so this lets you turn it off until it's necessary.
	offset 800 bit 23 */
	bool disableEtbWhenEngineStopped : 1 {};
	/**
	offset 800 bit 24 */
	bool is_enabled_spi_4 : 1 {};
	/**
	 * Disable the electronic throttle motor and DC idle motor for testing.
	 * This mode is for testing ETB/DC idle position sensors, etc without actually driving the throttle.
	offset 800 bit 25 */
	bool pauseEtbControl : 1 {};
	/**
	offset 800 bit 26 */
	bool tpsTpsPercentMode : 1 {};
	/**
	offset 800 bit 27 */
	bool verboseKLine : 1 {};
	/**
	offset 800 bit 28 */
	bool idleIncrementalPidCic : 1 {};
	/**
	 * AEM X-Series or evoTech Wideband
	offset 800 bit 29 */
	bool enableAemXSeries : 1 {};
	/**
	offset 800 bit 30 */
	bool unusedBit_380_30 : 1 {};
	/**
	offset 800 bit 31 */
	bool unusedBit_380_31 : 1 {};
	/**
	 * offset 804
	 */
	uint8_t sanderoPumpSpeed;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 805
	 */
	uint8_t alignmentFill_at_805[3] = {};
	/**
	offset 808 bit 0 */
	bool modeledFlowIdle : 1 {};
	/**
	offset 808 bit 1 */
	bool unusedBit_385_1 : 1 {};
	/**
	offset 808 bit 2 */
	bool unusedBit_385_2 : 1 {};
	/**
	offset 808 bit 3 */
	bool unusedBit_385_3 : 1 {};
	/**
	offset 808 bit 4 */
	bool unusedBit_385_4 : 1 {};
	/**
	offset 808 bit 5 */
	bool unusedBit_385_5 : 1 {};
	/**
	offset 808 bit 6 */
	bool unusedBit_385_6 : 1 {};
	/**
	offset 808 bit 7 */
	bool unusedBit_385_7 : 1 {};
	/**
	offset 808 bit 8 */
	bool unusedBit_385_8 : 1 {};
	/**
	offset 808 bit 9 */
	bool unusedBit_385_9 : 1 {};
	/**
	offset 808 bit 10 */
	bool unusedBit_385_10 : 1 {};
	/**
	offset 808 bit 11 */
	bool unusedBit_385_11 : 1 {};
	/**
	offset 808 bit 12 */
	bool unusedBit_385_12 : 1 {};
	/**
	offset 808 bit 13 */
	bool unusedBit_385_13 : 1 {};
	/**
	offset 808 bit 14 */
	bool unusedBit_385_14 : 1 {};
	/**
	offset 808 bit 15 */
	bool unusedBit_385_15 : 1 {};
	/**
	offset 808 bit 16 */
	bool unusedBit_385_16 : 1 {};
	/**
	offset 808 bit 17 */
	bool unusedBit_385_17 : 1 {};
	/**
	offset 808 bit 18 */
	bool unusedBit_385_18 : 1 {};
	/**
	offset 808 bit 19 */
	bool unusedBit_385_19 : 1 {};
	/**
	offset 808 bit 20 */
	bool unusedBit_385_20 : 1 {};
	/**
	offset 808 bit 21 */
	bool unusedBit_385_21 : 1 {};
	/**
	offset 808 bit 22 */
	bool unusedBit_385_22 : 1 {};
	/**
	offset 808 bit 23 */
	bool unusedBit_385_23 : 1 {};
	/**
	offset 808 bit 24 */
	bool unusedBit_385_24 : 1 {};
	/**
	offset 808 bit 25 */
	bool unusedBit_385_25 : 1 {};
	/**
	offset 808 bit 26 */
	bool unusedBit_385_26 : 1 {};
	/**
	offset 808 bit 27 */
	bool unusedBit_385_27 : 1 {};
	/**
	offset 808 bit 28 */
	bool unusedBit_385_28 : 1 {};
	/**
	offset 808 bit 29 */
	bool unusedBit_385_29 : 1 {};
	/**
	offset 808 bit 30 */
	bool unusedBit_385_30 : 1 {};
	/**
	offset 808 bit 31 */
	bool unusedBit_385_31 : 1 {};
	/**
	 * offset 812
	 */
	brain_input_pin_e logicAnalyzerPins[LOGIC_ANALYZER_CHANNEL_COUNT] = {};
	/**
	 * offset 820
	 */
	pin_output_mode_e mainRelayPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 821
	 */
	uint8_t alignmentFill_at_821[1] = {};
	/**
	 * offset 822
	 */
	Gpio hip9011CsPin;
	/**
	 * offset 824
	 */
	Gpio hip9011IntHoldPin;
	/**
	 * offset 826
	 */
	pin_output_mode_e hip9011IntHoldPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 827
	 */
	uint8_t alignmentFill_at_827[1] = {};
	/**
	 * offset 828
	 */
	uint32_t verboseCanBaseAddress;
	/**
	 * Voltagem de Boost
	 * units: v
	 * offset 832
	 */
	uint8_t mc33_hvolt;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 833
	 */
	uint8_t alignmentFill_at_833[1] = {};
	/**
	 * MAP minimo antes que o boost em malha fechada seja habilitado. Use para prevenir comportamento inadequado ao entrar em boost.
	 * units: kPa
	 * offset 834
	 */
	uint16_t minimumBoostClosedLoopMap;
	/**
	 * units: %
	 * offset 836
	 */
	int8_t initialIgnitionCutPercent;
	/**
	 * units: %
	 * offset 837
	 */
	int8_t finalIgnitionCutPercentBeforeLaunch;
	/**
	 * offset 838
	 */
	gppwm_channel_e boostOpenLoopYAxis;
	/**
	 * offset 839
	 */
	spi_device_e l9779spiDevice;
	/**
	 * offset 840
	 */
	imu_type_e imuType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 841
	 */
	uint8_t alignmentFill_at_841[1] = {};
	/**
	 * How far above idle speed do we consider idling, i.e. coasting detection threshold.
	 * For example, if target = 800, this param = 200, then anything below 1000 RPM is considered idle.
	 * units: RPM
	 * offset 842
	 */
	int16_t idlePidRpmUpperLimit;
	/**
	 * Apply nonlinearity correction below a pulse of this duration. Pulses longer than this duration will receive no adjustment.
	 * units: ms
	 * offset 844
	 */
	scaled_channel<uint16_t, 1000, 1> applyNonlinearBelowPulse;
	/**
	 * offset 846
	 */
	Gpio lps25BaroSensorScl;
	/**
	 * offset 848
	 */
	Gpio lps25BaroSensorSda;
	/**
	 * offset 850
	 */
	brain_input_pin_e vehicleSpeedSensorInputPin;
	/**
	 * Alguns veiculos tem um interruptor para indicar que o pedal da embreagem esta totalmente levantado
	 * offset 852
	 */
	switch_input_pin_e clutchUpPin;
	/**
	 * offset 854
	 */
	InjectorNonlinearMode injectorNonlinearMode;
	/**
	 * offset 855
	 */
	pin_input_mode_e clutchUpPinMode;
	/**
	 * offset 856
	 */
	Gpio max31855_cs[EGT_CHANNEL_COUNT] = {};
	/**
	 * Sensor de combustivel flex Continental/GM, tipo 50-150hz
	 * offset 872
	 */
	brain_input_pin_e flexSensorPin;
	/**
	 * Como o pino de reducao de torque e geralmente compartilhado com controle de largada, a maioria das pessoas tem uma RPM onde o comportamento abaixo disso e Controle de Largada, acima disso e Reducao de Torque
	 * units: rpm
	 * offset 874
	 */
	uint16_t torqueReductionArmingRpm;
	/**
	 * offset 876
	 */
	pin_output_mode_e stepperDirectionPinMode;
	/**
	 * offset 877
	 */
	spi_device_e mc33972spiDevice;
	/**
	 * Razao estequiometrica para seu combustivel secundario. Este valor e usado quando o sensor Flex Fuel indica E100, tipicamente 9.0
	 * units: :1
	 * offset 878
	 */
	scaled_channel<uint8_t, 10, 1> stoichRatioSecondary;
	/**
	 * Posicao maxima permitida do ETB. Algumas borboletas passam da totalmente aberta, entao isso permite limitar a totalmente aberta.
	 * units: %
	 * offset 879
	 */
	uint8_t etbMaximumPosition;
	/**
	 * Taxa que a ECU ira registrar no cartao SD, em hz (linhas de log por segundo).
	 * units: hz
	 * offset 880
	 */
	uint16_t sdCardLogFrequency;
	/**
	 * offset 882
	 */
	adc_channel_e idlePositionChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 883
	 */
	uint8_t alignmentFill_at_883[1] = {};
	/**
	 * offset 884
	 */
	uint16_t launchCorrectionsEndRpm;
	/**
	 * offset 886
	 */
	output_pin_e starterRelayDisablePin;
	/**
	 * Em alguns veiculos podemos desabilitar o motor de partida uma vez que o motor ja esta funcionando
	 * offset 888
	 */
	pin_output_mode_e starterRelayDisablePinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 889
	 */
	uint8_t alignmentFill_at_889[1] = {};
	/**
	 * Alguns Subaru e alguns Mazda usam valvula de ar de marcha lenta de duplo solenoide
	 * offset 890
	 */
	output_pin_e secondSolenoidPin;
	/**
	 * Veja tambem starterControlPin
	 * offset 892
	 */
	switch_input_pin_e startStopButtonPin;
	/**
	 * units: RPM
	 * offset 894
	 */
	scaled_channel<uint8_t, 1, 100> lambdaProtectionRestoreRpm;
	/**
	 * offset 895
	 */
	pin_output_mode_e acRelayPinMode;
	/**
	 * Esta quantidade de amostras MAP e usada para estimar o MAP atual. Esta quantidade de amostras e considerada, e o minimo e tomado. Valor recomendado e 1 para motores de borboleta unica, e seu numero de cilindros para corpos de borboleta individuais.
	 * units: count
	 * offset 896
	 */
	int mapMinBufferLength;
	/**
	 * Abaixo desta posicao da borboleta, o motor e considerado em marcha lenta. Se voce tem uma borboleta eletronica, isso verifica a posicao do pedal do acelerador em vez da posicao da borboleta, e deve ser definido para 1-2%.
	 * units: %
	 * offset 900
	 */
	int16_t idlePidDeactivationTpsThreshold;
	/**
	 * units: %
	 * offset 902
	 */
	int16_t stepperParkingExtraSteps;
	/**
	 * units: ADC
	 * offset 904
	 */
	uint16_t tps1SecondaryMin;
	/**
	 * units: ADC
	 * offset 906
	 */
	uint16_t tps1SecondaryMax;
	/**
	 * units: rpm
	 * offset 908
	 */
	int16_t antiLagRpmTreshold;
	/**
	 * Tempo maximo para girar o motor de partida quando o botao start/stop e pressionado
	 * units: Seconds
	 * offset 910
	 */
	uint16_t startCrankingDuration;
	/**
	 * This pin is used for debugging - snap a logic analyzer on it and see if it's ever high
	 * offset 912
	 */
	Gpio triggerErrorPin;
	/**
	 * offset 914
	 */
	pin_output_mode_e triggerErrorPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 915
	 */
	uint8_t alignmentFill_at_915[1] = {};
	/**
	 * offset 916
	 */
	output_pin_e acRelayPin;
	/**
	 * units: %
	 * offset 918
	 */
	uint8_t lambdaProtectionMinTps;
	/**
	 * Only respond once lambda is out of range for this period of time. Use to avoid transients triggering lambda protection when not needed
	 * units: s
	 * offset 919
	 */
	scaled_channel<uint8_t, 10, 1> lambdaProtectionTimeout;
	/**
	 * offset 920
	 */
	script_setting_t scriptSetting[SCRIPT_SETTING_COUNT] = {};
	/**
	 * offset 952
	 */
	Gpio spi1mosiPin;
	/**
	 * offset 954
	 */
	Gpio spi1misoPin;
	/**
	 * offset 956
	 */
	Gpio spi1sckPin;
	/**
	 * offset 958
	 */
	Gpio spi2mosiPin;
	/**
	 * offset 960
	 */
	Gpio spi2misoPin;
	/**
	 * offset 962
	 */
	Gpio spi2sckPin;
	/**
	 * offset 964
	 */
	Gpio spi3mosiPin;
	/**
	 * offset 966
	 */
	Gpio spi3misoPin;
	/**
	 * offset 968
	 */
	Gpio spi3sckPin;
	/**
	 * Saab Combustion Detection Module knock signal input pin
	 * also known as Saab Ion Sensing Module
	 * offset 970
	 */
	Gpio cdmInputPin;
	/**
	 * offset 972
	 */
	uart_device_e consoleUartDevice;
	/**
	 * evoTech console Sensor Sniffer mode
	 * offset 973
	 */
	sensor_chart_e sensorChartMode;
	/**
	 * offset 974
	 */
	maf_sensor_type_e mafSensorType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 975
	 */
	uint8_t alignmentFill_at_975[1] = {};
	/**
	offset 976 bit 0 */
	bool unused920_0 : 1 {};
	/**
	offset 976 bit 1 */
	bool unused920_1 : 1 {};
	/**
	 * If enabled we use two H-bridges to drive stepper idle air valve
	offset 976 bit 2 */
	bool useHbridgesToDriveIdleStepper : 1 {};
	/**
	offset 976 bit 3 */
	bool multisparkEnable : 1 {};
	/**
	offset 976 bit 4 */
	bool enableLaunchRetard : 1 {};
	/**
	offset 976 bit 5 */
	bool canInputBCM : 1 {};
	/**
	 * This property is useful if using evoTech as TCM or BCM only
	offset 976 bit 6 */
	bool consumeObdSensors : 1 {};
	/**
	 * Read VSS from OEM CAN bus according to selected CAN vehicle configuration.
	offset 976 bit 7 */
	bool enableCanVss : 1 {};
	/**
	offset 976 bit 8 */
	bool suppressWboWorkaround7048 : 1 {};
	/**
	 * If enabled, adjust at a constant rate instead of a rate proportional to the current lambda error. This mode may be easier to tune, and more tolerant of sensor noise.
	offset 976 bit 9 */
	bool stftIgnoreErrorMagnitude : 1 {};
	/**
	offset 976 bit 10 */
	bool vvtBooleanForVerySpecialCases : 1 {};
	/**
	offset 976 bit 11 */
	bool enableSoftwareKnock : 1 {};
	/**
	 * Verbose info in console below engineSnifferRpmThreshold
	 * enable vvt_details
	offset 976 bit 12 */
	bool verboseVVTDecoding : 1 {};
	/**
	offset 976 bit 13 */
	bool invertCamVVTSignal : 1 {};
	/**
	 * In Alpha-N mode, compensate for air temperature.
	offset 976 bit 14 */
	bool alphaNUseIat : 1 {};
	/**
	offset 976 bit 15 */
	bool knockBankCyl1 : 1 {};
	/**
	offset 976 bit 16 */
	bool knockBankCyl2 : 1 {};
	/**
	offset 976 bit 17 */
	bool knockBankCyl3 : 1 {};
	/**
	offset 976 bit 18 */
	bool knockBankCyl4 : 1 {};
	/**
	offset 976 bit 19 */
	bool knockBankCyl5 : 1 {};
	/**
	offset 976 bit 20 */
	bool knockBankCyl6 : 1 {};
	/**
	offset 976 bit 21 */
	bool knockBankCyl7 : 1 {};
	/**
	offset 976 bit 22 */
	bool knockBankCyl8 : 1 {};
	/**
	offset 976 bit 23 */
	bool knockBankCyl9 : 1 {};
	/**
	offset 976 bit 24 */
	bool knockBankCyl10 : 1 {};
	/**
	offset 976 bit 25 */
	bool knockBankCyl11 : 1 {};
	/**
	offset 976 bit 26 */
	bool knockBankCyl12 : 1 {};
	/**
	offset 976 bit 27 */
	bool tcuEnabled : 1 {};
	/**
	offset 976 bit 28 */
	bool canBroadcastUseChannelTwo : 1 {};
	/**
	 * If enabled we use four Push-Pull outputs to directly drive stepper idle air valve coils
	offset 976 bit 29 */
	bool useRawOutputToDriveIdleStepper : 1 {};
	/**
	 * Print incoming and outgoing second bus CAN messages in evoTech console
	offset 976 bit 30 */
	bool verboseCan2 : 1 {};
	/**
	offset 976 bit 31 */
	bool unusedBit_518_31 : 1 {};
	/**
	 * offset 980
	 */
	dc_io etbIo[ETB_COUNT] = {};
	/**
	 * Wastegate control Solenoid
	 * offset 996
	 */
	output_pin_e boostControlPin;
	/**
	 * offset 998
	 */
	pin_output_mode_e boostControlPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 999
	 */
	uint8_t alignmentFill_at_999[1] = {};
	/**
	 * offset 1000
	 */
	switch_input_pin_e ALSActivatePin;
	/**
	 * offset 1002
	 */
	switch_input_pin_e launchActivatePin;
	/**
	 * offset 1004
	 */
	pid_s boostPid;
	/**
	 * offset 1024
	 */
	boostType_e boostType;
	/**
	 * offset 1025
	 */
	pin_input_mode_e ignitionKeyDigitalPinMode;
	/**
	 * offset 1026
	 */
	Gpio ignitionKeyDigitalPin;
	/**
	 * How long it takes to enter standby mode, in seconds. Any value below 60 will cause the ECU to not enter standby, with an average consumption of 80mA
	 * units: s
	 * offset 1028
	 */
	uint16_t standbyTimeout;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1030
	 */
	uint8_t alignmentFill_at_1030[2] = {};
	/**
	 * units: Hz
	 * offset 1032
	 */
	int boostPwmFrequency;
	/**
	 * offset 1036
	 */
	launchActivationMode_e launchActivationMode;
	/**
	 * offset 1037
	 */
	antiLagActivationMode_e antiLagActivationMode;
	/**
	 * How long to look back for TPS-based acceleration enrichment. Increasing this time will trigger enrichment for longer when a throttle position change occurs.
	 * units: sec
	 * offset 1038
	 */
	scaled_channel<uint8_t, 20, 1> tpsAccelLookback;
	/**
	 * Pause closed loop fueling after deceleration fuel cut occurs. Set this to a little longer than however long is required for normal fueling behavior to resume after fuel cut.
	 * units: sec
	 * offset 1039
	 */
	scaled_channel<uint8_t, 10, 1> noFuelTrimAfterDfcoTime;
	/**
	 * Pause closed loop fueling after acceleration fuel occurs. Set this to a little longer than however long is required for normal fueling behavior to resume after fuel accel.
	 * units: sec
	 * offset 1040
	 */
	scaled_channel<uint8_t, 10, 1> noFuelTrimAfterAccelTime;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1041
	 */
	uint8_t alignmentFill_at_1041[3] = {};
	/**
	 * Launch desabilitado acima desta velocidade se a configuracao estiver acima de zero
	 * units: Kph
	 * offset 1044
	 */
	int launchSpeedThreshold;
	/**
	 * Janela de RPM de Launch inicial para ativar (subtrai do RPM de Launch)
	 * units: RPM
	 * offset 1048
	 */
	int launchRpmWindow;
	/**
	 * units: ms
	 * offset 1052
	 */
	float triggerEventsTimeoutMs;
	/**
	 * offset 1056
	 */
	float mapExpAverageAlpha;
	/**
	 * offset 1060
	 */
	float magicNumberAvailableForDevTricks;
	/**
	 * offset 1064
	 */
	float turbochargerFilter;
	/**
	 * offset 1068
	 */
	int launchTpsThreshold;
	/**
	 * offset 1072
	 */
	float launchActivateDelay;
	/**
	 * offset 1076
	 */
	stft_s stft;
	/**
	 * offset 1104
	 */
	dc_io stepperDcIo[DC_PER_STEPPER] = {};
	/**
	 * For example, BMW, GM or Chevrolet
	 * REQUIRED for evoTech Online
	 * offset 1120
	 */
	vehicle_info_t engineMake;
	/**
	 * For example, LS1 or NB2
	 * REQUIRED for evoTech Online
	 * offset 1152
	 */
	vehicle_info_t engineCode;
	/**
	 * For example, Hunchback or Orange Miata
	 * Vehicle name has to be unique between your vehicles.
	 * REQUIRED for evoTech Online
	 * offset 1184
	 */
	vehicle_info_t vehicleName;
	/**
	 * offset 1216
	 */
	output_pin_e tcu_solenoid[TCU_SOLENOID_COUNT] = {};
	/**
	 * offset 1228
	 */
	dc_function_e etbFunctions[ETB_COUNT] = {};
	/**
	 * offset 1230
	 */
	spi_device_e drv8860spiDevice;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1231
	 */
	uint8_t alignmentFill_at_1231[1] = {};
	/**
	 * offset 1232
	 */
	Gpio drv8860_cs;
	/**
	 * offset 1234
	 */
	pin_output_mode_e drv8860_csPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1235
	 */
	uint8_t alignmentFill_at_1235[1] = {};
	/**
	 * offset 1236
	 */
	Gpio drv8860_miso;
	/**
	 * offset 1238
	 */
	output_pin_e luaOutputPins[LUA_PWM_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1254
	 */
	uint8_t alignmentFill_at_1254[2] = {};
	/**
	 * Angulo entre sensor de comando e posicao zero VVT
	 * units: value
	 * offset 1256
	 */
	float vvtOffsets[CAM_INPUTS_COUNT] = {};
	/**
	 * offset 1272
	 */
	vr_threshold_s vrThreshold[VR_THRESHOLD_COUNT] = {};
	/**
	 * offset 1320
	 */
	gppwm_note_t gpPwmNote[GPPWM_CHANNELS] = {};
	/**
	 * units: ADC
	 * offset 1384
	 */
	uint16_t tps2SecondaryMin;
	/**
	 * units: ADC
	 * offset 1386
	 */
	uint16_t tps2SecondaryMax;
	/**
	 * Select which bus the wideband controller is attached to.
	offset 1388 bit 0 */
	bool widebandOnSecondBus : 1 {};
	/**
	 * Enables lambda sensor closed loop feedback for fuelling.
	offset 1388 bit 1 */
	bool fuelClosedLoopCorrectionEnabled : 1 {};
	/**
	 * Write SD card log even when powered by USB
	offset 1388 bit 2 */
	bool alwaysWriteSdCard : 1 {};
	/**
	 * Second harmonic (aka double) is usually quieter background noise
	offset 1388 bit 3 */
	bool knockDetectionUseDoubleFrequency : 1 {};
	/**
	offset 1388 bit 4 */
	bool yesUnderstandLocking : 1 {};
	/**
	 * Sometimes we have a performance issue while printing error
	offset 1388 bit 5 */
	bool silentTriggerError : 1 {};
	/**
	offset 1388 bit 6 */
	bool useLinearCltSensor : 1 {};
	/**
	 * enable can_read/disable can_read
	offset 1388 bit 7 */
	bool canReadEnabled : 1 {};
	/**
	 * enable can_write/disable can_write. See also can1ListenMode
	offset 1388 bit 8 */
	bool canWriteEnabled : 1 {};
	/**
	offset 1388 bit 9 */
	bool useLinearIatSensor : 1 {};
	/**
	offset 1388 bit 10 */
	bool enableOilPressureProtect : 1 {};
	/**
	 * Treat milliseconds value as duty cycle value, i.e. 0.5ms would become 50%
	offset 1388 bit 11 */
	bool tachPulseDurationAsDutyCycle : 1 {};
	/**
	 * This enables smart alternator control and activates the extra alternator settings.
	offset 1388 bit 12 */
	bool isAlternatorControlEnabled : 1 {};
	/**
	 * https://wiki.rusefi.com/Trigger-Configuration-Guide
	 * This setting flips the signal from the primary engine speed sensor.
	offset 1388 bit 13 */
	bool invertPrimaryTriggerSignal : 1 {};
	/**
	 * https://wiki.rusefi.com/Trigger-Configuration-Guide
	 * This setting flips the signal from the secondary engine speed sensor.
	offset 1388 bit 14 */
	bool invertSecondaryTriggerSignal : 1 {};
	/**
	offset 1388 bit 15 */
	bool cutFuelOnHardLimit : 1 {};
	/**
	 * Be careful enabling this: some engines are known to self-disassemble their valvetrain with a spark cut. Fuel cut is much safer.
	offset 1388 bit 16 */
	bool cutSparkOnHardLimit : 1 {};
	/**
	offset 1388 bit 17 */
	bool launchFuelCutEnable : 1 {};
	/**
	 * This is the Cut Mode normally used
	offset 1388 bit 18 */
	bool launchSparkCutEnable : 1 {};
	/**
	offset 1388 bit 19 */
	bool torqueReductionEnabled : 1 {};
	/**
	offset 1388 bit 20 */
	bool unused1308_20 : 1 {};
	/**
	offset 1388 bit 21 */
	bool limitTorqueReductionTime : 1 {};
	/**
	 * Are you a developer troubleshooting TS over CAN ISO/TP?
	offset 1388 bit 22 */
	bool verboseIsoTp : 1 {};
	/**
	offset 1388 bit 23 */
	bool engineSnifferFocusOnInputs : 1 {};
	/**
	offset 1388 bit 24 */
	bool unused1308_24 : 1 {};
	/**
	offset 1388 bit 25 */
	bool twoStroke : 1 {};
	/**
	 * Where is your primary skipped wheel located?
	offset 1388 bit 26 */
	bool skippedWheelOnCam : 1 {};
	/**
	offset 1388 bit 27 */
	bool unusedBit_593_27 : 1 {};
	/**
	offset 1388 bit 28 */
	bool unusedBit_593_28 : 1 {};
	/**
	offset 1388 bit 29 */
	bool unusedBit_593_29 : 1 {};
	/**
	offset 1388 bit 30 */
	bool unusedBit_593_30 : 1 {};
	/**
	offset 1388 bit 31 */
	bool unusedBit_593_31 : 1 {};
	/**
	 * offset 1392
	 */
	adc_channel_e hipOutputChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1393
	 */
	uint8_t alignmentFill_at_1393[1] = {};
	/**
	 * A/C button input
	 * offset 1394
	 */
	switch_input_pin_e acSwitch;
	/**
	 * offset 1396
	 */
	adc_channel_e vRefAdcChannel;
	/**
	 * Posicao neutra esperada
	 * units: %
	 * offset 1397
	 */
	uint8_t etbNeutralPosition;
	/**
	 * See also idleRpmPid
	 * offset 1398
	 */
	idle_mode_e idleMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1399
	 */
	uint8_t alignmentFill_at_1399[1] = {};
	/**
	offset 1400 bit 0 */
	bool isInjectionEnabled : 1 {};
	/**
	offset 1400 bit 1 */
	bool isIgnitionEnabled : 1 {};
	/**
	 * When enabled if TPS is held above 95% no fuel is injected while cranking to clear excess fuel from the cylinders.
	offset 1400 bit 2 */
	bool isCylinderCleanupEnabled : 1 {};
	/**
	 * Should we use tables to vary tau/beta based on CLT/MAP, or just with fixed values?
	offset 1400 bit 3 */
	bool complexWallModel : 1 {};
	/**
	 * RPM is measured based on last 720 degrees while instant RPM is measured based on the last 90 degrees of crank revolution
	offset 1400 bit 4 */
	bool alwaysInstantRpm : 1 {};
	/**
	offset 1400 bit 5 */
	bool isMapAveragingEnabled : 1 {};
	/**
	 * If enabled, use separate temperature multiplier table for cranking idle position.
	 * If disabled, use normal running multiplier table applied to the cranking base position.
	offset 1400 bit 6 */
	bool unusedOverrideCrankingIacSetting : 1 {};
	/**
	 * This activates a separate ignition timing table for idle conditions, this can help idle stability by using ignition retard and advance either side of the desired idle speed. Extra advance at low idle speeds will prevent stalling and extra retard at high idle speeds can help reduce engine power and slow the idle speed.
	offset 1400 bit 7 */
	bool useSeparateAdvanceForIdle : 1 {};
	/**
	offset 1400 bit 8 */
	bool isWaveAnalyzerEnabled : 1 {};
	/**
	 * This activates a separate fuel table for Idle, this allows fine tuning of the idle fuelling.
	offset 1400 bit 9 */
	bool useSeparateVeForIdle : 1 {};
	/**
	 * Verbose info in console below engineSnifferRpmThreshold
	 * enable trigger_details
	offset 1400 bit 10 */
	bool verboseTriggerSynchDetails : 1 {};
	/**
	offset 1400 bit 11 */
	bool unusedCutFuelInAcr : 1 {};
	/**
	offset 1400 bit 12 */
	bool hondaK : 1 {};
	/**
	 * This is needed if your coils are individually wired (COP) and you wish to use batch ignition (Wasted Spark).
	offset 1400 bit 13 */
	bool twoWireBatchIgnition : 1 {};
	/**
	 * Read MAP sensor on ECU start-up to use as baro value.
	offset 1400 bit 14 */
	bool useFixedBaroCorrFromMap : 1 {};
	/**
	 * In Constant mode, timing is automatically tapered to running as RPM increases.
	 * In Table mode, the "Cranking ignition advance" table is used directly.
	offset 1400 bit 15 */
	bool useSeparateAdvanceForCranking : 1 {};
	/**
	 * This enables the various ignition corrections during cranking (IAT, CLT and PID idle).
	 * You probably don't need this.
	offset 1400 bit 16 */
	bool useAdvanceCorrectionsForCranking : 1 {};
	/**
	 * Enable a second cranking table to use for E100 flex fuel, interpolating between the two based on flex fuel sensor.
	offset 1400 bit 17 */
	bool flexCranking : 1 {};
	/**
	 * This flag allows to use a special 'PID Multiplier' table (0.0-1.0) to compensate for nonlinear nature of IAC-RPM controller
	offset 1400 bit 18 */
	bool useIacPidMultTable : 1 {};
	/**
	offset 1400 bit 19 */
	bool isBoostControlEnabled : 1 {};
	/**
	 * Interpolates the Ignition Retard from 0 to 100% within the RPM Range
	offset 1400 bit 20 */
	bool launchSmoothRetard : 1 {};
	/**
	 * Some engines are OK running semi-random sequential while other engine require phase synchronization
	offset 1400 bit 21 */
	bool isPhaseSyncRequiredForIgnition : 1 {};
	/**
	 * If enabled, use a curve for RPM limit (based on coolant temperature) instead of a constant value.
	offset 1400 bit 22 */
	bool useCltBasedRpmLimit : 1 {};
	/**
	 * If enabled, don't wait for engine start to heat O2 sensors.
	 * WARNING: this will reduce the life of your sensor, as condensation in the exhaust from a cold start can crack the sensing element.
	offset 1400 bit 23 */
	bool forceO2Heating : 1 {};
	/**
	 * If increased VVT duty cycle increases the indicated VVT angle, set this to 'advance'. If it decreases, set this to 'retard'. Most intake cams use 'advance', and most exhaust cams use 'retard'.
	offset 1400 bit 24 */
	bool invertVvtControlIntake : 1 {};
	/**
	 * If increased VVT duty cycle increases the indicated VVT angle, set this to 'advance'. If it decreases, set this to 'retard'. Most intake cams use 'advance', and most exhaust cams use 'retard'.
	offset 1400 bit 25 */
	bool invertVvtControlExhaust : 1 {};
	/**
	offset 1400 bit 26 */
	bool useBiQuadOnAuxSpeedSensors : 1 {};
	/**
	 * 'Trigger' mode will write a high speed log of trigger events (warning: uses lots of space!). 'Normal' mode will write a standard MLG of sensors, engine function, etc. similar to the one captured in TunerStudio.
	offset 1400 bit 27 */
	bool sdTriggerLog : 1 {};
	/**
	offset 1400 bit 28 */
	bool unused1320_28 : 1 {};
	/**
	offset 1400 bit 29 */
	bool stepper_dc_use_two_wires : 1 {};
	/**
	offset 1400 bit 30 */
	bool watchOutForLinearTime : 1 {};
	/**
	offset 1400 bit 31 */
	bool unusedBit_636_31 : 1 {};
	/**
	 * units: count
	 * offset 1404
	 */
	uint32_t engineChartSize;
	/**
	 * units: mult
	 * offset 1408
	 */
	float turboSpeedSensorMultiplier;
	/**
	 * offset 1412
	 */
	Gpio camInputsDebug[CAM_INPUTS_COUNT] = {};
	/**
	 * Idle target speed when A/C is enabled. Some cars need the extra speed to keep the AC efficient while idling.
	 * units: RPM
	 * offset 1420
	 */
	int16_t acIdleRpmTarget;
	/**
	 * definir warningPeriod X
	 * units: seconds
	 * offset 1422
	 */
	int16_t warningPeriod;
	/**
	 * units: angle
	 * offset 1424
	 */
	float knockDetectionWindowStart;
	/**
	 * units: ms
	 * offset 1428
	 */
	float idleStepperReactionTime;
	/**
	 * units: count
	 * offset 1432
	 */
	int idleStepperTotalSteps;
	/**
	 * Posicao do pedal para perceber que precisamos reduzir o torque quando o pino de trigger e acionado
	 * offset 1436
	 */
	int torqueReductionArmingApp;
	/**
	 * Duracao em ms ou ciclo de trabalho dependendo do modo selecionado
	 * offset 1440
	 */
	float tachPulseDuractionMs;
	/**
	 * Length of time the deposited wall fuel takes to dissipate after the start of acceleration.
	 * units: Seconds
	 * offset 1444
	 */
	float wwaeTau;
	/**
	 * offset 1448
	 */
	pid_s alternatorControl;
	/**
	 * offset 1468
	 */
	pid_s etb;
	/**
	 * offset 1488
	 */
	Gpio triggerInputDebugPins[TRIGGER_INPUT_PIN_COUNT] = {};
	/**
	 * Faixa de RPM acima do limite superior para afunilamento de ar extra
	 * units: RPM
	 * offset 1492
	 */
	int16_t airTaperRpmRange;
	/**
	 * offset 1494
	 */
	brain_input_pin_e turboSpeedSensorInputPin;
	/**
	 * Closed throttle#2. todo: extract these two fields into a structure
	 * See also tps2_1AdcChannel
	 * units: ADC
	 * offset 1496
	 */
	int16_t tps2Min;
	/**
	 * Full throttle#2. tpsMax value as 10 bit ADC value. Not Voltage!
	 * See also tps1_1AdcChannel
	 * units: ADC
	 * offset 1498
	 */
	int16_t tps2Max;
	/**
	 * See also startStopButtonPin
	 * offset 1500
	 */
	output_pin_e starterControlPin;
	/**
	 * offset 1502
	 */
	pin_input_mode_e startStopButtonMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1503
	 */
	uint8_t alignmentFill_at_1503[1] = {};
	/**
	 * offset 1504
	 */
	Gpio mc33816_flag0;
	/**
	 * offset 1506
	 */
	scaled_channel<uint16_t, 1000, 1> tachPulsePerRev;
	/**
	 * Valor kPa que e muito baixo para ser verdadeiro
	 * units: kPa
	 * offset 1508
	 */
	float mapErrorDetectionTooLow;
	/**
	 * Valor kPa que e muito alto para ser verdadeiro
	 * units: kPa
	 * offset 1512
	 */
	float mapErrorDetectionTooHigh;
	/**
	 * How long to wait for the spark to fire before recharging the coil for another spark.
	 * units: ms
	 * offset 1516
	 */
	scaled_channel<uint16_t, 1000, 1> multisparkSparkDuration;
	/**
	 * This sets the dwell time for subsequent sparks. The main spark's dwell is set by the dwell table.
	 * units: ms
	 * offset 1518
	 */
	scaled_channel<uint16_t, 1000, 1> multisparkDwell;
	/**
	 * See cltIdleRpmBins
	 * offset 1520
	 */
	pid_s idleRpmPid;
	/**
	 * 0 = No fuel settling on port walls 1 = All the fuel settling on port walls setting this to 0 disables the wall wetting enrichment.
	 * units: Fraction
	 * offset 1540
	 */
	float wwaeBeta;
	/**
	 * See also EFI_CONSOLE_RX_BRAIN_PIN
	 * offset 1544
	 */
	Gpio binarySerialTxPin;
	/**
	 * offset 1546
	 */
	Gpio binarySerialRxPin;
	/**
	 * offset 1548
	 */
	Gpio auxValves[AUX_DIGITAL_VALVE_COUNT] = {};
	/**
	 * offset 1552
	 */
	switch_input_pin_e tcuUpshiftButtonPin;
	/**
	 * offset 1554
	 */
	switch_input_pin_e tcuDownshiftButtonPin;
	/**
	 * units: voltage
	 * offset 1556
	 */
	float throttlePedalUpVoltage;
	/**
	 * Pedal no chao
	 * units: voltage
	 * offset 1560
	 */
	float throttlePedalWOTVoltage;
	/**
	 * na deteccao de voltagem IGN ligar bomba de combustivel para construir pressao de combustivel
	 * units: seconds
	 * offset 1564
	 */
	int16_t startUpFuelPumpDuration;
	/**
	 * If the RPM closer to target than this value, disable closed loop idle correction to prevent oscillation
	 * units: RPM
	 * offset 1566
	 */
	int16_t idlePidRpmDeadZone;
	/**
	 * Maximum commanded airmass for the idle controller.
	 * units: mg
	 * offset 1568
	 */
	scaled_channel<uint8_t, 1, 2> idleMaximumAirmass;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1569
	 */
	uint8_t alignmentFill_at_1569[3] = {};
	/**
	 * For how long after the pin has been triggered will the cut/reduction stay active. After that, even if the pin is still triggered, torque is re-introduced
	 * units: ms
	 * offset 1572
	 */
	float torqueReductionTime;
	/**
	 * See Over/Undervoltage Shutdown/Retry bit in documentation
	offset 1576 bit 0 */
	bool mc33810DisableRecoveryMode : 1 {};
	/**
	offset 1576 bit 1 */
	bool mc33810Gpgd0Mode : 1 {};
	/**
	offset 1576 bit 2 */
	bool mc33810Gpgd1Mode : 1 {};
	/**
	offset 1576 bit 3 */
	bool mc33810Gpgd2Mode : 1 {};
	/**
	offset 1576 bit 4 */
	bool mc33810Gpgd3Mode : 1 {};
	/**
	 * Send out board statistics
	offset 1576 bit 5 */
	bool enableExtendedCanBroadcast : 1 {};
	/**
	 * global_can_data performance hack
	offset 1576 bit 6 */
	bool luaCanRxWorkaround : 1 {};
	/**
	offset 1576 bit 7 */
	bool flexSensorInverted : 1 {};
	/**
	offset 1576 bit 8 */
	bool useHardSkipInTraction : 1 {};
	/**
	 * Use Aux Speed 1 as one of speeds for wheel slip ratio?
	offset 1576 bit 9 */
	bool useAuxSpeedForSlipRatio : 1 {};
	/**
	 * VSS and Aux Speed 1 or Aux Speed 1 with Aux Speed 2?
	offset 1576 bit 10 */
	bool useVssAsSecondWheelSpeed : 1 {};
	/**
	offset 1576 bit 11 */
	bool is_enabled_spi_5 : 1 {};
	/**
	offset 1576 bit 12 */
	bool is_enabled_spi_6 : 1 {};
	/**
	 * AEM X-Series EGT gauge kit or evoTech EGT sensor from Wideband controller
	offset 1576 bit 13 */
	bool enableAemXSeriesEgt : 1 {};
	/**
	offset 1576 bit 14 */
	bool startRequestPinInverted : 1 {};
	/**
	offset 1576 bit 15 */
	bool tcu_rangeSensorPulldown : 1 {};
	/**
	offset 1576 bit 16 */
	bool devBit01 : 1 {};
	/**
	offset 1576 bit 17 */
	bool devBit0 : 1 {};
	/**
	offset 1576 bit 18 */
	bool devBit1 : 1 {};
	/**
	offset 1576 bit 19 */
	bool devBit2 : 1 {};
	/**
	offset 1576 bit 20 */
	bool devBit3 : 1 {};
	/**
	offset 1576 bit 21 */
	bool devBit4 : 1 {};
	/**
	offset 1576 bit 22 */
	bool devBit5 : 1 {};
	/**
	offset 1576 bit 23 */
	bool devBit6 : 1 {};
	/**
	offset 1576 bit 24 */
	bool devBit7 : 1 {};
	/**
	offset 1576 bit 25 */
	bool invertExhaustCamVVTSignal : 1 {};
	/**
	 * "Available via TS Plugin see https://rusefi.com/s/knock"
	offset 1576 bit 26 */
	bool enableKnockSpectrogram : 1 {};
	/**
	offset 1576 bit 27 */
	bool enableKnockSpectrogramFilter : 1 {};
	/**
	offset 1576 bit 28 */
	bool unusedBit_706_28 : 1 {};
	/**
	offset 1576 bit 29 */
	bool unusedBit_706_29 : 1 {};
	/**
	offset 1576 bit 30 */
	bool unusedBit_706_30 : 1 {};
	/**
	offset 1576 bit 31 */
	bool unusedBit_706_31 : 1 {};
	/**
	 * This is the duration in cycles that the IAC will take to reach its normal idle position, it can be used to hold the idle higher for a few seconds after cranking to improve startup.
	 * Should be 100 once tune is better
	 * units: cycles
	 * offset 1580
	 */
	int16_t afterCrankingIACtaperDuration;
	/**
	 * This value is an added for base idle value. Idle Value added when coasting and transitioning into idle.
	 * units: percent
	 * offset 1582
	 */
	int16_t iacByTpsTaper;
	/**
	 * offset 1584
	 */
	Gpio accelerometerCsPin;
	/**
	 * Below this speed, disable DFCO. Use this to prevent jerkiness from fuel enable/disable in low gears.
	 * units: kph
	 * offset 1586
	 */
	uint8_t coastingFuelCutVssLow;
	/**
	 * Above this speed, allow DFCO. Use this to prevent jerkiness from fuel enable/disable in low gears.
	 * units: kph
	 * offset 1587
	 */
	uint8_t coastingFuelCutVssHigh;
	/**
	 * Maximum change delta of TPS percentage over the 'length'. Actual TPS change has to be above this value in order for TPS/TPS acceleration to kick in.
	 * units: roc
	 * offset 1588
	 */
	float tpsAccelEnrichmentThreshold;
	/**
	 * offset 1592
	 */
	brain_input_pin_e auxSpeedSensorInputPin[AUX_SPEED_SENSOR_COUNT] = {};
	/**
	 * offset 1596
	 */
	uint8_t totalGearsCount;
	/**
	 * Sets what part of injection's is controlled by the injection phase table.
	 * offset 1597
	 */
	InjectionTimingMode injectionTimingMode;
	/**
	 * See http://rusefi.com/s/debugmode
	 * offset 1598
	 */
	debug_mode_e debugMode;
	/**
	 * Additional idle % when fan #1 is active
	 * units: %
	 * offset 1599
	 */
	uint8_t fan1ExtraIdle;
	/**
	 * Taxa de banda para TTL primario
	 * units: BPs
	 * offset 1600
	 */
	uint32_t uartConsoleSerialSpeed;
	/**
	 * For decel we simply multiply delta of TPS and tFor decel we do not use table?!
	 * units: roc
	 * offset 1604
	 */
	float tpsDecelEnleanmentThreshold;
	/**
	 * Magic multiplier, we multiply delta of TPS and get fuel squirt duration
	 * units: coeff
	 * offset 1608
	 */
	float tpsDecelEnleanmentMultiplier;
	/**
	 * Quantos graus de avanco de tempo serao reduzidos durante o Tempo de Reducao de Torque
	 * units: deg
	 * offset 1612
	 */
	float torqueReductionIgnitionRetard;
	/**
	 * units: voltage
	 * offset 1616
	 */
	float throttlePedalSecondaryUpVoltage;
	/**
	 * Pedal no chao
	 * units: voltage
	 * offset 1620
	 */
	float throttlePedalSecondaryWOTVoltage;
	/**
	 * offset 1624
	 */
	can_baudrate_e canBaudRate;
	/**
	 * Override the Y axis (load) value used for the VE table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1625
	 */
	ve_override_e veOverrideMode;
	/**
	 * offset 1626
	 */
	can_baudrate_e can2BaudRate;
	/**
	 * Override the Y axis (load) value used for the AFR table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1627
	 */
	load_override_e afrOverrideMode;
	/**
	 * units: A
	 * offset 1628
	 */
	scaled_channel<uint8_t, 10, 1> mc33_hpfp_i_peak;
	/**
	 * units: A
	 * offset 1629
	 */
	scaled_channel<uint8_t, 10, 1> mc33_hpfp_i_hold;
	/**
	 * Quanto tempo desativar a energia quando a corrente de manutencao e atingida antes de aplicar energia novamente
	 * units: us
	 * offset 1630
	 */
	uint8_t mc33_hpfp_i_hold_off;
	/**
	 * Quantidade maxima de tempo que o solenoide pode estar ativo antes de assumir um erro de programacao
	 * units: ms
	 * offset 1631
	 */
	uint8_t mc33_hpfp_max_hold;
	/**
	 * Habilite se o driver de motor DC (ponte H) inverter os sinais (ex: RZ7899 em placas Hellen)
	offset 1632 bit 0 */
	bool stepperDcInvertedPins : 1 {};
	/**
	 * Permitir OpenBLT no CAN Primário
	offset 1632 bit 1 */
	bool canOpenBLT : 1 {};
	/**
	 * Permitir OpenBLT no CAN Secundário
	offset 1632 bit 2 */
	bool can2OpenBLT : 1 {};
	/**
	 * Selecione se deseja configurar o fluxo do injetor em fluxo volumétrico (padrão, cc/min) ou fluxo de massa (g/s).
	offset 1632 bit 3 */
	bool injectorFlowAsMassFlow : 1 {};
	/**
	offset 1632 bit 4 */
	bool boardUseCanTerminator : 1 {};
	/**
	offset 1632 bit 5 */
	bool kLineDoHondaSend : 1 {};
	/**
	 * ListenMode é sobre reconhecer tráfego CAN no nível do protocolo. Diferente de canWriteEnabled
	offset 1632 bit 6 */
	bool can1ListenMode : 1 {};
	/**
	offset 1632 bit 7 */
	bool can2ListenMode : 1 {};
	/**
	offset 1632 bit 8 */
	bool unusedBit_743_8 : 1 {};
	/**
	offset 1632 bit 9 */
	bool unusedBit_743_9 : 1 {};
	/**
	offset 1632 bit 10 */
	bool unusedBit_743_10 : 1 {};
	/**
	offset 1632 bit 11 */
	bool unusedBit_743_11 : 1 {};
	/**
	offset 1632 bit 12 */
	bool unusedBit_743_12 : 1 {};
	/**
	offset 1632 bit 13 */
	bool unusedBit_743_13 : 1 {};
	/**
	offset 1632 bit 14 */
	bool unusedBit_743_14 : 1 {};
	/**
	offset 1632 bit 15 */
	bool unusedBit_743_15 : 1 {};
	/**
	offset 1632 bit 16 */
	bool unusedBit_743_16 : 1 {};
	/**
	offset 1632 bit 17 */
	bool unusedBit_743_17 : 1 {};
	/**
	offset 1632 bit 18 */
	bool unusedBit_743_18 : 1 {};
	/**
	offset 1632 bit 19 */
	bool unusedBit_743_19 : 1 {};
	/**
	offset 1632 bit 20 */
	bool unusedBit_743_20 : 1 {};
	/**
	offset 1632 bit 21 */
	bool unusedBit_743_21 : 1 {};
	/**
	offset 1632 bit 22 */
	bool unusedBit_743_22 : 1 {};
	/**
	offset 1632 bit 23 */
	bool unusedBit_743_23 : 1 {};
	/**
	offset 1632 bit 24 */
	bool unusedBit_743_24 : 1 {};
	/**
	offset 1632 bit 25 */
	bool unusedBit_743_25 : 1 {};
	/**
	offset 1632 bit 26 */
	bool unusedBit_743_26 : 1 {};
	/**
	offset 1632 bit 27 */
	bool unusedBit_743_27 : 1 {};
	/**
	offset 1632 bit 28 */
	bool unusedBit_743_28 : 1 {};
	/**
	offset 1632 bit 29 */
	bool unusedBit_743_29 : 1 {};
	/**
	offset 1632 bit 30 */
	bool unusedBit_743_30 : 1 {};
	/**
	offset 1632 bit 31 */
	bool unusedBit_743_31 : 1 {};
	/**
	 * offset 1636
	 */
	uint8_t camDecoder2jzPosition;
	/**
	 * offset 1637
	 */
	mc33810maxDwellTimer_e mc33810maxDwellTimer;
	/**
	 * Duração de cada pulso de teste
	 * units: ms
	 * offset 1638
	 */
	scaled_channel<uint16_t, 100, 1> benchTestOnTime;
	/**
	 * units: %
	 * offset 1640
	 */
	uint8_t lambdaProtectionRestoreTps;
	/**
	 * units: %
	 * offset 1641
	 */
	scaled_channel<uint8_t, 1, 10> lambdaProtectionRestoreLoad;
	/**
	 * offset 1642
	 */
	pin_input_mode_e launchActivatePinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1643
	 */
	uint8_t alignmentFill_at_1643[1] = {};
	/**
	 * offset 1644
	 */
	Gpio can2TxPin;
	/**
	 * offset 1646
	 */
	Gpio can2RxPin;
	/**
	 * offset 1648
	 */
	pin_output_mode_e starterControlPinMode;
	/**
	 * offset 1649
	 */
	adc_channel_e wastegatePositionSensor;
	/**
	 * Override the Y axis (load) value used for the ignition table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1650
	 */
	load_override_e ignOverrideMode;
	/**
	 * Select which fuel pressure sensor measures the pressure of the fuel at your injectors.
	 * offset 1651
	 */
	injector_pressure_type_e injectorPressureType;
	/**
	 * offset 1652
	 */
	output_pin_e hpfpValvePin;
	/**
	 * offset 1654
	 */
	pin_output_mode_e hpfpValvePinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1655
	 */
	uint8_t alignmentFill_at_1655[1] = {};
	/**
	 * Valor de MAP acima do qual o combustível é cortado em caso de overboost. Defina como 0 para desabilitar o corte de overboost.
	 * units: kPa (absoluto)
	 * offset 1656
	 */
	float boostCutPressure;
	/**
	 * units: kg/h
	 * offset 1660
	 */
	scaled_channel<uint8_t, 1, 5> tchargeBins[16] = {};
	/**
	 * units: ratio
	 * offset 1676
	 */
	scaled_channel<uint8_t, 100, 1> tchargeValues[16] = {};
	/**
	 * Fixed timing, useful for TDC testing
	 * units: deg
	 * offset 1692
	 */
	float fixedTiming;
	/**
	 * MAP voltage for low point
	 * units: v
	 * offset 1696
	 */
	float mapLowValueVoltage;
	/**
	 * MAP voltage for low point
	 * units: v
	 * offset 1700
	 */
	float mapHighValueVoltage;
	/**
	 * EGO value correction
	 * units: value
	 * offset 1704
	 */
	float egoValueShift;
	/**
	 * VVT output solenoid pin for this cam
	 * offset 1708
	 */
	output_pin_e vvtPins[CAM_INPUTS_COUNT] = {};
	/**
	 * Esta é a posição do IAC durante a partida, alguns motores dão partida melhor se receberem mais ar durante a partida para melhorar o enchimento dos cilindros.
	 * units: percent
	 * offset 1716
	 */
	int crankingIACposition;
	/**
	 * offset 1720
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMinRpmMinTps;
	/**
	 * offset 1721
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMinRpmMaxTps;
	/**
	 * offset 1722
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMaxRpmMinTps;
	/**
	 * offset 1723
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMaxRpmMaxTps;
	/**
	 * offset 1724
	 */
	pwm_freq_t vvtOutputFrequency;
	/**
	 * Minimim timing advance allowed. No spark on any cylinder will ever fire after this angle BTDC. For example, setting -10 here means no spark ever fires later than 10 deg ATDC. Note that this only concerns the primary spark: any trailing sparks or multispark may violate this constraint.
	 * units: deg BTDC
	 * offset 1726
	 */
	int8_t minimumIgnitionTiming;
	/**
	 * Maximum timing advance allowed. No spark on any cylinder will ever fire before this angle BTDC. For example, setting 45 here means no spark ever fires earlier than 45 deg BTDC
	 * units: deg BTDC
	 * offset 1727
	 */
	int8_t maximumIgnitionTiming;
	/**
	 * units: Hz
	 * offset 1728
	 */
	int alternatorPwmFrequency;
	/**
	 * defina vvt_mode X
	 * offset 1732
	 */
	vvt_mode_e vvtMode[CAMS_PER_BANK] = {};
	/**
	 * Marcha lenta adicional % quando o ventilador #2 está ativo
	 * units: %
	 * offset 1734
	 */
	uint8_t fan2ExtraIdle;
	/**
	 * Atraso para permitir que a pressão de combustível se estabilize antes de disparar o pulso de priming.
	 * units: seg
	 * offset 1735
	 */
	scaled_channel<uint8_t, 100, 1> primingDelay;
	/**
	 * offset 1736
	 */
	adc_channel_e auxAnalogInputs[LUA_ANALOG_INPUT_COUNT] = {};
	/**
	 * offset 1744
	 */
	output_pin_e trailingCoilPins[MAX_CYLINDER_COUNT] = {};
	/**
	 * offset 1768
	 */
	tle8888_mode_e tle8888mode;
	/**
	 * offset 1769
	 */
	pin_output_mode_e accelerometerCsPinMode;
	/**
	 * Nenhum = regulador de pressão de combustível referenciado ao MAP
	 * Pressão fixa = regulador de pressão referenciado à atmosfera (sem retorno, tipicamente)
	 * Pressão sensorizada = sensor de pressão de combustível
	 * offset 1770
	 */
	injector_compensation_mode_e injectorCompensationMode;
	/**
	 * offset 1771
	 */
	pin_output_mode_e fan2PinMode;
	/**
	 * Esta é a pressão na qual o fluxo do seu injetor é conhecido.
	 * Por exemplo, se seus injetores fluem 400cc/min a 3,5 bar, insira 350kpa aqui.
	 * units: kPa
	 * offset 1772
	 */
	float fuelReferencePressure;
	/**
	 * offset 1776
	 */
	ThermistorConf auxTempSensor1;
	/**
	 * offset 1808
	 */
	ThermistorConf auxTempSensor2;
	/**
	 * units: Deg
	 * offset 1840
	 */
	int16_t knockSamplingDuration;
	/**
	 * units: Hz
	 * offset 1842
	 */
	int16_t etbFreq;
	/**
	 * offset 1844
	 */
	pid_s etbWastegatePid;
	/**
	 * Para micro-stepping, certifique-se que a frequência PWM (etbFreq) é alta o suficiente
	 * offset 1864
	 */
	stepper_num_micro_steps_e stepperNumMicroSteps;
	/**
	 * Use para limitar a corrente quando o motor de passo está parado, não se movendo (100% = no limit)
	 * units: %
	 * offset 1865
	 */
	uint8_t stepperMinDutyCycle;
	/**
	 * Use para limitar a corrente máxima através do motor de passo (100% = no limit)
	 * units: %
	 * offset 1866
	 */
	uint8_t stepperMaxDutyCycle;
	/**
	 * offset 1867
	 */
	spi_device_e sdCardSpiDevice;
	/**
	 * correção de tempo de ignição e injeção por cilindro para motores irregulares
	 * units: deg
	 * offset 1868
	 */
	angle_t timing_offset_cylinder[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: segundos
	 * offset 1916
	 */
	float idlePidActivationTime;
	/**
	 * offset 1920
	 */
	pin_mode_e spi1SckMode;
	/**
	 * Modos podem ser usados para integração 3v<>5v usando pull-ups/pull-downs etc.
	 * offset 1921
	 */
	pin_mode_e spi1MosiMode;
	/**
	 * offset 1922
	 */
	pin_mode_e spi1MisoMode;
	/**
	 * offset 1923
	 */
	pin_mode_e spi2SckMode;
	/**
	 * offset 1924
	 */
	pin_mode_e spi2MosiMode;
	/**
	 * offset 1925
	 */
	pin_mode_e spi2MisoMode;
	/**
	 * offset 1926
	 */
	pin_mode_e spi3SckMode;
	/**
	 * offset 1927
	 */
	pin_mode_e spi3MosiMode;
	/**
	 * offset 1928
	 */
	pin_mode_e spi3MisoMode;
	/**
	 * offset 1929
	 */
	pin_output_mode_e stepperEnablePinMode;
	/**
	 * ResetB
	 * offset 1930
	 */
	Gpio mc33816_rstb;
	/**
	 * offset 1932
	 */
	Gpio mc33816_driven;
	/**
	 * Interruptor do pedal de freio
	 * offset 1934
	 */
	switch_input_pin_e brakePedalPin;
	/**
	 * PID de saída VVT
	 * TODO: renomear para vvtPid
	 * offset 1936
	 */
	pid_s auxPid[CAMS_PER_BANK] = {};
	/**
	 * offset 1976
	 */
	float injectorCorrectionPolynomial[8] = {};
	/**
	 * units: C
	 * offset 2008
	 */
	int8_t primeBins[PRIME_CURVE_COUNT] = {};
	/**
	 * offset 2016
	 */
	linear_sensor_s oilPressure;
	/**
	 * offset 2036
	 */
	spi_device_e accelerometerSpiDevice;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2037
	 */
	uint8_t alignmentFill_at_2037[1] = {};
	/**
	 * offset 2038
	 */
	output_pin_e fan2Pin;
	/**
	 * Limite de temperatura para ligar o ventilador de resfriamento #2, em Celsius
	 * units: deg C
	 * offset 2040
	 */
	uint8_t fan2OnTemperature;
	/**
	 * Limite de temperatura para desligar o ventilador de resfriamento #2, em Celsius
	 * units: deg C
	 * offset 2041
	 */
	uint8_t fan2OffTemperature;
	/**
	 * offset 2042
	 */
	Gpio stepperEnablePin;
	/**
	 * offset 2044
	 */
	Gpio tle8888_cs;
	/**
	 * offset 2046
	 */
	pin_output_mode_e tle8888_csPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2047
	 */
	uint8_t alignmentFill_at_2047[1] = {};
	/**
	 * offset 2048
	 */
	Gpio mc33816_cs;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2050
	 */
	uint8_t alignmentFill_at_2050[2] = {};
	/**
	 * units: hz
	 * offset 2052
	 */
	float auxFrequencyFilter;
	/**
	 * offset 2056
	 */
	sent_input_pin_e sentInputPins[SENT_INPUT_COUNT] = {};
	/**
	 * Define o RPM acima do qual o corte de combustível está ativo.
	 * units: rpm
	 * offset 2058
	 */
	int16_t coastingFuelCutRpmHigh;
	/**
	 * Define o RPM abaixo do qual o corte de combustível é desativado, isso previne solavancos ou problemas ao transitar para marcha lenta
	 * units: rpm
	 * offset 2060
	 */
	int16_t coastingFuelCutRpmLow;
	/**
	 * Posição do acelerador abaixo da qual o corte de combustível está ativo. Com acelerador eletrônico habilitado, isso verifica a posição do pedal em vez da borboleta, e deve ser definido para 1-2%.
	 * units: %
	 * offset 2062
	 */
	int16_t coastingFuelCutTps;
	/**
	 * O corte de combustível é desabilitado quando o motor está frio.
	 * units: C
	 * offset 2064
	 */
	int16_t coastingFuelCutClt;
	/**
	 * Aumenta a reação do PID para RPM<alvo adicionando percentual extra ao erro do PID
	 * units: %
	 * offset 2066
	 */
	int16_t pidExtraForLowRpm;
	/**
	 * Valor de MAP acima do qual a injeção de combustível é reabilitada.
	 * units: kPa
	 * offset 2068
	 */
	int16_t coastingFuelCutMap;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2070
	 */
	uint8_t alignmentFill_at_2070[2] = {};
	/**
	 * offset 2072
	 */
	linear_sensor_s highPressureFuel;
	/**
	 * offset 2092
	 */
	linear_sensor_s lowPressureFuel;
	/**
	 * offset 2112
	 */
	gppwm_note_t scriptCurveName[SCRIPT_CURVE_COUNT] = {};
	/**
	 * offset 2208
	 */
	gppwm_note_t scriptTableName[SCRIPT_TABLE_COUNT] = {};
	/**
	 * offset 2272
	 */
	gppwm_note_t scriptSettingName[SCRIPT_SETTING_COUNT] = {};
	/**
	 * Coeficiente de transferência de calor em fluxo zero.
	 * 0 means the air charge is fully heated to the same temperature as CLT.
	 * 1 means the air charge gains no heat, and enters the cylinder at the temperature measured by IAT.
	 * offset 2400
	 */
	float tChargeAirCoefMin;
	/**
	 * Coeficiente de transferência de calor em alto fluxo, conforme definido por "fluxo máximo de ar".
	 * 0 means the air charge is fully heated to the same temperature as CLT.
	 * 1 means the air charge gains no heat, and enters the cylinder at the temperature measured by IAT.
	 * offset 2404
	 */
	float tChargeAirCoefMax;
	/**
	 * Ponto de alto fluxo para estimativa de transferência de calor.
	 * Defina isso para talvez 50-75% do seu fluxo máximo de ar em WOT.
	 * units: kg/h
	 * offset 2408
	 */
	float tChargeAirFlowMax;
	/**
	 * Taxa máxima permitida de aumento para a temperatura estimada da carga de ar
	 * units: deg/sec
	 * offset 2412
	 */
	float tChargeAirIncrLimit;
	/**
	 * Taxa máxima permitida de diminuição para a temperatura estimada da carga de ar
	 * units: deg/sec
	 * offset 2416
	 */
	float tChargeAirDecrLimit;
	/**
	 * offset 2420
	 */
	float hip9011Gain;
	/**
	 * Valor mínimo de iTerm
	 * offset 2424
	 */
	int16_t etb_iTermMin;
	/**
	 * Valor máximo de iTerm
	 * offset 2426
	 */
	int16_t etb_iTermMax;
	/**
	 * Veja useIdleTimingPidControl
	 * offset 2428
	 */
	pid_s idleTimingPid;
	/**
	 * Ao entrar em marcha lenta, e as configurações do PID são agressivas, é bom fazer uma entrada suave ao entrar em malha fechada
	 * offset 2448
	 */
	float idleTimingSoftEntryTime;
	/**
	 * offset 2452
	 */
	pin_input_mode_e torqueReductionTriggerPinMode;
	/**
	 * offset 2453
	 */
	torqueReductionActivationMode_e torqueReductionActivationMode;
	/**
	 * Atraso em ciclos entre porções de enriquecimento de combustível
	 * units: ciclos
	 * offset 2454
	 */
	int16_t tpsAccelFractionPeriod;
	/**
	 * Divisor de fração: 1 ou menos = toda a porção de uma vez, ou dividido em frações decrescentes
	 * units: coef
	 * offset 2456
	 */
	float tpsAccelFractionDivisor;
	/**
	 * offset 2460
	 */
	spi_device_e tle8888spiDevice;
	/**
	 * offset 2461
	 */
	spi_device_e mc33816spiDevice;
	/**
	 * Valor mínimo de iTerm
	 * offset 2462
	 */
	int16_t idlerpmpid_iTermMin;
	/**
	 * offset 2464
	 */
	spi_device_e tle6240spiDevice;
	/**
	 * Razão estequiométrica para seu combustível primário. Quando Flex Fuel está habilitado, este valor é usado quando o sensor Flex Fuel indica E0.
	 * E0 = 14.7
	 * E10 = 14.1
	 * E85 = 9.9
	 * E100 = 9.0
	 * units: :1
	 * offset 2465
	 */
	scaled_channel<uint8_t, 10, 1> stoichRatioPrimary;
	/**
	 * Valor máximo de iTerm
	 * offset 2466
	 */
	int16_t idlerpmpid_iTermMax;
	/**
	 * Define o alcance do controle de marcha lenta no ETB. Em 100% da posição de marcha lenta, o valor especificado aqui define a posição base do ETB.
	 * units: %
	 * offset 2468
	 */
	float etbIdleThrottleRange;
	/**
	 * Selecione a qual banco de correção de combustível este cilindro pertence. Agrupe cilindros que compartilham o mesmo sensor O2
	 * offset 2472
	 */
	uint8_t cylinderBankSelect[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: mg
	 * offset 2484
	 */
	scaled_channel<uint8_t, 1, 5> primeValues[PRIME_CURVE_COUNT] = {};
	/**
	 * Ponto central de tensão do comparador de trigger
	 * units: V
	 * offset 2492
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompCenterVolt;
	/**
	 * Tensão de histerese do comparador de trigger (Mín)
	 * units: V
	 * offset 2493
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompHystMin;
	/**
	 * Tensão de histerese do comparador de trigger (Máx)
	 * units: V
	 * offset 2494
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompHystMax;
	/**
	 * RPM de saturação do sensor VR
	 * units: RPM
	 * offset 2495
	 */
	scaled_channel<uint8_t, 1, 50> triggerCompSensorSatRpm;
	/**
	 * units: razão
	 * offset 2496
	 */
	scaled_channel<uint16_t, 100, 1> tractionControlSlipBins[TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * units: RPM
	 * offset 2508
	 */
	uint8_t tractionControlSpeedBins[TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * offset 2514
	 */
	int8_t disableFan1AtSpeed;
	/**
	 * offset 2515
	 */
	int8_t disableFan2AtSpeed;
	/**
	 * offset 2516
	 */
	can_vss_nbc_e canVssNbcType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2518
	 */
	uint8_t alignmentFill_at_2518[2] = {};
	/**
	 * offset 2520
	 */
	gppwm_channel gppwm[GPPWM_CHANNELS] = {};
	/**
	 * Corrente de Boost
	 * units: mA
	 * offset 2952
	 */
	uint16_t mc33_i_boost;
	/**
	 * Corrente de Pico
	 * units: mA
	 * offset 2954
	 */
	uint16_t mc33_i_peak;
	/**
	 * Corrente de Manutenção
	 * units: mA
	 * offset 2956
	 */
	uint16_t mc33_i_hold;
	/**
	 * Tempo máximo permitido na fase de boost. Se a corrente do injetor não atingir o limite antes desse tempo, assume-se que o injetor está ausente ou com falha em circuito aberto.
	 * units: us
	 * offset 2958
	 */
	uint16_t mc33_t_max_boost;
	/**
	 * units: us
	 * offset 2960
	 */
	uint16_t mc33_t_peak_off;
	/**
	 * Duração da fase de pico
	 * units: us
	 * offset 2962
	 */
	uint16_t mc33_t_peak_tot;
	/**
	 * units: us
	 * offset 2964
	 */
	uint16_t mc33_t_bypass;
	/**
	 * units: us
	 * offset 2966
	 */
	uint16_t mc33_t_hold_off;
	/**
	 * Duração da fase de manutenção
	 * units: us
	 * offset 2968
	 */
	uint16_t mc33_t_hold_tot;
	/**
	 * offset 2970
	 */
	pin_input_mode_e tcuUpshiftButtonPinMode;
	/**
	 * offset 2971
	 */
	pin_input_mode_e tcuDownshiftButtonPinMode;
	/**
	 * offset 2972
	 */
	pin_input_mode_e acSwitchMode;
	/**
	 * offset 2973
	 */
	pin_output_mode_e tcu_solenoid_mode[TCU_SOLENOID_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2979
	 */
	uint8_t alignmentFill_at_2979[1] = {};
	/**
	 * units: razão
	 * offset 2980
	 */
	float triggerGapOverrideFrom[GAP_TRACKING_LENGTH] = {};
	/**
	 * units: razão
	 * offset 3052
	 */
	float triggerGapOverrideTo[GAP_TRACKING_LENGTH] = {};
	/**
	 * Abaixo deste RPM, use a informação do comando para sincronizar a posição do virabrequim para operação totalmente sequencial. Use isto se seu sensor de comando apresentar comportamentos estranhos em altas rotações. Defina como 0 para desabilitar e sempre usar o comando para ajudar a sincronizar o virabrequim.
	 * units: rpm
	 * offset 3124
	 */
	scaled_channel<uint8_t, 1, 50> maxCamPhaseResolveRpm;
	/**
	 * Atraso antes de cortar o combustível. Defina como 0 para cortar imediatamente sem atraso. Pode causar estouros e pipocos no escapamento...
	 * units: seg
	 * offset 3125
	 */
	scaled_channel<uint8_t, 10, 1> dfcoDelay;
	/**
	 * Atraso antes de acionar o compressor do ar-condicionado. Defina como 0 para acionar imediatamente sem atraso. Use isto para evitar queda de rotação em marcha lenta ao acionar o ar-condicionado.
	 * units: seg
	 * offset 3126
	 */
	scaled_channel<uint8_t, 10, 1> acDelay;
	/**
	 * offset 3127
	 */
	tChargeMode_e tChargeMode;
	/**
	 * units: mg
	 * offset 3128
	 */
	scaled_channel<uint16_t, 1000, 1> fordInjectorSmallPulseBreakPoint;
	/**
	 * Limite de erro do ETB (alvo vs. real) acima do qual o temporizador de travamento é iniciado. Se o temporizador atingir o tempo especificado no período de detecção de travamento, a borboleta é considerada travada e a operação do motor é limitada.
	 * units: %
	 * offset 3130
	 */
	uint8_t etbJamDetectThreshold;
	/**
	 * units: lóbulo/comando
	 * offset 3131
	 */
	uint8_t hpfpCamLobes;
	/**
	 * offset 3132
	 */
	hpfp_cam_e hpfpCam;
	/**
	 * Baixa rotação do motor para o A/C. Motores maiores podem suportar valores menores
	 * units: RPM
	 * offset 3133
	 */
	scaled_channel<int8_t, 1, 10> acLowRpmLimit;
	/**
	 * Se o tempo de ativação solicitado estiver abaixo deste ângulo, não acione a bomba
	 * units: graus
	 * offset 3134
	 */
	uint8_t hpfpMinAngle;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3135
	 */
	uint8_t alignmentFill_at_3135[1] = {};
	/**
	 * Tamanho da câmara da bomba em cc. Típica Bosch HDP5 tem 9,0mm de diâmetro, típico BMW N* curso é 4,4mm.
	 * units: cc
	 * offset 3136
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpPumpVolume;
	/**
	 * Tempo de ativação da válvula (para permitir que a bomba gere pressão e mantenha a válvula aberta sozinha)
	 * units: graus
	 * offset 3138
	 */
	uint8_t hpfpActivationAngle;
	/**
	 * offset 3139
	 */
	uint8_t issFilterReciprocal;
	/**
	 * units: %/kPa
	 * offset 3140
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpPidP;
	/**
	 * units: %/kPa/lóbulo
	 * offset 3142
	 */
	scaled_channel<uint16_t, 100000, 1> hpfpPidI;
	/**
	 * A taxa mais rápida que a pressão alvo pode ser reduzida. Isso porque HPFP não tem como aliviar pressão além de injetar combustível.
	 * units: kPa/s
	 * offset 3144
	 */
	uint16_t hpfpTargetDecay;
	/**
	 * offset 3146
	 */
	output_pin_e stepper_raw_output[4] = {};
	/**
	 * units: ratio
	 * offset 3154
	 */
	scaled_channel<uint16_t, 100, 1> gearRatio[TCU_GEAR_COUNT] = {};
	/**
	 * We need to give engine time to build oil pressure without diverting it to VVT
	 * units: ms
	 * offset 3174
	 */
	uint16_t vvtActivationDelayMs;
	/**
	 * units: Nm
	 * offset 3176
	 */
	scaled_channel<uint8_t, 1, 10> torqueTable[TORQUE_CURVE_SIZE][TORQUE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 3212
	 */
	uint16_t torqueRpmBins[TORQUE_CURVE_SIZE] = {};
	/**
	 * units: Load
	 * offset 3224
	 */
	uint16_t torqueLoadBins[TORQUE_CURVE_SIZE] = {};
	/**
	 * offset 3236
	 */
	GearControllerMode gearControllerMode;
	/**
	 * offset 3237
	 */
	TransmissionControllerMode transmissionControllerMode;
	/**
	 * During revolution where ACR should be disabled at what specific angle to disengage
	 * units: deg
	 * offset 3238
	 */
	uint16_t acrDisablePhase;
	/**
	 * offset 3240
	 */
	linear_sensor_s auxLinear1;
	/**
	 * offset 3260
	 */
	linear_sensor_s auxLinear2;
	/**
	 * offset 3280
	 */
	output_pin_e tcu_tcc_onoff_solenoid;
	/**
	 * offset 3282
	 */
	pin_output_mode_e tcu_tcc_onoff_solenoid_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3283
	 */
	uint8_t alignmentFill_at_3283[1] = {};
	/**
	 * offset 3284
	 */
	output_pin_e tcu_tcc_pwm_solenoid;
	/**
	 * offset 3286
	 */
	pin_output_mode_e tcu_tcc_pwm_solenoid_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3287
	 */
	uint8_t alignmentFill_at_3287[1] = {};
	/**
	 * offset 3288
	 */
	pwm_freq_t tcu_tcc_pwm_solenoid_freq;
	/**
	 * offset 3290
	 */
	output_pin_e tcu_pc_solenoid_pin;
	/**
	 * offset 3292
	 */
	pin_output_mode_e tcu_pc_solenoid_pin_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3293
	 */
	uint8_t alignmentFill_at_3293[1] = {};
	/**
	 * offset 3294
	 */
	pwm_freq_t tcu_pc_solenoid_freq;
	/**
	 * offset 3296
	 */
	output_pin_e tcu_32_solenoid_pin;
	/**
	 * offset 3298
	 */
	pin_output_mode_e tcu_32_solenoid_pin_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3299
	 */
	uint8_t alignmentFill_at_3299[1] = {};
	/**
	 * offset 3300
	 */
	pwm_freq_t tcu_32_solenoid_freq;
	/**
	 * offset 3302
	 */
	output_pin_e acrPin2;
	/**
	 * Set a minimum allowed target position to avoid slamming/driving against the hard mechanical stop in the throttle.
	 * units: %
	 * offset 3304
	 */
	scaled_channel<uint8_t, 10, 1> etbMinimumPosition;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3305
	 */
	uint8_t alignmentFill_at_3305[1] = {};
	/**
	 * offset 3306
	 */
	uint16_t tuneHidingKey;
	/**
	 * Individual charaters are accessible using vin(index) Lua function
	 * offset 3308
	 */
	vin_number_t vinNumber;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3325
	 */
	uint8_t alignmentFill_at_3325[1] = {};
	/**
	 * offset 3326
	 */
	uint16_t highSpeedOffsets[HIGH_SPEED_COUNT] = {};
	/**
	 * offset 3390
	 */
	fuel_pressure_sensor_mode_e fuelPressureSensorMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3391
	 */
	uint8_t alignmentFill_at_3391[1] = {};
	/**
	 * offset 3392
	 */
	switch_input_pin_e luaDigitalInputPins[LUA_DIGITAL_INPUT_COUNT] = {};
	/**
	 * units: rpm
	 * offset 3408
	 */
	int16_t ALSMinRPM;
	/**
	 * units: rpm
	 * offset 3410
	 */
	int16_t ALSMaxRPM;
	/**
	 * units: sec
	 * offset 3412
	 */
	int16_t ALSMaxDuration;
	/**
	 * units: C
	 * offset 3414
	 */
	int8_t ALSMinCLT;
	/**
	 * units: C
	 * offset 3415
	 */
	int8_t ALSMaxCLT;
	/**
	 * offset 3416
	 */
	uint8_t alsMinTimeBetween;
	/**
	 * offset 3417
	 */
	uint8_t alsEtbPosition;
	/**
	 * units: %
	 * offset 3418
	 */
	uint8_t acRelayAlternatorDutyAdder;
	/**
	 * If you have SENT TPS sensor please select type. For analog TPS leave None
	 * offset 3419
	 */
	SentEtbType sentEtbType;
	/**
	 * offset 3420
	 */
	uint16_t customSentTpsMin;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3422
	 */
	uint8_t alignmentFill_at_3422[2] = {};
	/**
	 * units: %
	 * offset 3424
	 */
	int ALSIdleAdd;
	/**
	 * units: %
	 * offset 3428
	 */
	int ALSEtbAdd;
	/**
	 * offset 3432
	 */
	float ALSSkipRatio;
	/**
	 * Hysterisis: if Pressure High Disable is 240kpa, and acPressureEnableHyst is 20, when the ECU sees 240kpa, A/C will be disabled, and stay disabled until 240-20=220kpa is reached
	 * units: kPa (absolute)
	 * offset 3436
	 */
	scaled_channel<uint8_t, 2, 1> acPressureEnableHyst;
	/**
	 * offset 3437
	 */
	pin_input_mode_e ALSActivatePinMode;
	/**
	 * For Ford TPS, use 53%. For Toyota ETCS-i, use ~65%
	 * units: %
	 * offset 3438
	 */
	scaled_channel<uint8_t, 2, 1> tpsSecondaryMaximum;
	/**
	 * For Toyota ETCS-i, use ~69%
	 * units: %
	 * offset 3439
	 */
	scaled_channel<uint8_t, 2, 1> ppsSecondaryMaximum;
	/**
	 * offset 3440
	 */
	pin_input_mode_e luaDigitalInputPinModes[LUA_DIGITAL_INPUT_COUNT] = {};
	/**
	 * offset 3448
	 */
	uint16_t customSentTpsMax;
	/**
	 * offset 3450
	 */
	uint16_t kLineBaudRate;
	/**
	 * offset 3452
	 */
	CanGpioType canGpioType;
	/**
	 * offset 3453
	 */
	UiMode uiMode;
	/**
	 * Crank angle ATDC of first lobe peak
	 * units: deg
	 * offset 3454
	 */
	int16_t hpfpPeakPos;
	/**
	 * units: us
	 * offset 3456
	 */
	int16_t kLinePeriodUs;
	/**
	 * Window that the correction will be added throughout (example, if rpm limit is 7000, and rpmSoftLimitWindowSize is 200, the corrections activate at 6800RPM, creating a 200rpm window)
	 * units: RPM
	 * offset 3458
	 */
	scaled_channel<uint8_t, 1, 10> rpmSoftLimitWindowSize;
	/**
	 * Degrees of timing REMOVED from actual timing during soft RPM limit window
	 * units: deg
	 * offset 3459
	 */
	scaled_channel<uint8_t, 5, 1> rpmSoftLimitTimingRetard;
	/**
	 * % of fuel ADDED during window
	 * units: %
	 * offset 3460
	 */
	scaled_channel<uint8_t, 5, 1> rpmSoftLimitFuelAdded;
	/**
	 * Hysterisis: if the hard limit is 7200rpm and rpmHardLimitHyst is 200rpm, then when the ECU sees 7200rpm, fuel/ign will cut, and stay cut until 7000rpm (7200-200) is reached
	 * units: RPM
	 * offset 3461
	 */
	scaled_channel<uint8_t, 1, 10> rpmHardLimitHyst;
	/**
	 * Time between bench test pulses
	 * units: ms
	 * offset 3462
	 */
	scaled_channel<uint16_t, 10, 1> benchTestOffTime;
	/**
	 * Hysterisis: if hard cut is 240kpa, and boostCutPressureHyst is 20, when the ECU sees 240kpa, fuel/ign will cut, and stay cut until 240-20=220kpa is reached
	 * units: kPa (absolute)
	 * offset 3464
	 */
	scaled_channel<uint8_t, 2, 1> boostCutPressureHyst;
	/**
	 * Boost duty cycle modified by gear
	 * units: %
	 * offset 3465
	 */
	scaled_channel<int8_t, 2, 1> gearBasedOpenLoopBoostAdder[TCU_GEAR_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3475
	 */
	uint8_t alignmentFill_at_3475[1] = {};
	/**
	 * How many test bench pulses do you want
	 * offset 3476
	 */
	uint32_t benchTestCount;
	/**
	 * How long initial idle adder is held before starting to decay.
	 * units: seconds
	 * offset 3480
	 */
	scaled_channel<uint8_t, 10, 1> iacByTpsHoldTime;
	/**
	 * How long it takes to remove initial IAC adder to return to normal idle.
	 * units: seconds
	 * offset 3481
	 */
	scaled_channel<uint8_t, 10, 1> iacByTpsDecayTime;
	/**
	 * offset 3482
	 */
	switch_input_pin_e tcu_rangeInput[RANGE_INPUT_COUNT] = {};
	/**
	 * offset 3494
	 */
	pin_input_mode_e tcu_rangeInputMode[RANGE_INPUT_COUNT] = {};
	/**
	 * Scale the reported vehicle speed value from CAN. Example: Parameter set to 1.1, CAN VSS reports 50kph, ECU will report 55kph instead.
	 * units: ratio
	 * offset 3500
	 */
	scaled_channel<uint16_t, 10000, 1> canVssScaling;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3502
	 */
	uint8_t alignmentFill_at_3502[2] = {};
	/**
	 * offset 3504
	 */
	ThermistorConf oilTempSensor;
	/**
	 * offset 3536
	 */
	ThermistorConf fuelTempSensor;
	/**
	 * offset 3568
	 */
	ThermistorConf ambientTempSensor;
	/**
	 * offset 3600
	 */
	ThermistorConf compressorDischargeTemperature;
	/**
	 * Place the sensor before the throttle, but after any turbocharger/supercharger and intercoolers if fitted. Uses the same calibration as the MAP sensor.
	 * offset 3632
	 */
	adc_channel_e throttleInletPressureChannel;
	/**
	 * Place the sensor after the turbocharger/supercharger, but before any intercoolers if fitted. Uses the same calibration as the MAP sensor.
	 * offset 3633
	 */
	adc_channel_e compressorDischargePressureChannel;
	/**
	 * offset 3634
	 */
	Gpio dacOutputPins[DAC_OUTPUT_COUNT] = {};
	/**
	 * offset 3638
	 */
	output_pin_e speedometerOutputPin;
	/**
	 * Number of speedometer pulses per kilometer travelled.
	 * offset 3640
	 */
	uint16_t speedometerPulsePerKm;
	/**
	 * offset 3642
	 */
	uint8_t simulatorCamPosition[CAM_INPUTS_COUNT] = {};
	/**
	 * offset 3646
	 */
	adc_channel_e ignKeyAdcChannel;
	/**
	 * offset 3647
	 */
	pin_mode_e spi6MisoMode;
	/**
	 * units: ratio
	 * offset 3648
	 */
	float triggerVVTGapOverrideFrom[VVT_TRACKING_LENGTH] = {};
	/**
	 * units: ratio
	 * offset 3664
	 */
	float triggerVVTGapOverrideTo[VVT_TRACKING_LENGTH] = {};
	/**
	 * units: %
	 * offset 3680
	 */
	int8_t tractionControlEtbDrop[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * If injector duty cycle hits this value, instantly cut fuel.
	 * units: %
	 * offset 3716
	 */
	uint8_t maxInjectorDutyInstant;
	/**
	 * If injector duty cycle hits this value for the specified delay time, cut fuel.
	 * units: %
	 * offset 3717
	 */
	uint8_t maxInjectorDutySustained;
	/**
	 * Timeout period for duty cycle over the sustained limit to trigger duty cycle protection.
	 * units: sec
	 * offset 3718
	 */
	scaled_channel<uint8_t, 10, 1> maxInjectorDutySustainedTimeout;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3719
	 */
	uint8_t alignmentFill_at_3719[1] = {};
	/**
	 * offset 3720
	 */
	output_pin_e injectionPinsStage2[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: Deg
	 * offset 3744
	 */
	int8_t tractionControlTimingDrop[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * units: %
	 * offset 3780
	 */
	int8_t tractionControlIgnitionSkip[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * offset 3816
	 */
	float auxSpeed1Multiplier;
	/**
	 * offset 3820
	 */
	float brakeMeanEffectivePressureDifferential;
	/**
	 * offset 3824
	 */
	Gpio spi4mosiPin;
	/**
	 * offset 3826
	 */
	Gpio spi4misoPin;
	/**
	 * offset 3828
	 */
	Gpio spi4sckPin;
	/**
	 * offset 3830
	 */
	Gpio spi5mosiPin;
	/**
	 * offset 3832
	 */
	Gpio spi5misoPin;
	/**
	 * offset 3834
	 */
	Gpio spi5sckPin;
	/**
	 * offset 3836
	 */
	Gpio spi6mosiPin;
	/**
	 * offset 3838
	 */
	Gpio spi6misoPin;
	/**
	 * offset 3840
	 */
	Gpio spi6sckPin;
	/**
	 * offset 3842
	 */
	pin_mode_e spi4SckMode;
	/**
	 * offset 3843
	 */
	pin_mode_e spi4MosiMode;
	/**
	 * offset 3844
	 */
	pin_mode_e spi4MisoMode;
	/**
	 * offset 3845
	 */
	pin_mode_e spi5SckMode;
	/**
	 * offset 3846
	 */
	pin_mode_e spi5MosiMode;
	/**
	 * offset 3847
	 */
	pin_mode_e spi5MisoMode;
	/**
	 * offset 3848
	 */
	pin_mode_e spi6SckMode;
	/**
	 * offset 3849
	 */
	pin_mode_e spi6MosiMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3850
	 */
	uint8_t alignmentFill_at_3850[2] = {};
	/**
	 * Secondary TTL channel baud rate
	 * units: BPs
	 * offset 3852
	 */
	uint32_t tunerStudioSerialSpeed;
	/**
	 * offset 3856
	 */
	Gpio camSimulatorPin;
	/**
	 * offset 3858
	 */
	pin_output_mode_e camSimulatorPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3859
	 */
	uint8_t alignmentFill_at_3859[1] = {};
	/**
	 * offset 3860
	 */
	int anotherCiTest;
	/**
	 * offset 3864
	 */
	uint32_t device_uid[3] = {};
	/**
	 * offset 3876
	 */
	adc_channel_e tcu_rangeAnalogInput[RANGE_INPUT_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3882
	 */
	uint8_t alignmentFill_at_3882[2] = {};
	/**
	 * units: Ohm
	 * offset 3884
	 */
	float tcu_rangeSensorBiasResistor;
	/**
	 * offset 3888
	 */
	MsIoBox_config_s msIoBox0;
	/**
	 * Nominal coil charge current, 0.25A step
	 * units: A
	 * offset 3892
	 */
	scaled_channel<uint8_t, 4, 1> mc33810Nomi;
	/**
	 * Maximum coil charge current, 1A step
	 * units: A
	 * offset 3893
	 */
	uint8_t mc33810Maxi;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3894
	 */
	uint8_t alignmentFill_at_3894[2] = {};
	/**
	 * offset 3896
	 */
	linear_sensor_s acPressure;
	/**
	 * value of A/C pressure in kPa before that compressor is disengaged
	 * units: kPa
	 * offset 3916
	 */
	uint16_t minAcPressure;
	/**
	 * value of A/C pressure in kPa after that compressor is disengaged
	 * units: kPa
	 * offset 3918
	 */
	uint16_t maxAcPressure;
	/**
	 * Delay before cutting fuel due to low oil pressure. Use this to ignore short pressure blips and sensor noise.
	 * units: sec
	 * offset 3920
	 */
	scaled_channel<uint8_t, 10, 1> minimumOilPressureTimeout;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3921
	 */
	uint8_t alignmentFill_at_3921[3] = {};
	/**
	 * offset 3924
	 */
	linear_sensor_s auxLinear3;
	/**
	 * offset 3944
	 */
	linear_sensor_s auxLinear4;
	/**
	 * Below TPS value all knock suppression will be disabled.
	 * units: %
	 * offset 3964
	 */
	scaled_channel<uint8_t, 1, 1> knockSuppressMinTps;
	/**
	 * Fuel to odd when a knock event occurs. Advice: 5% (mild), 10% (turbo/high comp.), 15% (high knock, e.g. GDI), 20% (spicy lump),
	 * units: %
	 * offset 3965
	 */
	scaled_channel<uint8_t, 10, 1> knockFuelTrimAggression;
	/**
	 * After a knock event, reapply fuel at this rate.
	 * units: 1%/s
	 * offset 3966
	 */
	scaled_channel<uint8_t, 10, 1> knockFuelTrimReapplyRate;
	/**
	 * Fuel trim when knock, max 30%
	 * units: %
	 * offset 3967
	 */
	scaled_channel<uint8_t, 1, 1> knockFuelTrim;
	/**
	 * units: sense
	 * offset 3968
	 */
	float knockSpectrumSensitivity;
	/**
	 * "Estimated knock frequency, ignore cylinderBore if this one > 0"
	 * units: Hz
	 * offset 3972
	 */
	float knockFrequency;
	/**
	 * Nenhum = I have a MAP-referenced fuel pressure regulator
	 * Fixed rail pressure = I have an atmosphere-referenced fuel pressure regulator (returnless, typically)
	 * Sensed rail pressure = I have a fuel pressure sensor
	 * offset 3976
	 */
	injector_compensation_mode_e secondaryInjectorCompensationMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3977
	 */
	uint8_t alignmentFill_at_3977[3] = {};
	/**
	 * This is the pressure at which your injector flow is known.
	 * For example if your injectors flow 400cc/min at 3.5 bar, enter 350kpa here.
	 * units: kPa
	 * offset 3980
	 */
	float secondaryInjectorFuelReferencePressure;
	/**
	 * SENT input connected to ETB
	 * offset 3984
	 */
	SentInput EtbSentInput;
	/**
	 * SENT input used for high pressure fuel sensor
	 * offset 3985
	 */
	SentInput FuelHighPressureSentInput;
	/**
	 * If you have SENT High Pressure Fuel Sensor please select type. For analog TPS leave None
	 * offset 3986
	 */
	SentFuelHighPressureType FuelHighPressureSentType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3987
	 */
	uint8_t alignmentFill_at_3987[1] = {};
	/**
	offset 3988 bit 0 */
	bool nitrousControlEnabled : 1 {};
	/**
	offset 3988 bit 1 */
	bool unusedFancy2 : 1 {};
	/**
	offset 3988 bit 2 */
	bool unusedFancy3 : 1 {};
	/**
	offset 3988 bit 3 */
	bool unusedFancy4 : 1 {};
	/**
	offset 3988 bit 4 */
	bool unusedFancy5 : 1 {};
	/**
	offset 3988 bit 5 */
	bool unusedFancy6 : 1 {};
	/**
	offset 3988 bit 6 */
	bool unusedFancy7 : 1 {};
	/**
	offset 3988 bit 7 */
	bool unusedFancy8 : 1 {};
	/**
	offset 3988 bit 8 */
	bool unusedFancy9 : 1 {};
	/**
	offset 3988 bit 9 */
	bool unusedFancy10 : 1 {};
	/**
	offset 3988 bit 10 */
	bool unusedFancy11 : 1 {};
	/**
	offset 3988 bit 11 */
	bool unusedFancy12 : 1 {};
	/**
	offset 3988 bit 12 */
	bool unusedFancy13 : 1 {};
	/**
	offset 3988 bit 13 */
	bool unusedFancy14 : 1 {};
	/**
	offset 3988 bit 14 */
	bool unusedFancy15 : 1 {};
	/**
	offset 3988 bit 15 */
	bool unusedFancy16 : 1 {};
	/**
	offset 3988 bit 16 */
	bool unusedFancy17 : 1 {};
	/**
	offset 3988 bit 17 */
	bool unusedFancy18 : 1 {};
	/**
	offset 3988 bit 18 */
	bool unusedFancy19 : 1 {};
	/**
	offset 3988 bit 19 */
	bool unusedFancy20 : 1 {};
	/**
	offset 3988 bit 20 */
	bool unusedFancy21 : 1 {};
	/**
	offset 3988 bit 21 */
	bool unusedFancy22 : 1 {};
	/**
	offset 3988 bit 22 */
	bool unusedFancy23 : 1 {};
	/**
	offset 3988 bit 23 */
	bool unusedFancy24 : 1 {};
	/**
	offset 3988 bit 24 */
	bool unusedFancy25 : 1 {};
	/**
	offset 3988 bit 25 */
	bool unusedFancy26 : 1 {};
	/**
	offset 3988 bit 26 */
	bool unusedFancy27 : 1 {};
	/**
	offset 3988 bit 27 */
	bool unusedFancy28 : 1 {};
	/**
	offset 3988 bit 28 */
	bool unusedFancy29 : 1 {};
	/**
	offset 3988 bit 29 */
	bool unusedFancy30 : 1 {};
	/**
	offset 3988 bit 30 */
	bool unusedFancy31 : 1 {};
	/**
	offset 3988 bit 31 */
	bool unusedFancy32 : 1 {};
	/**
	 * offset 3992
	 */
	nitrous_arming_method_e nitrousControlArmingMethod;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3993
	 */
	uint8_t alignmentFill_at_3993[1] = {};
	/**
	 * Pin that activates nitrous control
	 * offset 3994
	 */
	switch_input_pin_e nitrousControlTriggerPin;
	/**
	 * offset 3996
	 */
	pin_input_mode_e nitrousControlTriggerPinMode;
	/**
	 * offset 3997
	 */
	lua_gauge_e nitrousLuaGauge;
	/**
	 * offset 3998
	 */
	lua_gauge_meaning_e nitrousLuaGaugeMeaning;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3999
	 */
	uint8_t alignmentFill_at_3999[1] = {};
	/**
	 * offset 4000
	 */
	float nitrousLuaGaugeArmingValue;
	/**
	 * offset 4004
	 */
	int nitrousMinimumTps;
	/**
	 * units: deg C
	 * offset 4008
	 */
	uint8_t nitrousMinimumClt;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4009
	 */
	uint8_t alignmentFill_at_4009[1] = {};
	/**
	 * units: kPa
	 * offset 4010
	 */
	int16_t nitrousMaximumMap;
	/**
	 * units: afr
	 * offset 4012
	 */
	scaled_channel<uint8_t, 10, 1> nitrousMaximumAfr;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4013
	 */
	uint8_t alignmentFill_at_4013[1] = {};
	/**
	 * units: rpm
	 * offset 4014
	 */
	uint16_t nitrousActivationRpm;
	/**
	 * units: rpm
	 * offset 4016
	 */
	uint16_t nitrousDeactivationRpm;
	/**
	 * units: rpm
	 * offset 4018
	 */
	uint16_t nitrousDeactivationRpmWindow;
	/**
	 * Retard timing by this amount during DFCO. Smooths the transition back from fuel cut. After fuel is restored, ramp timing back in over the period specified.
	 * units: deg
	 * offset 4020
	 */
	uint8_t dfcoRetardDeg;
	/**
	 * Smooths the transition back from fuel cut. After fuel is restored, ramp timing back in over the period specified.
	 * units: s
	 * offset 4021
	 */
	scaled_channel<uint8_t, 10, 1> dfcoRetardRampInTime;
	/**
	 * offset 4022
	 */
	output_pin_e nitrousRelayPin;
	/**
	 * offset 4024
	 */
	pin_output_mode_e nitrousRelayPinMode;
	/**
	 * units: %
	 * offset 4025
	 */
	int8_t nitrousFuelAdderPercent;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4026
	 */
	uint8_t alignmentFill_at_4026[2] = {};
	/**
	 * Retard timing to remove from actual final timing (after all corrections) due to additional air.
	 * units: deg
	 * offset 4028
	 */
	float nitrousIgnitionRetard;
	/**
	 * units: Kph
	 * offset 4032
	 */
	uint16_t nitrousMinimumVehicleSpeed;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4034
	 */
	uint8_t alignmentFill_at_4034[2] = {};
	/**
	 * Exponential Average Alpha filtering parameter
	 * offset 4036
	 */
	float fuelLevelAveragingAlpha;
	/**
	 * How often do we update fuel level gauge
	 * units: seconds
	 * offset 4040
	 */
	float fuelLevelUpdatePeriodSec;
	/**
	 * Error below specified value
	 * units: v
	 * offset 4044
	 */
	float fuelLevelLowThresholdVoltage;
	/**
	 * Error above specified value
	 * units: v
	 * offset 4048
	 */
	float fuelLevelHighThresholdVoltage;
	/**
	 * offset 4052
	 */
	float mapExpAverageAfr;
	/**
	 * Compensates for trigger delay due to belt stretch, or other electromechanical issues. beware that raising this value advances ignition timing!
	 * units: uS
	 * offset 4056
	 */
	scaled_channel<uint8_t, 1, 1> sparkHardwareLatencyCorrection;
	/**
	 * Delay before cutting fuel due to extra high oil pressure. Use this to ignore short pressure blips and sensor noise.
	 * units: sec
	 * offset 4057
	 */
	scaled_channel<uint8_t, 10, 1> maxOilPressureTimeout;
	/**
	 * Maximum allowed oil pressure. If oil pressure exceed this level within <timeout> seconds, fuel will be cut. Set to 0 to disable.
	 * units: kPa
	 * offset 4058
	 */
	scaled_channel<uint8_t, 1, 10> maxOilPressure;
	/**
	 * units: units
	 * offset 4059
	 */
	uint8_t unusedOftenChangesDuringFirmwareUpdate[END_OF_CALIBRATION_PADDING] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4110
	 */
	uint8_t alignmentFill_at_4110[2] = {};
};
static_assert(sizeof(engine_configuration_s) == 4112);

// start of ign_cyl_trim_s
struct ign_cyl_trim_s {
	/**
	 * offset 0
	 */
	scaled_channel<int8_t, 5, 1> table[IGN_TRIM_SIZE][IGN_TRIM_SIZE] = {};
};
static_assert(sizeof(ign_cyl_trim_s) == 16);

// start of fuel_cyl_trim_s
struct fuel_cyl_trim_s {
	/**
	 * offset 0
	 */
	scaled_channel<int8_t, 5, 1> table[FUEL_TRIM_SIZE][FUEL_TRIM_SIZE] = {};
};
static_assert(sizeof(fuel_cyl_trim_s) == 16);

// start of blend_table_s
struct blend_table_s {
	/**
	 * offset 0
	 */
	scaled_channel<int16_t, 10, 1> table[BLEND_TABLE_COUNT][BLEND_TABLE_COUNT] = {};
	/**
	 * units: Load
	 * offset 128
	 */
	uint16_t loadBins[BLEND_TABLE_COUNT] = {};
	/**
	 * units: RPM
	 * offset 144
	 */
	uint16_t rpmBins[BLEND_TABLE_COUNT] = {};
	/**
	 * offset 160
	 */
	gppwm_channel_e blendParameter;
	/**
	 * offset 161
	 */
	gppwm_channel_e yAxisOverride;
	/**
	 * offset 162
	 */
	scaled_channel<int16_t, 10, 1> blendBins[BLEND_FACTOR_SIZE] = {};
	/**
	 * units: %
	 * offset 178
	 */
	scaled_channel<uint8_t, 2, 1> blendValues[BLEND_FACTOR_SIZE] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 186
	 */
	uint8_t alignmentFill_at_186[2] = {};
};
static_assert(sizeof(blend_table_s) == 188);

// start of persistent_config_s
struct persistent_config_s {
	/**
	 * offset 0
	 */
	engine_configuration_s engineConfiguration;
	/**
	 * units: mult
	 * offset 4112
	 */
	float postCrankingFactor[CRANKING_ENRICH_COUNT][CRANKING_ENRICH_COUNT] = {};
	/**
	 * units: count
	 * offset 4256
	 */
	uint16_t postCrankingDurationBins[CRANKING_ENRICH_COUNT] = {};
	/**
	 * units: C
	 * offset 4268
	 */
	int16_t postCrankingCLTBins[CRANKING_ENRICH_COUNT] = {};
	/**
	 * target TPS value, 0 to 100%
	 * TODO: use int8 data date once we template interpolation method
	 * units: target TPS position
	 * offset 4280
	 */
	float etbBiasBins[ETB_BIAS_CURVE_LENGTH] = {};
	/**
	 * PWM bias, open loop component of PID closed loop control
	 * units: ETB duty cycle bias
	 * offset 4312
	 */
	float etbBiasValues[ETB_BIAS_CURVE_LENGTH] = {};
	/**
	 * units: %
	 * offset 4344
	 */
	scaled_channel<uint8_t, 20, 1> iacPidMultTable[IAC_PID_MULT_SIZE][IAC_PID_MULT_SIZE] = {};
	/**
	 * units: Load
	 * offset 4408
	 */
	uint8_t iacPidMultLoadBins[IAC_PID_MULT_SIZE] = {};
	/**
	 * units: RPM
	 * offset 4416
	 */
	scaled_channel<uint8_t, 1, 10> iacPidMultRpmBins[IAC_PID_MULT_SIZE] = {};
	/**
	 * On Single Coil or Wasted Spark setups you have to lower dwell at high RPM
	 * units: RPM
	 * offset 4424
	 */
	uint16_t sparkDwellRpmBins[DWELL_CURVE_SIZE] = {};
	/**
	 * units: ms
	 * offset 4440
	 */
	scaled_channel<uint16_t, 100, 1> sparkDwellValues[DWELL_CURVE_SIZE] = {};
	/**
	 * CLT-based target RPM for automatic idle controller
	 * units: C
	 * offset 4456
	 */
	scaled_channel<int8_t, 1, 2> cltIdleRpmBins[CLT_CURVE_SIZE] = {};
	/**
	 * See idleRpmPid
	 * units: RPM
	 * offset 4472
	 */
	scaled_channel<uint8_t, 1, 20> cltIdleRpm[CLT_CURVE_SIZE] = {};
	/**
	 * units: deg
	 * offset 4488
	 */
	scaled_channel<int16_t, 10, 1> ignitionCltCorrTable[CLT_TIMING_CURVE_SIZE][CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * CLT-based timing correction
	 * units: C
	 * offset 4538
	 */
	int8_t ignitionCltCorrTempBins[CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * units: Load
	 * offset 4543
	 */
	scaled_channel<uint8_t, 1, 5> ignitionCltCorrLoadBins[CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * units: x
	 * offset 4548
	 */
	float scriptCurve1Bins[SCRIPT_CURVE_16] = {};
	/**
	 * units: y
	 * offset 4612
	 */
	float scriptCurve1[SCRIPT_CURVE_16] = {};
	/**
	 * units: x
	 * offset 4676
	 */
	float scriptCurve2Bins[SCRIPT_CURVE_16] = {};
	/**
	 * units: y
	 * offset 4740
	 */
	float scriptCurve2[SCRIPT_CURVE_16] = {};
	/**
	 * units: x
	 * offset 4804
	 */
	float scriptCurve3Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4836
	 */
	float scriptCurve3[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4868
	 */
	float scriptCurve4Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4900
	 */
	float scriptCurve4[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4932
	 */
	float scriptCurve5Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4964
	 */
	float scriptCurve5[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4996
	 */
	float scriptCurve6Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 5028
	 */
	float scriptCurve6[SCRIPT_CURVE_8] = {};
	/**
	 * units: kPa
	 * offset 5060
	 */
	float baroCorrPressureBins[BARO_CORR_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5076
	 */
	float baroCorrRpmBins[BARO_CORR_SIZE] = {};
	/**
	 * units: ratio
	 * offset 5092
	 */
	float baroCorrTable[BARO_CORR_SIZE][BARO_CORR_SIZE] = {};
	/**
	 * Cranking fuel correction coefficient based on TPS
	 * units: Ratio
	 * offset 5156
	 */
	float crankingTpsCoef[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: %
	 * offset 5188
	 */
	float crankingTpsBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Cranking (see useSeparateAdvanceForCranking)
	 * units: RPM
	 * offset 5220
	 */
	uint16_t crankingAdvanceBins[CRANKING_ADVANCE_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Cranking (see useSeparateAdvanceForCranking)
	 * units: deg
	 * offset 5228
	 */
	scaled_channel<int16_t, 100, 1> crankingAdvance[CRANKING_ADVANCE_CURVE_SIZE] = {};
	/**
	 * offset 5236
	 */
	scaled_channel<uint8_t, 2, 1> boostTableOpenLoop[BOOST_LOAD_COUNT][BOOST_RPM_COUNT] = {};
	/**
	 * units: RPM
	 * offset 5300
	 */
	scaled_channel<uint8_t, 1, 100> boostRpmBins[BOOST_RPM_COUNT] = {};
	/**
	 * offset 5308
	 */
	uint16_t boostOpenLoopLoadBins[BOOST_LOAD_COUNT] = {};
	/**
	 * offset 5324
	 */
	scaled_channel<uint8_t, 1, 2> boostTableClosedLoop[BOOST_LOAD_COUNT][BOOST_RPM_COUNT] = {};
	/**
	 * offset 5388
	 */
	uint16_t boostClosedLoopLoadBins[BOOST_LOAD_COUNT] = {};
	/**
	 * units: %
	 * offset 5404
	 */
	uint8_t pedalToTpsTable[PEDAL_TO_TPS_SIZE][PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %
	 * offset 5468
	 */
	uint8_t pedalToTpsPedalBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %
	 * offset 5476
	 */
	uint8_t pedalToTpsPedalSpeedBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5484
	 */
	scaled_channel<uint8_t, 1, 100> pedalToTpsRpmBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %/s
	 * offset 5492
	 */
	scaled_channel<uint8_t, 1, 10> etbMaxSpeedOpen[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %/s
	 * offset 5500
	 */
	scaled_channel<uint8_t, 1, 10> etbMaxSpeedClose[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * CLT-based cranking position multiplier for simple manual idle controller
	 * units: C
	 * offset 5508
	 */
	float cltCrankingCorrBins[CLT_CRANKING_CURVE_SIZE] = {};
	/**
	 * CLT-based cranking position multiplier for simple manual idle controller
	 * units: %
	 * offset 5540
	 */
	float cltCrankingCorr[CLT_CRANKING_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Idle (see useSeparateAdvanceForIdle)
	 * units: RPM
	 * offset 5572
	 */
	scaled_channel<uint8_t, 1, 50> idleAdvanceBins[IDLE_ADVANCE_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Idle (see useSeparateAdvanceForIdle)
	 * units: deg
	 * offset 5580
	 */
	float idleAdvance[IDLE_ADVANCE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5612
	 */
	scaled_channel<uint8_t, 1, 10> idleVeRpmBins[IDLE_VE_SIZE] = {};
	/**
	 * units: load
	 * offset 5616
	 */
	uint8_t idleVeLoadBins[IDLE_VE_SIZE] = {};
	/**
	 * units: %
	 * offset 5620
	 */
	scaled_channel<uint16_t, 10, 1> idleVeTable[IDLE_VE_SIZE][IDLE_VE_SIZE] = {};
	/**
	 * offset 5652
	 */
	lua_script_t luaScript;
	/**
	 * units: C
	 * offset 31652
	 */
	float cltFuelCorrBins[CLT_FUEL_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 31716
	 */
	float cltFuelCorr[CLT_FUEL_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 31780
	 */
	float iatFuelCorrBins[IAT_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 31844
	 */
	float iatFuelCorr[IAT_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 31908
	 */
	float crankingFuelCoef[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 31940
	 */
	float crankingFuelBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: counter
	 * offset 31972
	 */
	float crankingCycleBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 32004
	 */
	int16_t crankingCycleFuelCltBins[CRANKING_CYCLE_CLT_SIZE] = {};
	/**
	 * units: mult
	 * offset 32012
	 */
	float crankingCycleFuelCoef[CRANKING_CYCLE_CLT_SIZE][CRANKING_CURVE_SIZE] = {};
	/**
	 * RPM-based idle position
	 * units: RPM
	 * offset 32140
	 */
	scaled_channel<uint8_t, 1, 50> rpmIdleCorrBins[CLT_CURVE_SIZE] = {};
	/**
	 * CLT-based idle position for simple manual idle controller
	 * units: C
	 * offset 32156
	 */
	uint16_t cltIdleCorrBins[CLT_CURVE_SIZE] = {};
	/**
	 * CLT-based idle position for simple manual idle controller
	 * units: %
	 * offset 32188
	 */
	scaled_channel<uint16_t, 10, 1> cltIdleCorrTable[CLT_CURVE_SIZE][CLT_CURVE_SIZE] = {};
	/**
	 * Long Term Idle Trim (LTIT) multiplicativo para idle open loop
	 * units: %
	 * offset 32700
	 */
	scaled_channel<uint16_t, 10, 1> ltitTable[CLT_CURVE_SIZE] = {};
	/**
	 * Also known as MAF transfer function.
	 * kg/hour value.
	 * By the way 2.081989116 kg/h = 1 ft3/m
	 * units: kg/hour
	 * offset 32732
	 */
	float mafDecoding[MAF_DECODING_COUNT] = {};
	/**
	 * units: V
	 * offset 32860
	 */
	float mafDecodingBins[MAF_DECODING_COUNT] = {};
	/**
	 * units: deg
	 * offset 32988
	 */
	scaled_channel<int16_t, 10, 1> ignitionIatCorrTable[IAT_IGN_CORR_COUNT][IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: C
	 * offset 33116
	 */
	int8_t ignitionIatCorrTempBins[IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: Load
	 * offset 33124
	 */
	scaled_channel<uint8_t, 1, 5> ignitionIatCorrLoadBins[IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: deg
	 * offset 33132
	 */
	int16_t injectionPhase[INJ_PHASE_LOAD_COUNT][INJ_PHASE_RPM_COUNT] = {};
	/**
	 * units: Load
	 * offset 33644
	 */
	uint16_t injPhaseLoadBins[INJ_PHASE_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 33676
	 */
	uint16_t injPhaseRpmBins[INJ_PHASE_RPM_COUNT] = {};
	/**
	 * units: onoff
	 * offset 33708
	 */
	uint8_t tcuSolenoidTable[TCU_SOLENOID_COUNT][TCU_GEAR_COUNT] = {};
	/**
	 * units: kPa
	 * offset 33768
	 */
	scaled_channel<uint16_t, 100, 1> mapEstimateTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * units: % TPS
	 * offset 34280
	 */
	scaled_channel<uint16_t, 100, 1> mapEstimateTpsBins[FUEL_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 34312
	 */
	uint16_t mapEstimateRpmBins[FUEL_RPM_COUNT] = {};
	/**
	 * units: value
	 * offset 34344
	 */
	int8_t vvtTable1[VVT_TABLE_SIZE][VVT_TABLE_SIZE] = {};
	/**
	 * units: L
	 * offset 34408
	 */
	uint16_t vvtTable1LoadBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 34424
	 */
	uint16_t vvtTable1RpmBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: value
	 * offset 34440
	 */
	int8_t vvtTable2[VVT_TABLE_SIZE][VVT_TABLE_SIZE] = {};
	/**
	 * units: L
	 * offset 34504
	 */
	uint16_t vvtTable2LoadBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 34520
	 */
	uint16_t vvtTable2RpmBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: deg
	 * offset 34536
	 */
	scaled_channel<int16_t, 10, 1> ignitionTable[IGN_LOAD_COUNT][IGN_RPM_COUNT] = {};
	/**
	 * units: Load
	 * offset 35048
	 */
	uint16_t ignitionLoadBins[IGN_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 35080
	 */
	uint16_t ignitionRpmBins[IGN_RPM_COUNT] = {};
	/**
	 * units: %
	 * offset 35112
	 */
	scaled_channel<uint16_t, 10, 1> veTable[VE_LOAD_COUNT][VE_RPM_COUNT] = {};
	/**
	 * units: {bitStringValue(fuelUnits, fuelAlgorithm) }
	 * offset 35624
	 */
	uint16_t veLoadBins[VE_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 35656
	 */
	uint16_t veRpmBins[VE_RPM_COUNT] = {};
	/**
	 * units: %
	 * offset 35688
	 */
	scaled_channel<uint16_t, 10, 1> ltftTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * units: 10x%/s
	 * offset 36200
	 */
	scaled_channel<uint16_t, 10, 1> ltftCorrectionRate[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	offset 36712 bit 0 */
	bool ltftEnabled : 1 {};
	/**
	offset 36712 bit 1 */
	bool unusedBit_94_1 : 1 {};
	/**
	offset 36712 bit 2 */
	bool unusedBit_94_2 : 1 {};
	/**
	offset 36712 bit 3 */
	bool unusedBit_94_3 : 1 {};
	/**
	offset 36712 bit 4 */
	bool unusedBit_94_4 : 1 {};
	/**
	offset 36712 bit 5 */
	bool unusedBit_94_5 : 1 {};
	/**
	offset 36712 bit 6 */
	bool unusedBit_94_6 : 1 {};
	/**
	offset 36712 bit 7 */
	bool unusedBit_94_7 : 1 {};
	/**
	offset 36712 bit 8 */
	bool unusedBit_94_8 : 1 {};
	/**
	offset 36712 bit 9 */
	bool unusedBit_94_9 : 1 {};
	/**
	offset 36712 bit 10 */
	bool unusedBit_94_10 : 1 {};
	/**
	offset 36712 bit 11 */
	bool unusedBit_94_11 : 1 {};
	/**
	offset 36712 bit 12 */
	bool unusedBit_94_12 : 1 {};
	/**
	offset 36712 bit 13 */
	bool unusedBit_94_13 : 1 {};
	/**
	offset 36712 bit 14 */
	bool unusedBit_94_14 : 1 {};
	/**
	offset 36712 bit 15 */
	bool unusedBit_94_15 : 1 {};
	/**
	offset 36712 bit 16 */
	bool unusedBit_94_16 : 1 {};
	/**
	offset 36712 bit 17 */
	bool unusedBit_94_17 : 1 {};
	/**
	offset 36712 bit 18 */
	bool unusedBit_94_18 : 1 {};
	/**
	offset 36712 bit 19 */
	bool unusedBit_94_19 : 1 {};
	/**
	offset 36712 bit 20 */
	bool unusedBit_94_20 : 1 {};
	/**
	offset 36712 bit 21 */
	bool unusedBit_94_21 : 1 {};
	/**
	offset 36712 bit 22 */
	bool unusedBit_94_22 : 1 {};
	/**
	offset 36712 bit 23 */
	bool unusedBit_94_23 : 1 {};
	/**
	offset 36712 bit 24 */
	bool unusedBit_94_24 : 1 {};
	/**
	offset 36712 bit 25 */
	bool unusedBit_94_25 : 1 {};
	/**
	offset 36712 bit 26 */
	bool unusedBit_94_26 : 1 {};
	/**
	offset 36712 bit 27 */
	bool unusedBit_94_27 : 1 {};
	/**
	offset 36712 bit 28 */
	bool unusedBit_94_28 : 1 {};
	/**
	offset 36712 bit 29 */
	bool unusedBit_94_29 : 1 {};
	/**
	offset 36712 bit 30 */
	bool unusedBit_94_30 : 1 {};
	/**
	offset 36712 bit 31 */
	bool unusedBit_94_31 : 1 {};
	/**
	 * units: value
	 * offset 36716
	 */
	uint8_t ltftCRC;
	/**
	 * Minimum temperature to start correcting ltft tables
	 * units: deg C
	 * offset 36717
	 */
	uint8_t ltftMinModTemp;
	/**
	 * Minimum temperature to start using ltft tables
	 * units: deg C
	 * offset 36718
	 */
	uint8_t ltftMinTemp;
	/**
	 * How much long term fuel trim should act to reduce short term fuel trim, 100 should keep stft in about 3%, 255 in 8% and 33 in 1%, and
	 * units: %
	 * offset 36719
	 */
	uint8_t ltftPermissivity;
	/**
	 * units: %
	 * offset 36720
	 */
	uint8_t ltftMaxCorrection;
	/**
	 * units: %
	 * offset 36721
	 */
	uint8_t ltftMinCorrection;
	/**
	 * Constante de tempo do filtro EMA para STFT usado no LTFT
	 * units: 0-255
	 * offset 36722
	 */
	uint8_t ltftEmaAlpha;
	/**
	 * Limiar de rejeição para STFT (em %)
	 * units: %
	 * offset 36723
	 */
	uint8_t ltftStftRejectThreshold;
	/**
	 * Delay pós-ignição ON para aprendizado/aplicação do LTFT
	 * units: s
	 * offset 36724
	 */
	uint8_t ltftIgnitionOnDelay;
	/**
	 * Delay após ignição OFF para salvamento do LTFT
	 * units: s
	 * offset 36725
	 */
	uint8_t ltftIgnitionOffSaveDelay;
	/**
	 * units: lambda
	 * offset 36726
	 */
	scaled_channel<uint8_t, 147, 1> lambdaTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * offset 36982
	 */
	uint16_t lambdaLoadBins[FUEL_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 37014
	 */
	uint16_t lambdaRpmBins[FUEL_RPM_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 37046
	 */
	uint8_t alignmentFill_at_37046[2] = {};
	/**
	 * units: value
	 * offset 37048
	 */
	float tpsTpsAccelTable[TPS_TPS_ACCEL_TABLE][TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: from
	 * offset 37304
	 */
	float tpsTpsAccelFromRpmBins[TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: to
	 * offset 37336
	 */
	float tpsTpsAccelToRpmBins[TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: value
	 * offset 37368
	 */
	float scriptTable1[SCRIPT_TABLE_8][SCRIPT_TABLE_8] = {};
	/**
	 * units: L
	 * offset 37624
	 */
	int16_t scriptTable1LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 37640
	 */
	int16_t scriptTable1RpmBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: value
	 * offset 37656
	 */
	float scriptTable2[TABLE_2_LOAD_SIZE][TABLE_2_RPM_SIZE] = {};
	/**
	 * units: L
	 * offset 37912
	 */
	int16_t scriptTable2LoadBins[TABLE_2_LOAD_SIZE] = {};
	/**
	 * units: RPM
	 * offset 37928
	 */
	int16_t scriptTable2RpmBins[TABLE_2_RPM_SIZE] = {};
	/**
	 * units: value
	 * offset 37944
	 */
	uint8_t scriptTable3[SCRIPT_TABLE_8][SCRIPT_TABLE_8] = {};
	/**
	 * units: L
	 * offset 38008
	 */
	int16_t scriptTable3LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 38024
	 */
	int16_t scriptTable3RpmBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: value
	 * offset 38040
	 */
	uint8_t scriptTable4[SCRIPT_TABLE_8][TABLE_4_RPM] = {};
	/**
	 * units: L
	 * offset 38120
	 */
	int16_t scriptTable4LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 38136
	 */
	int16_t scriptTable4RpmBins[TABLE_4_RPM] = {};
	/**
	 * offset 38156
	 */
	uint16_t ignTrimLoadBins[IGN_TRIM_SIZE] = {};
	/**
	 * units: rpm
	 * offset 38164
	 */
	uint16_t ignTrimRpmBins[IGN_TRIM_SIZE] = {};
	/**
	 * offset 38172
	 */
	ign_cyl_trim_s ignTrims[12] = {};
	/**
	 * offset 38364
	 */
	uint16_t fuelTrimLoadBins[FUEL_TRIM_SIZE] = {};
	/**
	 * units: rpm
	 * offset 38372
	 */
	uint16_t fuelTrimRpmBins[FUEL_TRIM_SIZE] = {};
	/**
	 * offset 38380
	 */
	fuel_cyl_trim_s fuelTrims[12] = {};
	/**
	 * units: ratio
	 * offset 38572
	 */
	scaled_channel<uint16_t, 100, 1> crankingFuelCoefE100[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: Airmass
	 * offset 38588
	 */
	scaled_channel<uint8_t, 1, 5> tcu_pcAirmassBins[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38596
	 */
	uint8_t tcu_pcValsR[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38604
	 */
	uint8_t tcu_pcValsN[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38612
	 */
	uint8_t tcu_pcVals1[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38620
	 */
	uint8_t tcu_pcVals2[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38628
	 */
	uint8_t tcu_pcVals3[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38636
	 */
	uint8_t tcu_pcVals4[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38644
	 */
	uint8_t tcu_pcVals12[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38652
	 */
	uint8_t tcu_pcVals23[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38660
	 */
	uint8_t tcu_pcVals34[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38668
	 */
	uint8_t tcu_pcVals21[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38676
	 */
	uint8_t tcu_pcVals32[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 38684
	 */
	uint8_t tcu_pcVals43[TCU_TABLE_WIDTH] = {};
	/**
	 * units: TPS
	 * offset 38692
	 */
	uint8_t tcu_tccTpsBins[8] = {};
	/**
	 * units: MPH
	 * offset 38700
	 */
	uint8_t tcu_tccLockSpeed[8] = {};
	/**
	 * units: MPH
	 * offset 38708
	 */
	uint8_t tcu_tccUnlockSpeed[8] = {};
	/**
	 * units: KPH
	 * offset 38716
	 */
	uint8_t tcu_32SpeedBins[8] = {};
	/**
	 * units: %
	 * offset 38724
	 */
	uint8_t tcu_32Vals[8] = {};
	/**
	 * units: %
	 * offset 38732
	 */
	scaled_channel<int8_t, 10, 1> throttle2TrimTable[ETB2_TRIM_SIZE][ETB2_TRIM_SIZE] = {};
	/**
	 * units: %
	 * offset 38768
	 */
	uint8_t throttle2TrimTpsBins[ETB2_TRIM_SIZE] = {};
	/**
	 * units: RPM
	 * offset 38774
	 */
	scaled_channel<uint8_t, 1, 100> throttle2TrimRpmBins[ETB2_TRIM_SIZE] = {};
	/**
	 * units: deg
	 * offset 38780
	 */
	scaled_channel<uint8_t, 4, 1> maxKnockRetardTable[KNOCK_TABLE_SIZE][KNOCK_TABLE_SIZE] = {};
	/**
	 * units: %
	 * offset 38816
	 */
	uint8_t maxKnockRetardLoadBins[KNOCK_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 38822
	 */
	scaled_channel<uint8_t, 1, 100> maxKnockRetardRpmBins[KNOCK_TABLE_SIZE] = {};
	/**
	 * units: deg
	 * offset 38828
	 */
	scaled_channel<int16_t, 10, 1> ALSTimingRetardTable[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 38860
	 */
	uint16_t alsIgnRetardLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 38868
	 */
	uint16_t alsIgnRetardrpmBins[ALS_SIZE] = {};
	/**
	 * units: percent
	 * offset 38876
	 */
	scaled_channel<int16_t, 10, 1> ALSFuelAdjustment[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 38908
	 */
	uint16_t alsFuelAdjustmentLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 38916
	 */
	uint16_t alsFuelAdjustmentrpmBins[ALS_SIZE] = {};
	/**
	 * units: ratio
	 * offset 38924
	 */
	scaled_channel<int16_t, 1, 10> ALSIgnSkipTable[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 38956
	 */
	uint16_t alsIgnSkipLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 38964
	 */
	uint16_t alsIgnSkiprpmBins[ALS_SIZE] = {};
	/**
	 * offset 38972
	 */
	blend_table_s ignBlends[IGN_BLEND_COUNT] = {};
	/**
	 * offset 39348
	 */
	blend_table_s veBlends[VE_BLEND_COUNT] = {};
	/**
	 * units: %
	 * offset 40100
	 */
	scaled_channel<uint16_t, 10, 1> throttleEstimateEffectiveAreaBins[THR_EST_SIZE] = {};
	/**
	 * In units of g/s normalized to choked flow conditions
	 * units: g/s
	 * offset 40124
	 */
	scaled_channel<uint16_t, 10, 1> throttleEstimateEffectiveAreaValues[THR_EST_SIZE] = {};
	/**
	 * offset 40148
	 */
	blend_table_s boostOpenLoopBlends[BOOST_BLEND_COUNT] = {};
	/**
	 * offset 40524
	 */
	blend_table_s boostClosedLoopBlends[BOOST_BLEND_COUNT] = {};
	/**
	 * units: level
	 * offset 40900
	 */
	float tcu_rangeP[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 40924
	 */
	float tcu_rangeR[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 40948
	 */
	float tcu_rangeN[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 40972
	 */
	float tcu_rangeD[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 40996
	 */
	float tcu_rangeM[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41020
	 */
	float tcu_rangeM3[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41044
	 */
	float tcu_rangeM2[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41068
	 */
	float tcu_rangeM1[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41092
	 */
	float tcu_rangePlus[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41116
	 */
	float tcu_rangeMinus[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 41140
	 */
	float tcu_rangeLow[RANGE_INPUT_COUNT] = {};
	/**
	 * units: lambda
	 * offset 41164
	 */
	scaled_channel<uint8_t, 100, 1> lambdaMaxDeviationTable[LAM_SIZE][LAM_SIZE] = {};
	/**
	 * offset 41180
	 */
	uint16_t lambdaMaxDeviationLoadBins[LAM_SIZE] = {};
	/**
	 * units: RPM
	 * offset 41188
	 */
	uint16_t lambdaMaxDeviationRpmBins[LAM_SIZE] = {};
	/**
	 * units: %
	 * offset 41196
	 */
	uint8_t injectorStagingTable[INJ_STAGING_COUNT][INJ_STAGING_COUNT] = {};
	/**
	 * offset 41232
	 */
	uint16_t injectorStagingLoadBins[INJ_STAGING_COUNT] = {};
	/**
	 * units: RPM
	 * offset 41244
	 */
	uint16_t injectorStagingRpmBins[INJ_STAGING_COUNT] = {};
	/**
	 * units: deg C
	 * offset 41256
	 */
	int8_t wwCltBins[WWAE_TABLE_SIZE] = {};
	/**
	 * offset 41264
	 */
	scaled_channel<uint8_t, 100, 1> wwTauCltValues[WWAE_TABLE_SIZE] = {};
	/**
	 * offset 41272
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaCltValues[WWAE_TABLE_SIZE] = {};
	/**
	 * units: kPa
	 * offset 41280
	 */
	int8_t wwMapBins[WWAE_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 41288
	 */
	scaled_channel<uint8_t, 1, 50> wwRpmBins[WWAE_RPM_SIZE] = {};
	/**
	 * offset 41296
	 */
	scaled_channel<uint8_t, 100, 1> wwTauMapRpmValues[WWAE_RPM_SIZE][WWAE_TABLE_SIZE] = {};
	/**
	 * offset 41360
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaMapRpmValues[WWAE_RPM_SIZE][WWAE_TABLE_SIZE] = {};
	/**
	 * units: kPa
	 * offset 41424
	 */
	scaled_channel<uint8_t, 1, 1> wwCorrectionMapBins[WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 41432
	 */
	scaled_channel<uint8_t, 1, 50> wwCorrectionRpmBins[WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: Tau correction multiplier
	 * offset 41440
	 */
	scaled_channel<uint8_t, 100, 1> wwTauCorrection[WWAE_CORRECTION_SIZE][WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: Beta correction multiplier
	 * offset 41504
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaCorrection[WWAE_CORRECTION_SIZE][WWAE_CORRECTION_SIZE] = {};
	/**
	 * 200
	 * units: Wall Wetting Buffer Size
	 * offset 41568
	 */
	uint16_t wwBufferSize;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 41570
	 */
	uint8_t alignmentFill_at_41570[2] = {};
	/**
	 * "Enable directional wall wetting corrections"
	offset 41572 bit 0 */
	bool wwDirectionalCorrections : 1 {};
	/**
	offset 41572 bit 1 */
	bool unusedBit_231_1 : 1 {};
	/**
	offset 41572 bit 2 */
	bool unusedBit_231_2 : 1 {};
	/**
	offset 41572 bit 3 */
	bool unusedBit_231_3 : 1 {};
	/**
	offset 41572 bit 4 */
	bool unusedBit_231_4 : 1 {};
	/**
	offset 41572 bit 5 */
	bool unusedBit_231_5 : 1 {};
	/**
	offset 41572 bit 6 */
	bool unusedBit_231_6 : 1 {};
	/**
	offset 41572 bit 7 */
	bool unusedBit_231_7 : 1 {};
	/**
	offset 41572 bit 8 */
	bool unusedBit_231_8 : 1 {};
	/**
	offset 41572 bit 9 */
	bool unusedBit_231_9 : 1 {};
	/**
	offset 41572 bit 10 */
	bool unusedBit_231_10 : 1 {};
	/**
	offset 41572 bit 11 */
	bool unusedBit_231_11 : 1 {};
	/**
	offset 41572 bit 12 */
	bool unusedBit_231_12 : 1 {};
	/**
	offset 41572 bit 13 */
	bool unusedBit_231_13 : 1 {};
	/**
	offset 41572 bit 14 */
	bool unusedBit_231_14 : 1 {};
	/**
	offset 41572 bit 15 */
	bool unusedBit_231_15 : 1 {};
	/**
	offset 41572 bit 16 */
	bool unusedBit_231_16 : 1 {};
	/**
	offset 41572 bit 17 */
	bool unusedBit_231_17 : 1 {};
	/**
	offset 41572 bit 18 */
	bool unusedBit_231_18 : 1 {};
	/**
	offset 41572 bit 19 */
	bool unusedBit_231_19 : 1 {};
	/**
	offset 41572 bit 20 */
	bool unusedBit_231_20 : 1 {};
	/**
	offset 41572 bit 21 */
	bool unusedBit_231_21 : 1 {};
	/**
	offset 41572 bit 22 */
	bool unusedBit_231_22 : 1 {};
	/**
	offset 41572 bit 23 */
	bool unusedBit_231_23 : 1 {};
	/**
	offset 41572 bit 24 */
	bool unusedBit_231_24 : 1 {};
	/**
	offset 41572 bit 25 */
	bool unusedBit_231_25 : 1 {};
	/**
	offset 41572 bit 26 */
	bool unusedBit_231_26 : 1 {};
	/**
	offset 41572 bit 27 */
	bool unusedBit_231_27 : 1 {};
	/**
	offset 41572 bit 28 */
	bool unusedBit_231_28 : 1 {};
	/**
	offset 41572 bit 29 */
	bool unusedBit_231_29 : 1 {};
	/**
	offset 41572 bit 30 */
	bool unusedBit_231_30 : 1 {};
	/**
	offset 41572 bit 31 */
	bool unusedBit_231_31 : 1 {};
	/**
	 * units: %
	 * offset 41576
	 */
	scaled_channel<uint8_t, 2, 1> hpfpLobeProfileQuantityBins[HPFP_LOBE_PROFILE_SIZE] = {};
	/**
	 * units: deg
	 * offset 41592
	 */
	scaled_channel<uint8_t, 2, 1> hpfpLobeProfileAngle[HPFP_LOBE_PROFILE_SIZE] = {};
	/**
	 * units: volts
	 * offset 41608
	 */
	uint8_t hpfpDeadtimeVoltsBins[HPFP_DEADTIME_SIZE] = {};
	/**
	 * units: ms
	 * offset 41616
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpDeadtimeMS[HPFP_DEADTIME_SIZE] = {};
	/**
	 * units: kPa
	 * offset 41632
	 */
	uint16_t hpfpTarget[HPFP_TARGET_SIZE][HPFP_TARGET_SIZE] = {};
	/**
	 * units: load
	 * offset 41832
	 */
	scaled_channel<uint16_t, 10, 1> hpfpTargetLoadBins[HPFP_TARGET_SIZE] = {};
	/**
	 * units: RPM
	 * offset 41852
	 */
	scaled_channel<uint16_t, 1, 50> hpfpTargetRpmBins[HPFP_TARGET_SIZE] = {};
	/**
	 * units: %
	 * offset 41872
	 */
	int8_t hpfpCompensation[HPFP_COMPENSATION_SIZE][HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: cc/lobe
	 * offset 41972
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpCompensationLoadBins[HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 41992
	 */
	scaled_channel<uint16_t, 1, 50> hpfpCompensationRpmBins[HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 42012
	 */
	uint16_t knockNoiseRpmBins[ENGINE_NOISE_CURVE_SIZE] = {};
	/**
	 * Knock sensor output knock detection threshold depending on current RPM.
	 * units: dB
	 * offset 42044
	 */
	scaled_channel<int8_t, 2, 1> knockBaseNoise[ENGINE_NOISE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 42060
	 */
	scaled_channel<uint8_t, 1, 50> tpsTspCorrValuesBins[TPS_TPS_ACCEL_RPM_CORR_TABLE] = {};
	/**
	 * units: multiplier
	 * offset 42064
	 */
	scaled_channel<uint8_t, 50, 1> tpsTspCorrValues[TPS_TPS_ACCEL_RPM_CORR_TABLE] = {};
	/**
	 * units: C
	 * offset 42068
	 */
	scaled_channel<uint8_t, 1, 1> tpsAcelEctBins[TPS_TPS_ACCEL_CLT_CORR_TABLE] = {};
	/**
	 * units: multiplier
	 * offset 42076
	 */
	scaled_channel<uint8_t, 50, 1> tpsAcelEctValues[TPS_TPS_ACCEL_CLT_CORR_TABLE] = {};
	/**
	 * units: C
	 * offset 42084
	 */
	int8_t cltRevLimitRpmBins[CLT_LIMITER_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 42088
	 */
	uint16_t cltRevLimitRpm[CLT_LIMITER_CURVE_SIZE] = {};
	/**
	 * units: volt
	 * offset 42096
	 */
	scaled_channel<uint16_t, 1000, 1> fuelLevelBins[FUEL_LEVEL_TABLE_COUNT] = {};
	/**
	 * units: %
	 * offset 42112
	 */
	uint8_t fuelLevelValues[FUEL_LEVEL_TABLE_COUNT] = {};
	/**
	 * units: volts
	 * offset 42120
	 */
	scaled_channel<uint8_t, 10, 1> dwellVoltageCorrVoltBins[DWELL_CURVE_SIZE] = {};
	/**
	 * units: multiplier
	 * offset 42128
	 */
	scaled_channel<uint8_t, 50, 1> dwellVoltageCorrValues[DWELL_CURVE_SIZE] = {};
	/**
	 * units: %
	 * offset 42136
	 */
	scaled_channel<uint8_t, 1, 1> tcu_shiftTpsBins[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42144
	 */
	uint8_t tcu_shiftSpeed12[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42152
	 */
	uint8_t tcu_shiftSpeed23[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42160
	 */
	uint8_t tcu_shiftSpeed34[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42168
	 */
	uint8_t tcu_shiftSpeed21[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42176
	 */
	uint8_t tcu_shiftSpeed32[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 42184
	 */
	uint8_t tcu_shiftSpeed43[TCU_TABLE_WIDTH] = {};
	/**
	 * units: ms
	 * offset 42192
	 */
	float tcu_shiftTime;
	/**
	 * units: Volts
	 * offset 42196
	 */
	scaled_channel<int16_t, 10, 1> alternatorVoltageTargetTable[ALTERNATOR_VOLTAGE_TARGET_SIZE][ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: Load
	 * offset 42228
	 */
	uint16_t alternatorVoltageTargetLoadBins[ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: RPM
	 * offset 42236
	 */
	uint16_t alternatorVoltageTargetRpmBins[ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: C
	 * offset 42244
	 */
	float cltBoostCorrBins[BOOST_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 42264
	 */
	float cltBoostCorr[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 42284
	 */
	float iatBoostCorrBins[BOOST_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 42304
	 */
	float iatBoostCorr[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 42324
	 */
	float cltBoostAdderBins[BOOST_CURVE_SIZE] = {};
	/**
	 * offset 42344
	 */
	float cltBoostAdder[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 42364
	 */
	float iatBoostAdderBins[BOOST_CURVE_SIZE] = {};
	/**
	 * offset 42384
	 */
	float iatBoostAdder[BOOST_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 42404
	 */
	scaled_channel<uint8_t, 1, 100> minimumOilPressureBins[8] = {};
	/**
	 * units: kPa
	 * offset 42412
	 */
	scaled_channel<uint8_t, 1, 10> minimumOilPressureValues[8] = {};
	/**
	 * offset 42420
	 */
	blend_table_s targetAfrBlends[TARGET_AFR_BLEND_COUNT] = {};
	/**
	 * units: RPM
	 * offset 42796
	 */
	scaled_channel<uint8_t, 1, 100> trimRpmBins[FUEL_TRIM_RPM_COUNT] = {};
	/**
	 * offset 42804
	 */
	uint16_t trimLoadBins[FUEL_TRIM_LOAD_COUNT] = {};
	/**
	 * @@DYNO_RPM_STEP_TOOLTIP@@
	 * units: Rpm
	 * offset 42820
	 */
	scaled_channel<uint8_t, 1, 1> dynoRpmStep;
	/**
	 * @@DYNO_SAE_TEMPERATURE_C_TOOLTIP@@
	 * units: C
	 * offset 42821
	 */
	scaled_channel<int8_t, 1, 1> dynoSaeTemperatureC;
	/**
	 * @@DYNO_SAE_RELATIVE_HUMIDITY_TOOLTIP@@
	 * units: %
	 * offset 42822
	 */
	scaled_channel<uint8_t, 1, 1> dynoSaeRelativeHumidity;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 42823
	 */
	uint8_t alignmentFill_at_42823[1] = {};
	/**
	 * @@DYNO_SAE_BARO_TOOLTIP@@
	 * units: KPa
	 * offset 42824
	 */
	scaled_channel<float, 1, 1> dynoSaeBaro;
	/**
	 * @@DYNO_CAR_WHEEL_DIA_INCH_TOOLTIP@@
	 * units: Inch
	 * offset 42828
	 */
	scaled_channel<int8_t, 1, 1> dynoCarWheelDiaInch;
	/**
	 * @@DYNO_CAR_WHEEL_ASPECT_RATIO_TOOLTIP@@
	 * units: Aspect Ratio (height)
	 * offset 42829
	 */
	scaled_channel<int8_t, 1, 1> dynoCarWheelAspectRatio;
	/**
	 * @@DYNO_CAR_WHEEL_TIRE_WIDTH_TOOLTIP@@
	 * units: Width mm
	 * offset 42830
	 */
	scaled_channel<int16_t, 1, 1> dynoCarWheelTireWidthMm;
	/**
	 * @@DYNO_CAR_GEAR_PRIMARY_REDUCTION_TOOLTIP@@
	 * units: Units
	 * offset 42832
	 */
	scaled_channel<float, 1, 1> dynoCarGearPrimaryReduction;
	/**
	 * @@DYNO_CAR_GEAR_RATIO_TOOLTIP@@
	 * units: Units
	 * offset 42836
	 */
	scaled_channel<float, 1, 1> dynoCarGearRatio;
	/**
	 * @@DYNO_CAR_GEAR_FINAL_DRIVE_TOOLTIP@@
	 * units: Units
	 * offset 42840
	 */
	scaled_channel<float, 1, 1> dynoCarGearFinalDrive;
	/**
	 * @@DYNO_CAR_CAR_MASS_TOOLTIP@@
	 * units: Kg
	 * offset 42844
	 */
	scaled_channel<int16_t, 1, 1> dynoCarCarMassKg;
	/**
	 * @@DYNO_CAR_CARGO_MASS_TOOLTIP@@
	 * units: Kg
	 * offset 42846
	 */
	scaled_channel<int16_t, 1, 1> dynoCarCargoMassKg;
	/**
	 * @@DYNO_CAR_COEFF_OF_DRAG_TOOLTIP@@
	 * units: Coeff
	 * offset 42848
	 */
	scaled_channel<float, 1, 1> dynoCarCoeffOfDrag;
	/**
	 * @@DYNO_CAR_FRONTAL_AREA_TOOLTIP@@
	 * units: m2
	 * offset 42852
	 */
	scaled_channel<float, 1, 1> dynoCarFrontalAreaM2;
	/**
	 * units: deg
	 * offset 42856
	 */
	scaled_channel<int8_t, 10, 1> trailingSparkTable[TRAILING_SPARK_SIZE][TRAILING_SPARK_SIZE] = {};
	/**
	 * units: rpm
	 * offset 42872
	 */
	scaled_channel<uint8_t, 1, 50> trailingSparkRpmBins[TRAILING_SPARK_SIZE] = {};
	/**
	 * units: Load
	 * offset 42876
	 */
	scaled_channel<uint8_t, 1, 5> trailingSparkLoadBins[TRAILING_SPARK_SIZE] = {};
	/**
	offset 42880 bit 0 */
	bool boardUseTempPullUp : 1 {};
	/**
	offset 42880 bit 1 */
	bool boardUse2stepPullDown : 1 {};
	/**
	offset 42880 bit 2 */
	bool boardUseD2PullDown : 1 {};
	/**
	offset 42880 bit 3 */
	bool boardUseD3PullDown : 1 {};
	/**
	offset 42880 bit 4 */
	bool boardUseCrankPullUp : 1 {};
	/**
	offset 42880 bit 5 */
	bool boardUseH1PullDown : 1 {};
	/**
	offset 42880 bit 6 */
	bool boardUseH2PullDown : 1 {};
	/**
	offset 42880 bit 7 */
	bool boardUseH3PullDown : 1 {};
	/**
	offset 42880 bit 8 */
	bool boardUseH4PullDown : 1 {};
	/**
	offset 42880 bit 9 */
	bool boardUseH5PullDown : 1 {};
	/**
	offset 42880 bit 10 */
	bool boardUseFlexPullDown : 1 {};
	/**
	offset 42880 bit 11 */
	bool unusedBit_337_11 : 1 {};
	/**
	offset 42880 bit 12 */
	bool unusedBit_337_12 : 1 {};
	/**
	offset 42880 bit 13 */
	bool unusedBit_337_13 : 1 {};
	/**
	offset 42880 bit 14 */
	bool unusedBit_337_14 : 1 {};
	/**
	offset 42880 bit 15 */
	bool unusedBit_337_15 : 1 {};
	/**
	offset 42880 bit 16 */
	bool unusedBit_337_16 : 1 {};
	/**
	offset 42880 bit 17 */
	bool unusedBit_337_17 : 1 {};
	/**
	offset 42880 bit 18 */
	bool unusedBit_337_18 : 1 {};
	/**
	offset 42880 bit 19 */
	bool unusedBit_337_19 : 1 {};
	/**
	offset 42880 bit 20 */
	bool unusedBit_337_20 : 1 {};
	/**
	offset 42880 bit 21 */
	bool unusedBit_337_21 : 1 {};
	/**
	offset 42880 bit 22 */
	bool unusedBit_337_22 : 1 {};
	/**
	offset 42880 bit 23 */
	bool unusedBit_337_23 : 1 {};
	/**
	offset 42880 bit 24 */
	bool unusedBit_337_24 : 1 {};
	/**
	offset 42880 bit 25 */
	bool unusedBit_337_25 : 1 {};
	/**
	offset 42880 bit 26 */
	bool unusedBit_337_26 : 1 {};
	/**
	offset 42880 bit 27 */
	bool unusedBit_337_27 : 1 {};
	/**
	offset 42880 bit 28 */
	bool unusedBit_337_28 : 1 {};
	/**
	offset 42880 bit 29 */
	bool unusedBit_337_29 : 1 {};
	/**
	offset 42880 bit 30 */
	bool unusedBit_337_30 : 1 {};
	/**
	offset 42880 bit 31 */
	bool unusedBit_337_31 : 1 {};
};
static_assert(sizeof(persistent_config_s) == 42884);

// end
// this section was generated automatically by rusEFI tool config_definition-all.jar based on gen_config.sh integration/evotech_config.txt
