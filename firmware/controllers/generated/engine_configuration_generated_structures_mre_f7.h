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
	 * offset 1304
	 */
	gppwm_note_t gpPwmNote[GPPWM_CHANNELS] = {};
	/**
	 * units: ADC
	 * offset 1368
	 */
	uint16_t tps2SecondaryMin;
	/**
	 * units: ADC
	 * offset 1370
	 */
	uint16_t tps2SecondaryMax;
	/**
	 * Select which bus the wideband controller is attached to.
	offset 1372 bit 0 */
	bool widebandOnSecondBus : 1 {};
	/**
	 * Enables lambda sensor closed loop feedback for fuelling.
	offset 1372 bit 1 */
	bool fuelClosedLoopCorrectionEnabled : 1 {};
	/**
	 * Write SD card log even when powered by USB
	offset 1372 bit 2 */
	bool alwaysWriteSdCard : 1 {};
	/**
	 * Second harmonic (aka double) is usually quieter background noise
	offset 1372 bit 3 */
	bool knockDetectionUseDoubleFrequency : 1 {};
	/**
	offset 1372 bit 4 */
	bool yesUnderstandLocking : 1 {};
	/**
	 * Sometimes we have a performance issue while printing error
	offset 1372 bit 5 */
	bool silentTriggerError : 1 {};
	/**
	offset 1372 bit 6 */
	bool useLinearCltSensor : 1 {};
	/**
	 * enable can_read/disable can_read
	offset 1372 bit 7 */
	bool canReadEnabled : 1 {};
	/**
	 * enable can_write/disable can_write. See also can1ListenMode
	offset 1372 bit 8 */
	bool canWriteEnabled : 1 {};
	/**
	offset 1372 bit 9 */
	bool useLinearIatSensor : 1 {};
	/**
	offset 1372 bit 10 */
	bool enableOilPressureProtect : 1 {};
	/**
	 * Treat milliseconds value as duty cycle value, i.e. 0.5ms would become 50%
	offset 1372 bit 11 */
	bool tachPulseDurationAsDutyCycle : 1 {};
	/**
	 * This enables smart alternator control and activates the extra alternator settings.
	offset 1372 bit 12 */
	bool isAlternatorControlEnabled : 1 {};
	/**
	 * https://wiki.rusefi.com/Trigger-Configuration-Guide
	 * This setting flips the signal from the primary engine speed sensor.
	offset 1372 bit 13 */
	bool invertPrimaryTriggerSignal : 1 {};
	/**
	 * https://wiki.rusefi.com/Trigger-Configuration-Guide
	 * This setting flips the signal from the secondary engine speed sensor.
	offset 1372 bit 14 */
	bool invertSecondaryTriggerSignal : 1 {};
	/**
	offset 1372 bit 15 */
	bool cutFuelOnHardLimit : 1 {};
	/**
	 * Be careful enabling this: some engines are known to self-disassemble their valvetrain with a spark cut. Fuel cut is much safer.
	offset 1372 bit 16 */
	bool cutSparkOnHardLimit : 1 {};
	/**
	offset 1372 bit 17 */
	bool launchFuelCutEnable : 1 {};
	/**
	 * This is the Cut Mode normally used
	offset 1372 bit 18 */
	bool launchSparkCutEnable : 1 {};
	/**
	offset 1372 bit 19 */
	bool torqueReductionEnabled : 1 {};
	/**
	offset 1372 bit 20 */
	bool unused1308_20 : 1 {};
	/**
	offset 1372 bit 21 */
	bool limitTorqueReductionTime : 1 {};
	/**
	 * Are you a developer troubleshooting TS over CAN ISO/TP?
	offset 1372 bit 22 */
	bool verboseIsoTp : 1 {};
	/**
	offset 1372 bit 23 */
	bool engineSnifferFocusOnInputs : 1 {};
	/**
	offset 1372 bit 24 */
	bool unused1308_24 : 1 {};
	/**
	offset 1372 bit 25 */
	bool twoStroke : 1 {};
	/**
	 * Where is your primary skipped wheel located?
	offset 1372 bit 26 */
	bool skippedWheelOnCam : 1 {};
	/**
	offset 1372 bit 27 */
	bool unusedBit_593_27 : 1 {};
	/**
	offset 1372 bit 28 */
	bool unusedBit_593_28 : 1 {};
	/**
	offset 1372 bit 29 */
	bool unusedBit_593_29 : 1 {};
	/**
	offset 1372 bit 30 */
	bool unusedBit_593_30 : 1 {};
	/**
	offset 1372 bit 31 */
	bool unusedBit_593_31 : 1 {};
	/**
	 * offset 1376
	 */
	adc_channel_e hipOutputChannel;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1377
	 */
	uint8_t alignmentFill_at_1377[1] = {};
	/**
	 * A/C button input
	 * offset 1378
	 */
	switch_input_pin_e acSwitch;
	/**
	 * offset 1380
	 */
	adc_channel_e vRefAdcChannel;
	/**
	 * Posicao neutra esperada
	 * units: %
	 * offset 1381
	 */
	uint8_t etbNeutralPosition;
	/**
	 * See also idleRpmPid
	 * offset 1382
	 */
	idle_mode_e idleMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1383
	 */
	uint8_t alignmentFill_at_1383[1] = {};
	/**
	offset 1384 bit 0 */
	bool isInjectionEnabled : 1 {};
	/**
	offset 1384 bit 1 */
	bool isIgnitionEnabled : 1 {};
	/**
	 * When enabled if TPS is held above 95% no fuel is injected while cranking to clear excess fuel from the cylinders.
	offset 1384 bit 2 */
	bool isCylinderCleanupEnabled : 1 {};
	/**
	 * Should we use tables to vary tau/beta based on CLT/MAP, or just with fixed values?
	offset 1384 bit 3 */
	bool complexWallModel : 1 {};
	/**
	 * RPM is measured based on last 720 degrees while instant RPM is measured based on the last 90 degrees of crank revolution
	offset 1384 bit 4 */
	bool alwaysInstantRpm : 1 {};
	/**
	offset 1384 bit 5 */
	bool isMapAveragingEnabled : 1 {};
	/**
	 * If enabled, use separate temperature multiplier table for cranking idle position.
	 * If disabled, use normal running multiplier table applied to the cranking base position.
	offset 1384 bit 6 */
	bool unusedOverrideCrankingIacSetting : 1 {};
	/**
	 * This activates a separate ignition timing table for idle conditions, this can help idle stability by using ignition retard and advance either side of the desired idle speed. Extra advance at low idle speeds will prevent stalling and extra retard at high idle speeds can help reduce engine power and slow the idle speed.
	offset 1384 bit 7 */
	bool useSeparateAdvanceForIdle : 1 {};
	/**
	offset 1384 bit 8 */
	bool isWaveAnalyzerEnabled : 1 {};
	/**
	 * This activates a separate fuel table for Idle, this allows fine tuning of the idle fuelling.
	offset 1384 bit 9 */
	bool useSeparateVeForIdle : 1 {};
	/**
	 * Verbose info in console below engineSnifferRpmThreshold
	 * enable trigger_details
	offset 1384 bit 10 */
	bool verboseTriggerSynchDetails : 1 {};
	/**
	offset 1384 bit 11 */
	bool unusedCutFuelInAcr : 1 {};
	/**
	offset 1384 bit 12 */
	bool hondaK : 1 {};
	/**
	 * This is needed if your coils are individually wired (COP) and you wish to use batch ignition (Wasted Spark).
	offset 1384 bit 13 */
	bool twoWireBatchIgnition : 1 {};
	/**
	 * Read MAP sensor on ECU start-up to use as baro value.
	offset 1384 bit 14 */
	bool useFixedBaroCorrFromMap : 1 {};
	/**
	 * In Constant mode, timing is automatically tapered to running as RPM increases.
	 * In Table mode, the "Cranking ignition advance" table is used directly.
	offset 1384 bit 15 */
	bool useSeparateAdvanceForCranking : 1 {};
	/**
	 * This enables the various ignition corrections during cranking (IAT, CLT and PID idle).
	 * You probably don't need this.
	offset 1384 bit 16 */
	bool useAdvanceCorrectionsForCranking : 1 {};
	/**
	 * Enable a second cranking table to use for E100 flex fuel, interpolating between the two based on flex fuel sensor.
	offset 1384 bit 17 */
	bool flexCranking : 1 {};
	/**
	 * This flag allows to use a special 'PID Multiplier' table (0.0-1.0) to compensate for nonlinear nature of IAC-RPM controller
	offset 1384 bit 18 */
	bool useIacPidMultTable : 1 {};
	/**
	offset 1384 bit 19 */
	bool isBoostControlEnabled : 1 {};
	/**
	 * Interpolates the Ignition Retard from 0 to 100% within the RPM Range
	offset 1384 bit 20 */
	bool launchSmoothRetard : 1 {};
	/**
	 * Some engines are OK running semi-random sequential while other engine require phase synchronization
	offset 1384 bit 21 */
	bool isPhaseSyncRequiredForIgnition : 1 {};
	/**
	 * If enabled, use a curve for RPM limit (based on coolant temperature) instead of a constant value.
	offset 1384 bit 22 */
	bool useCltBasedRpmLimit : 1 {};
	/**
	 * If enabled, don't wait for engine start to heat O2 sensors.
	 * WARNING: this will reduce the life of your sensor, as condensation in the exhaust from a cold start can crack the sensing element.
	offset 1384 bit 23 */
	bool forceO2Heating : 1 {};
	/**
	 * If increased VVT duty cycle increases the indicated VVT angle, set this to 'advance'. If it decreases, set this to 'retard'. Most intake cams use 'advance', and most exhaust cams use 'retard'.
	offset 1384 bit 24 */
	bool invertVvtControlIntake : 1 {};
	/**
	 * If increased VVT duty cycle increases the indicated VVT angle, set this to 'advance'. If it decreases, set this to 'retard'. Most intake cams use 'advance', and most exhaust cams use 'retard'.
	offset 1384 bit 25 */
	bool invertVvtControlExhaust : 1 {};
	/**
	offset 1384 bit 26 */
	bool useBiQuadOnAuxSpeedSensors : 1 {};
	/**
	 * 'Trigger' mode will write a high speed log of trigger events (warning: uses lots of space!). 'Normal' mode will write a standard MLG of sensors, engine function, etc. similar to the one captured in TunerStudio.
	offset 1384 bit 27 */
	bool sdTriggerLog : 1 {};
	/**
	offset 1384 bit 28 */
	bool unused1320_28 : 1 {};
	/**
	offset 1384 bit 29 */
	bool stepper_dc_use_two_wires : 1 {};
	/**
	offset 1384 bit 30 */
	bool watchOutForLinearTime : 1 {};
	/**
	offset 1384 bit 31 */
	bool unusedBit_636_31 : 1 {};
	/**
	 * units: count
	 * offset 1388
	 */
	uint32_t engineChartSize;
	/**
	 * units: mult
	 * offset 1392
	 */
	float turboSpeedSensorMultiplier;
	/**
	 * offset 1396
	 */
	Gpio camInputsDebug[CAM_INPUTS_COUNT] = {};
	/**
	 * Idle target speed when A/C is enabled. Some cars need the extra speed to keep the AC efficient while idling.
	 * units: RPM
	 * offset 1404
	 */
	int16_t acIdleRpmTarget;
	/**
	 * definir warningPeriod X
	 * units: seconds
	 * offset 1406
	 */
	int16_t warningPeriod;
	/**
	 * units: angle
	 * offset 1408
	 */
	float knockDetectionWindowStart;
	/**
	 * units: ms
	 * offset 1412
	 */
	float idleStepperReactionTime;
	/**
	 * units: count
	 * offset 1416
	 */
	int idleStepperTotalSteps;
	/**
	 * Posicao do pedal para perceber que precisamos reduzir o torque quando o pino de trigger e acionado
	 * offset 1420
	 */
	int torqueReductionArmingApp;
	/**
	 * Duracao em ms ou ciclo de trabalho dependendo do modo selecionado
	 * offset 1424
	 */
	float tachPulseDuractionMs;
	/**
	 * Length of time the deposited wall fuel takes to dissipate after the start of acceleration.
	 * units: Seconds
	 * offset 1428
	 */
	float wwaeTau;
	/**
	 * offset 1432
	 */
	pid_s alternatorControl;
	/**
	 * offset 1452
	 */
	pid_s etb;
	/**
	 * offset 1472
	 */
	Gpio triggerInputDebugPins[TRIGGER_INPUT_PIN_COUNT] = {};
	/**
	 * Faixa de RPM acima do limite superior para afunilamento de ar extra
	 * units: RPM
	 * offset 1476
	 */
	int16_t airTaperRpmRange;
	/**
	 * offset 1478
	 */
	brain_input_pin_e turboSpeedSensorInputPin;
	/**
	 * Closed throttle#2. todo: extract these two fields into a structure
	 * See also tps2_1AdcChannel
	 * units: ADC
	 * offset 1480
	 */
	int16_t tps2Min;
	/**
	 * Full throttle#2. tpsMax value as 10 bit ADC value. Not Voltage!
	 * See also tps1_1AdcChannel
	 * units: ADC
	 * offset 1482
	 */
	int16_t tps2Max;
	/**
	 * See also startStopButtonPin
	 * offset 1484
	 */
	output_pin_e starterControlPin;
	/**
	 * offset 1486
	 */
	pin_input_mode_e startStopButtonMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1487
	 */
	uint8_t alignmentFill_at_1487[1] = {};
	/**
	 * offset 1488
	 */
	Gpio mc33816_flag0;
	/**
	 * offset 1490
	 */
	scaled_channel<uint16_t, 1000, 1> tachPulsePerRev;
	/**
	 * Valor kPa que e muito baixo para ser verdadeiro
	 * units: kPa
	 * offset 1492
	 */
	float mapErrorDetectionTooLow;
	/**
	 * Valor kPa que e muito alto para ser verdadeiro
	 * units: kPa
	 * offset 1496
	 */
	float mapErrorDetectionTooHigh;
	/**
	 * How long to wait for the spark to fire before recharging the coil for another spark.
	 * units: ms
	 * offset 1500
	 */
	scaled_channel<uint16_t, 1000, 1> multisparkSparkDuration;
	/**
	 * This sets the dwell time for subsequent sparks. The main spark's dwell is set by the dwell table.
	 * units: ms
	 * offset 1502
	 */
	scaled_channel<uint16_t, 1000, 1> multisparkDwell;
	/**
	 * See cltIdleRpmBins
	 * offset 1504
	 */
	pid_s idleRpmPid;
	/**
	 * 0 = No fuel settling on port walls 1 = All the fuel settling on port walls setting this to 0 disables the wall wetting enrichment.
	 * units: Fraction
	 * offset 1524
	 */
	float wwaeBeta;
	/**
	 * See also EFI_CONSOLE_RX_BRAIN_PIN
	 * offset 1528
	 */
	Gpio binarySerialTxPin;
	/**
	 * offset 1530
	 */
	Gpio binarySerialRxPin;
	/**
	 * offset 1532
	 */
	Gpio auxValves[AUX_DIGITAL_VALVE_COUNT] = {};
	/**
	 * offset 1536
	 */
	switch_input_pin_e tcuUpshiftButtonPin;
	/**
	 * offset 1538
	 */
	switch_input_pin_e tcuDownshiftButtonPin;
	/**
	 * units: voltage
	 * offset 1540
	 */
	float throttlePedalUpVoltage;
	/**
	 * Pedal no chao
	 * units: voltage
	 * offset 1544
	 */
	float throttlePedalWOTVoltage;
	/**
	 * na deteccao de voltagem IGN ligar bomba de combustivel para construir pressao de combustivel
	 * units: seconds
	 * offset 1548
	 */
	int16_t startUpFuelPumpDuration;
	/**
	 * If the RPM closer to target than this value, disable closed loop idle correction to prevent oscillation
	 * units: RPM
	 * offset 1550
	 */
	int16_t idlePidRpmDeadZone;
	/**
	 * Maximum commanded airmass for the idle controller.
	 * units: mg
	 * offset 1552
	 */
	scaled_channel<uint8_t, 1, 2> idleMaximumAirmass;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1553
	 */
	uint8_t alignmentFill_at_1553[3] = {};
	/**
	 * For how long after the pin has been triggered will the cut/reduction stay active. After that, even if the pin is still triggered, torque is re-introduced
	 * units: ms
	 * offset 1556
	 */
	float torqueReductionTime;
	/**
	 * See Over/Undervoltage Shutdown/Retry bit in documentation
	offset 1560 bit 0 */
	bool mc33810DisableRecoveryMode : 1 {};
	/**
	offset 1560 bit 1 */
	bool mc33810Gpgd0Mode : 1 {};
	/**
	offset 1560 bit 2 */
	bool mc33810Gpgd1Mode : 1 {};
	/**
	offset 1560 bit 3 */
	bool mc33810Gpgd2Mode : 1 {};
	/**
	offset 1560 bit 4 */
	bool mc33810Gpgd3Mode : 1 {};
	/**
	 * Send out board statistics
	offset 1560 bit 5 */
	bool enableExtendedCanBroadcast : 1 {};
	/**
	 * global_can_data performance hack
	offset 1560 bit 6 */
	bool luaCanRxWorkaround : 1 {};
	/**
	offset 1560 bit 7 */
	bool flexSensorInverted : 1 {};
	/**
	offset 1560 bit 8 */
	bool useHardSkipInTraction : 1 {};
	/**
	 * Use Aux Speed 1 as one of speeds for wheel slip ratio?
	offset 1560 bit 9 */
	bool useAuxSpeedForSlipRatio : 1 {};
	/**
	 * VSS and Aux Speed 1 or Aux Speed 1 with Aux Speed 2?
	offset 1560 bit 10 */
	bool useVssAsSecondWheelSpeed : 1 {};
	/**
	offset 1560 bit 11 */
	bool is_enabled_spi_5 : 1 {};
	/**
	offset 1560 bit 12 */
	bool is_enabled_spi_6 : 1 {};
	/**
	 * AEM X-Series EGT gauge kit or evoTech EGT sensor from Wideband controller
	offset 1560 bit 13 */
	bool enableAemXSeriesEgt : 1 {};
	/**
	offset 1560 bit 14 */
	bool startRequestPinInverted : 1 {};
	/**
	offset 1560 bit 15 */
	bool tcu_rangeSensorPulldown : 1 {};
	/**
	offset 1560 bit 16 */
	bool devBit01 : 1 {};
	/**
	offset 1560 bit 17 */
	bool devBit0 : 1 {};
	/**
	offset 1560 bit 18 */
	bool devBit1 : 1 {};
	/**
	offset 1560 bit 19 */
	bool devBit2 : 1 {};
	/**
	offset 1560 bit 20 */
	bool devBit3 : 1 {};
	/**
	offset 1560 bit 21 */
	bool devBit4 : 1 {};
	/**
	offset 1560 bit 22 */
	bool devBit5 : 1 {};
	/**
	offset 1560 bit 23 */
	bool devBit6 : 1 {};
	/**
	offset 1560 bit 24 */
	bool devBit7 : 1 {};
	/**
	offset 1560 bit 25 */
	bool invertExhaustCamVVTSignal : 1 {};
	/**
	 * "Available via TS Plugin see https://rusefi.com/s/knock"
	offset 1560 bit 26 */
	bool enableKnockSpectrogram : 1 {};
	/**
	offset 1560 bit 27 */
	bool enableKnockSpectrogramFilter : 1 {};
	/**
	offset 1560 bit 28 */
	bool unusedBit_706_28 : 1 {};
	/**
	offset 1560 bit 29 */
	bool unusedBit_706_29 : 1 {};
	/**
	offset 1560 bit 30 */
	bool unusedBit_706_30 : 1 {};
	/**
	offset 1560 bit 31 */
	bool unusedBit_706_31 : 1 {};
	/**
	 * This is the duration in cycles that the IAC will take to reach its normal idle position, it can be used to hold the idle higher for a few seconds after cranking to improve startup.
	 * Should be 100 once tune is better
	 * units: cycles
	 * offset 1564
	 */
	int16_t afterCrankingIACtaperDuration;
	/**
	 * This value is an added for base idle value. Idle Value added when coasting and transitioning into idle.
	 * units: percent
	 * offset 1566
	 */
	int16_t iacByTpsTaper;
	/**
	 * offset 1568
	 */
	Gpio accelerometerCsPin;
	/**
	 * Below this speed, disable DFCO. Use this to prevent jerkiness from fuel enable/disable in low gears.
	 * units: kph
	 * offset 1570
	 */
	uint8_t coastingFuelCutVssLow;
	/**
	 * Above this speed, allow DFCO. Use this to prevent jerkiness from fuel enable/disable in low gears.
	 * units: kph
	 * offset 1571
	 */
	uint8_t coastingFuelCutVssHigh;
	/**
	 * Maximum change delta of TPS percentage over the 'length'. Actual TPS change has to be above this value in order for TPS/TPS acceleration to kick in.
	 * units: roc
	 * offset 1572
	 */
	float tpsAccelEnrichmentThreshold;
	/**
	 * offset 1576
	 */
	brain_input_pin_e auxSpeedSensorInputPin[AUX_SPEED_SENSOR_COUNT] = {};
	/**
	 * offset 1580
	 */
	uint8_t totalGearsCount;
	/**
	 * Sets what part of injection's is controlled by the injection phase table.
	 * offset 1581
	 */
	InjectionTimingMode injectionTimingMode;
	/**
	 * See http://rusefi.com/s/debugmode
	 * offset 1582
	 */
	debug_mode_e debugMode;
	/**
	 * Additional idle % when fan #1 is active
	 * units: %
	 * offset 1583
	 */
	uint8_t fan1ExtraIdle;
	/**
	 * Taxa de banda para TTL primario
	 * units: BPs
	 * offset 1584
	 */
	uint32_t uartConsoleSerialSpeed;
	/**
	 * For decel we simply multiply delta of TPS and tFor decel we do not use table?!
	 * units: roc
	 * offset 1588
	 */
	float tpsDecelEnleanmentThreshold;
	/**
	 * Magic multiplier, we multiply delta of TPS and get fuel squirt duration
	 * units: coeff
	 * offset 1592
	 */
	float tpsDecelEnleanmentMultiplier;
	/**
	 * Quantos graus de avanco de tempo serao reduzidos durante o Tempo de Reducao de Torque
	 * units: deg
	 * offset 1596
	 */
	float torqueReductionIgnitionRetard;
	/**
	 * units: voltage
	 * offset 1600
	 */
	float throttlePedalSecondaryUpVoltage;
	/**
	 * Pedal no chao
	 * units: voltage
	 * offset 1604
	 */
	float throttlePedalSecondaryWOTVoltage;
	/**
	 * offset 1608
	 */
	can_baudrate_e canBaudRate;
	/**
	 * Override the Y axis (load) value used for the VE table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1609
	 */
	ve_override_e veOverrideMode;
	/**
	 * offset 1610
	 */
	can_baudrate_e can2BaudRate;
	/**
	 * Override the Y axis (load) value used for the AFR table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1611
	 */
	load_override_e afrOverrideMode;
	/**
	 * units: A
	 * offset 1612
	 */
	scaled_channel<uint8_t, 10, 1> mc33_hpfp_i_peak;
	/**
	 * units: A
	 * offset 1613
	 */
	scaled_channel<uint8_t, 10, 1> mc33_hpfp_i_hold;
	/**
	 * Quanto tempo desativar a energia quando a corrente de manutencao e atingida antes de aplicar energia novamente
	 * units: us
	 * offset 1614
	 */
	uint8_t mc33_hpfp_i_hold_off;
	/**
	 * Quantidade maxima de tempo que o solenoide pode estar ativo antes de assumir um erro de programacao
	 * units: ms
	 * offset 1615
	 */
	uint8_t mc33_hpfp_max_hold;
	/**
	 * Habilite se o driver de motor DC (ponte H) inverter os sinais (ex: RZ7899 em placas Hellen)
	offset 1616 bit 0 */
	bool stepperDcInvertedPins : 1 {};
	/**
	 * Permitir OpenBLT no CAN Primário
	offset 1616 bit 1 */
	bool canOpenBLT : 1 {};
	/**
	 * Permitir OpenBLT no CAN Secundário
	offset 1616 bit 2 */
	bool can2OpenBLT : 1 {};
	/**
	 * Selecione se deseja configurar o fluxo do injetor em fluxo volumétrico (padrão, cc/min) ou fluxo de massa (g/s).
	offset 1616 bit 3 */
	bool injectorFlowAsMassFlow : 1 {};
	/**
	offset 1616 bit 4 */
	bool boardUseCanTerminator : 1 {};
	/**
	offset 1616 bit 5 */
	bool kLineDoHondaSend : 1 {};
	/**
	 * ListenMode é sobre reconhecer tráfego CAN no nível do protocolo. Diferente de canWriteEnabled
	offset 1616 bit 6 */
	bool can1ListenMode : 1 {};
	/**
	offset 1616 bit 7 */
	bool can2ListenMode : 1 {};
	/**
	offset 1616 bit 8 */
	bool unusedBit_743_8 : 1 {};
	/**
	offset 1616 bit 9 */
	bool unusedBit_743_9 : 1 {};
	/**
	offset 1616 bit 10 */
	bool unusedBit_743_10 : 1 {};
	/**
	offset 1616 bit 11 */
	bool unusedBit_743_11 : 1 {};
	/**
	offset 1616 bit 12 */
	bool unusedBit_743_12 : 1 {};
	/**
	offset 1616 bit 13 */
	bool unusedBit_743_13 : 1 {};
	/**
	offset 1616 bit 14 */
	bool unusedBit_743_14 : 1 {};
	/**
	offset 1616 bit 15 */
	bool unusedBit_743_15 : 1 {};
	/**
	offset 1616 bit 16 */
	bool unusedBit_743_16 : 1 {};
	/**
	offset 1616 bit 17 */
	bool unusedBit_743_17 : 1 {};
	/**
	offset 1616 bit 18 */
	bool unusedBit_743_18 : 1 {};
	/**
	offset 1616 bit 19 */
	bool unusedBit_743_19 : 1 {};
	/**
	offset 1616 bit 20 */
	bool unusedBit_743_20 : 1 {};
	/**
	offset 1616 bit 21 */
	bool unusedBit_743_21 : 1 {};
	/**
	offset 1616 bit 22 */
	bool unusedBit_743_22 : 1 {};
	/**
	offset 1616 bit 23 */
	bool unusedBit_743_23 : 1 {};
	/**
	offset 1616 bit 24 */
	bool unusedBit_743_24 : 1 {};
	/**
	offset 1616 bit 25 */
	bool unusedBit_743_25 : 1 {};
	/**
	offset 1616 bit 26 */
	bool unusedBit_743_26 : 1 {};
	/**
	offset 1616 bit 27 */
	bool unusedBit_743_27 : 1 {};
	/**
	offset 1616 bit 28 */
	bool unusedBit_743_28 : 1 {};
	/**
	offset 1616 bit 29 */
	bool unusedBit_743_29 : 1 {};
	/**
	offset 1616 bit 30 */
	bool unusedBit_743_30 : 1 {};
	/**
	offset 1616 bit 31 */
	bool unusedBit_743_31 : 1 {};
	/**
	 * offset 1620
	 */
	uint8_t camDecoder2jzPosition;
	/**
	 * offset 1621
	 */
	mc33810maxDwellTimer_e mc33810maxDwellTimer;
	/**
	 * Duração de cada pulso de teste
	 * units: ms
	 * offset 1622
	 */
	scaled_channel<uint16_t, 100, 1> benchTestOnTime;
	/**
	 * units: %
	 * offset 1624
	 */
	uint8_t lambdaProtectionRestoreTps;
	/**
	 * units: %
	 * offset 1625
	 */
	scaled_channel<uint8_t, 1, 10> lambdaProtectionRestoreLoad;
	/**
	 * offset 1626
	 */
	pin_input_mode_e launchActivatePinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1627
	 */
	uint8_t alignmentFill_at_1627[1] = {};
	/**
	 * offset 1628
	 */
	Gpio can2TxPin;
	/**
	 * offset 1630
	 */
	Gpio can2RxPin;
	/**
	 * offset 1632
	 */
	pin_output_mode_e starterControlPinMode;
	/**
	 * offset 1633
	 */
	adc_channel_e wastegatePositionSensor;
	/**
	 * Override the Y axis (load) value used for the ignition table.
	 * Advanced users only: If you aren't sure you need this, you probably don't need this.
	 * offset 1634
	 */
	load_override_e ignOverrideMode;
	/**
	 * Select which fuel pressure sensor measures the pressure of the fuel at your injectors.
	 * offset 1635
	 */
	injector_pressure_type_e injectorPressureType;
	/**
	 * offset 1636
	 */
	output_pin_e hpfpValvePin;
	/**
	 * offset 1638
	 */
	pin_output_mode_e hpfpValvePinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 1639
	 */
	uint8_t alignmentFill_at_1639[1] = {};
	/**
	 * Valor de MAP acima do qual o combustível é cortado em caso de overboost. Defina como 0 para desabilitar o corte de overboost.
	 * units: kPa (absoluto)
	 * offset 1640
	 */
	float boostCutPressure;
	/**
	 * units: kg/h
	 * offset 1644
	 */
	scaled_channel<uint8_t, 1, 5> tchargeBins[16] = {};
	/**
	 * units: ratio
	 * offset 1660
	 */
	scaled_channel<uint8_t, 100, 1> tchargeValues[16] = {};
	/**
	 * Fixed timing, useful for TDC testing
	 * units: deg
	 * offset 1676
	 */
	float fixedTiming;
	/**
	 * MAP voltage for low point
	 * units: v
	 * offset 1680
	 */
	float mapLowValueVoltage;
	/**
	 * MAP voltage for low point
	 * units: v
	 * offset 1684
	 */
	float mapHighValueVoltage;
	/**
	 * EGO value correction
	 * units: value
	 * offset 1688
	 */
	float egoValueShift;
	/**
	 * VVT output solenoid pin for this cam
	 * offset 1692
	 */
	output_pin_e vvtPins[CAM_INPUTS_COUNT] = {};
	/**
	 * Esta é a posição do IAC durante a partida, alguns motores dão partida melhor se receberem mais ar durante a partida para melhorar o enchimento dos cilindros.
	 * units: percent
	 * offset 1700
	 */
	int crankingIACposition;
	/**
	 * offset 1704
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMinRpmMinTps;
	/**
	 * offset 1705
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMinRpmMaxTps;
	/**
	 * offset 1706
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMaxRpmMinTps;
	/**
	 * offset 1707
	 */
	scaled_channel<uint8_t, 200, 1> tChargeMaxRpmMaxTps;
	/**
	 * offset 1708
	 */
	pwm_freq_t vvtOutputFrequency;
	/**
	 * Minimim timing advance allowed. No spark on any cylinder will ever fire after this angle BTDC. For example, setting -10 here means no spark ever fires later than 10 deg ATDC. Note that this only concerns the primary spark: any trailing sparks or multispark may violate this constraint.
	 * units: deg BTDC
	 * offset 1710
	 */
	int8_t minimumIgnitionTiming;
	/**
	 * Maximum timing advance allowed. No spark on any cylinder will ever fire before this angle BTDC. For example, setting 45 here means no spark ever fires earlier than 45 deg BTDC
	 * units: deg BTDC
	 * offset 1711
	 */
	int8_t maximumIgnitionTiming;
	/**
	 * units: Hz
	 * offset 1712
	 */
	int alternatorPwmFrequency;
	/**
	 * defina vvt_mode X
	 * offset 1716
	 */
	vvt_mode_e vvtMode[CAMS_PER_BANK] = {};
	/**
	 * Marcha lenta adicional % quando o ventilador #2 está ativo
	 * units: %
	 * offset 1718
	 */
	uint8_t fan2ExtraIdle;
	/**
	 * Atraso para permitir que a pressão de combustível se estabilize antes de disparar o pulso de priming.
	 * units: seg
	 * offset 1719
	 */
	scaled_channel<uint8_t, 100, 1> primingDelay;
	/**
	 * offset 1720
	 */
	adc_channel_e auxAnalogInputs[LUA_ANALOG_INPUT_COUNT] = {};
	/**
	 * offset 1728
	 */
	output_pin_e trailingCoilPins[MAX_CYLINDER_COUNT] = {};
	/**
	 * offset 1752
	 */
	tle8888_mode_e tle8888mode;
	/**
	 * offset 1753
	 */
	pin_output_mode_e accelerometerCsPinMode;
	/**
	 * Nenhum = regulador de pressão de combustível referenciado ao MAP
	 * Pressão fixa = regulador de pressão referenciado à atmosfera (sem retorno, tipicamente)
	 * Pressão sensorizada = sensor de pressão de combustível
	 * offset 1754
	 */
	injector_compensation_mode_e injectorCompensationMode;
	/**
	 * offset 1755
	 */
	pin_output_mode_e fan2PinMode;
	/**
	 * Esta é a pressão na qual o fluxo do seu injetor é conhecido.
	 * Por exemplo, se seus injetores fluem 400cc/min a 3,5 bar, insira 350kpa aqui.
	 * units: kPa
	 * offset 1756
	 */
	float fuelReferencePressure;
	/**
	 * offset 1760
	 */
	ThermistorConf auxTempSensor1;
	/**
	 * offset 1792
	 */
	ThermistorConf auxTempSensor2;
	/**
	 * units: Deg
	 * offset 1824
	 */
	int16_t knockSamplingDuration;
	/**
	 * units: Hz
	 * offset 1826
	 */
	int16_t etbFreq;
	/**
	 * offset 1828
	 */
	pid_s etbWastegatePid;
	/**
	 * Para micro-stepping, certifique-se que a frequência PWM (etbFreq) é alta o suficiente
	 * offset 1848
	 */
	stepper_num_micro_steps_e stepperNumMicroSteps;
	/**
	 * Use para limitar a corrente quando o motor de passo está parado, não se movendo (100% = no limit)
	 * units: %
	 * offset 1849
	 */
	uint8_t stepperMinDutyCycle;
	/**
	 * Use para limitar a corrente máxima através do motor de passo (100% = no limit)
	 * units: %
	 * offset 1850
	 */
	uint8_t stepperMaxDutyCycle;
	/**
	 * offset 1851
	 */
	spi_device_e sdCardSpiDevice;
	/**
	 * correção de tempo de ignição e injeção por cilindro para motores irregulares
	 * units: deg
	 * offset 1852
	 */
	angle_t timing_offset_cylinder[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: segundos
	 * offset 1900
	 */
	float idlePidActivationTime;
	/**
	 * offset 1904
	 */
	pin_mode_e spi1SckMode;
	/**
	 * Modos podem ser usados para integração 3v<>5v usando pull-ups/pull-downs etc.
	 * offset 1905
	 */
	pin_mode_e spi1MosiMode;
	/**
	 * offset 1906
	 */
	pin_mode_e spi1MisoMode;
	/**
	 * offset 1907
	 */
	pin_mode_e spi2SckMode;
	/**
	 * offset 1908
	 */
	pin_mode_e spi2MosiMode;
	/**
	 * offset 1909
	 */
	pin_mode_e spi2MisoMode;
	/**
	 * offset 1910
	 */
	pin_mode_e spi3SckMode;
	/**
	 * offset 1911
	 */
	pin_mode_e spi3MosiMode;
	/**
	 * offset 1912
	 */
	pin_mode_e spi3MisoMode;
	/**
	 * offset 1913
	 */
	pin_output_mode_e stepperEnablePinMode;
	/**
	 * ResetB
	 * offset 1914
	 */
	Gpio mc33816_rstb;
	/**
	 * offset 1916
	 */
	Gpio mc33816_driven;
	/**
	 * Interruptor do pedal de freio
	 * offset 1918
	 */
	switch_input_pin_e brakePedalPin;
	/**
	 * PID de saída VVT
	 * TODO: renomear para vvtPid
	 * offset 1920
	 */
	pid_s auxPid[CAMS_PER_BANK] = {};
	/**
	 * offset 1960
	 */
	float injectorCorrectionPolynomial[8] = {};
	/**
	 * units: C
	 * offset 1992
	 */
	int8_t primeBins[PRIME_CURVE_COUNT] = {};
	/**
	 * offset 2000
	 */
	linear_sensor_s oilPressure;
	/**
	 * offset 2020
	 */
	spi_device_e accelerometerSpiDevice;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2021
	 */
	uint8_t alignmentFill_at_2021[1] = {};
	/**
	 * offset 2022
	 */
	output_pin_e fan2Pin;
	/**
	 * Limite de temperatura para ligar o ventilador de resfriamento #2, em Celsius
	 * units: deg C
	 * offset 2024
	 */
	uint8_t fan2OnTemperature;
	/**
	 * Limite de temperatura para desligar o ventilador de resfriamento #2, em Celsius
	 * units: deg C
	 * offset 2025
	 */
	uint8_t fan2OffTemperature;
	/**
	 * offset 2026
	 */
	Gpio stepperEnablePin;
	/**
	 * offset 2028
	 */
	Gpio tle8888_cs;
	/**
	 * offset 2030
	 */
	pin_output_mode_e tle8888_csPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2031
	 */
	uint8_t alignmentFill_at_2031[1] = {};
	/**
	 * offset 2032
	 */
	Gpio mc33816_cs;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2034
	 */
	uint8_t alignmentFill_at_2034[2] = {};
	/**
	 * units: hz
	 * offset 2036
	 */
	float auxFrequencyFilter;
	/**
	 * offset 2040
	 */
	sent_input_pin_e sentInputPins[SENT_INPUT_COUNT] = {};
	/**
	 * Define o RPM acima do qual o corte de combustível está ativo.
	 * units: rpm
	 * offset 2042
	 */
	int16_t coastingFuelCutRpmHigh;
	/**
	 * Define o RPM abaixo do qual o corte de combustível é desativado, isso previne solavancos ou problemas ao transitar para marcha lenta
	 * units: rpm
	 * offset 2044
	 */
	int16_t coastingFuelCutRpmLow;
	/**
	 * Posição do acelerador abaixo da qual o corte de combustível está ativo. Com acelerador eletrônico habilitado, isso verifica a posição do pedal em vez da borboleta, e deve ser definido para 1-2%.
	 * units: %
	 * offset 2046
	 */
	int16_t coastingFuelCutTps;
	/**
	 * O corte de combustível é desabilitado quando o motor está frio.
	 * units: C
	 * offset 2048
	 */
	int16_t coastingFuelCutClt;
	/**
	 * Aumenta a reação do PID para RPM<alvo adicionando percentual extra ao erro do PID
	 * units: %
	 * offset 2050
	 */
	int16_t pidExtraForLowRpm;
	/**
	 * Valor de MAP acima do qual a injeção de combustível é reabilitada.
	 * units: kPa
	 * offset 2052
	 */
	int16_t coastingFuelCutMap;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2054
	 */
	uint8_t alignmentFill_at_2054[2] = {};
	/**
	 * offset 2056
	 */
	linear_sensor_s highPressureFuel;
	/**
	 * offset 2076
	 */
	linear_sensor_s lowPressureFuel;
	/**
	 * offset 2096
	 */
	gppwm_note_t scriptCurveName[SCRIPT_CURVE_COUNT] = {};
	/**
	 * offset 2192
	 */
	gppwm_note_t scriptTableName[SCRIPT_TABLE_COUNT] = {};
	/**
	 * offset 2256
	 */
	gppwm_note_t scriptSettingName[SCRIPT_SETTING_COUNT] = {};
	/**
	 * Coeficiente de transferência de calor em fluxo zero.
	 * 0 means the air charge is fully heated to the same temperature as CLT.
	 * 1 means the air charge gains no heat, and enters the cylinder at the temperature measured by IAT.
	 * offset 2384
	 */
	float tChargeAirCoefMin;
	/**
	 * Coeficiente de transferência de calor em alto fluxo, conforme definido por "fluxo máximo de ar".
	 * 0 means the air charge is fully heated to the same temperature as CLT.
	 * 1 means the air charge gains no heat, and enters the cylinder at the temperature measured by IAT.
	 * offset 2388
	 */
	float tChargeAirCoefMax;
	/**
	 * Ponto de alto fluxo para estimativa de transferência de calor.
	 * Defina isso para talvez 50-75% do seu fluxo máximo de ar em WOT.
	 * units: kg/h
	 * offset 2392
	 */
	float tChargeAirFlowMax;
	/**
	 * Taxa máxima permitida de aumento para a temperatura estimada da carga de ar
	 * units: deg/sec
	 * offset 2396
	 */
	float tChargeAirIncrLimit;
	/**
	 * Taxa máxima permitida de diminuição para a temperatura estimada da carga de ar
	 * units: deg/sec
	 * offset 2400
	 */
	float tChargeAirDecrLimit;
	/**
	 * offset 2404
	 */
	float hip9011Gain;
	/**
	 * Valor mínimo de iTerm
	 * offset 2408
	 */
	int16_t etb_iTermMin;
	/**
	 * Valor máximo de iTerm
	 * offset 2410
	 */
	int16_t etb_iTermMax;
	/**
	 * Veja useIdleTimingPidControl
	 * offset 2412
	 */
	pid_s idleTimingPid;
	/**
	 * Ao entrar em marcha lenta, e as configurações do PID são agressivas, é bom fazer uma entrada suave ao entrar em malha fechada
	 * offset 2432
	 */
	float idleTimingSoftEntryTime;
	/**
	 * offset 2436
	 */
	pin_input_mode_e torqueReductionTriggerPinMode;
	/**
	 * offset 2437
	 */
	torqueReductionActivationMode_e torqueReductionActivationMode;
	/**
	 * Atraso em ciclos entre porções de enriquecimento de combustível
	 * units: ciclos
	 * offset 2438
	 */
	int16_t tpsAccelFractionPeriod;
	/**
	 * Divisor de fração: 1 ou menos = toda a porção de uma vez, ou dividido em frações decrescentes
	 * units: coef
	 * offset 2440
	 */
	float tpsAccelFractionDivisor;
	/**
	 * offset 2444
	 */
	spi_device_e tle8888spiDevice;
	/**
	 * offset 2445
	 */
	spi_device_e mc33816spiDevice;
	/**
	 * Valor mínimo de iTerm
	 * offset 2446
	 */
	int16_t idlerpmpid_iTermMin;
	/**
	 * offset 2448
	 */
	spi_device_e tle6240spiDevice;
	/**
	 * Razão estequiométrica para seu combustível primário. Quando Flex Fuel está habilitado, este valor é usado quando o sensor Flex Fuel indica E0.
	 * E0 = 14.7
	 * E10 = 14.1
	 * E85 = 9.9
	 * E100 = 9.0
	 * units: :1
	 * offset 2449
	 */
	scaled_channel<uint8_t, 10, 1> stoichRatioPrimary;
	/**
	 * Valor máximo de iTerm
	 * offset 2450
	 */
	int16_t idlerpmpid_iTermMax;
	/**
	 * Define o alcance do controle de marcha lenta no ETB. Em 100% da posição de marcha lenta, o valor especificado aqui define a posição base do ETB.
	 * units: %
	 * offset 2452
	 */
	float etbIdleThrottleRange;
	/**
	 * Selecione a qual banco de correção de combustível este cilindro pertence. Agrupe cilindros que compartilham o mesmo sensor O2
	 * offset 2456
	 */
	uint8_t cylinderBankSelect[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: mg
	 * offset 2468
	 */
	scaled_channel<uint8_t, 1, 5> primeValues[PRIME_CURVE_COUNT] = {};
	/**
	 * Ponto central de tensão do comparador de trigger
	 * units: V
	 * offset 2476
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompCenterVolt;
	/**
	 * Tensão de histerese do comparador de trigger (Mín)
	 * units: V
	 * offset 2477
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompHystMin;
	/**
	 * Tensão de histerese do comparador de trigger (Máx)
	 * units: V
	 * offset 2478
	 */
	scaled_channel<uint8_t, 50, 1> triggerCompHystMax;
	/**
	 * RPM de saturação do sensor VR
	 * units: RPM
	 * offset 2479
	 */
	scaled_channel<uint8_t, 1, 50> triggerCompSensorSatRpm;
	/**
	 * units: razão
	 * offset 2480
	 */
	scaled_channel<uint16_t, 100, 1> tractionControlSlipBins[TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * units: RPM
	 * offset 2492
	 */
	uint8_t tractionControlSpeedBins[TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * offset 2498
	 */
	int8_t disableFan1AtSpeed;
	/**
	 * offset 2499
	 */
	int8_t disableFan2AtSpeed;
	/**
	 * offset 2500
	 */
	can_vss_nbc_e canVssNbcType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2502
	 */
	uint8_t alignmentFill_at_2502[2] = {};
	/**
	 * offset 2504
	 */
	gppwm_channel gppwm[GPPWM_CHANNELS] = {};
	/**
	 * Corrente de Boost
	 * units: mA
	 * offset 2936
	 */
	uint16_t mc33_i_boost;
	/**
	 * Corrente de Pico
	 * units: mA
	 * offset 2938
	 */
	uint16_t mc33_i_peak;
	/**
	 * Corrente de Manutenção
	 * units: mA
	 * offset 2940
	 */
	uint16_t mc33_i_hold;
	/**
	 * Tempo máximo permitido na fase de boost. Se a corrente do injetor não atingir o limite antes desse tempo, assume-se que o injetor está ausente ou com falha em circuito aberto.
	 * units: us
	 * offset 2942
	 */
	uint16_t mc33_t_max_boost;
	/**
	 * units: us
	 * offset 2944
	 */
	uint16_t mc33_t_peak_off;
	/**
	 * Duração da fase de pico
	 * units: us
	 * offset 2946
	 */
	uint16_t mc33_t_peak_tot;
	/**
	 * units: us
	 * offset 2948
	 */
	uint16_t mc33_t_bypass;
	/**
	 * units: us
	 * offset 2950
	 */
	uint16_t mc33_t_hold_off;
	/**
	 * Duração da fase de manutenção
	 * units: us
	 * offset 2952
	 */
	uint16_t mc33_t_hold_tot;
	/**
	 * offset 2954
	 */
	pin_input_mode_e tcuUpshiftButtonPinMode;
	/**
	 * offset 2955
	 */
	pin_input_mode_e tcuDownshiftButtonPinMode;
	/**
	 * offset 2956
	 */
	pin_input_mode_e acSwitchMode;
	/**
	 * offset 2957
	 */
	pin_output_mode_e tcu_solenoid_mode[TCU_SOLENOID_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 2963
	 */
	uint8_t alignmentFill_at_2963[1] = {};
	/**
	 * units: razão
	 * offset 2964
	 */
	float triggerGapOverrideFrom[GAP_TRACKING_LENGTH] = {};
	/**
	 * units: razão
	 * offset 3036
	 */
	float triggerGapOverrideTo[GAP_TRACKING_LENGTH] = {};
	/**
	 * Abaixo deste RPM, use a informação do comando para sincronizar a posição do virabrequim para operação totalmente sequencial. Use isto se seu sensor de comando apresentar comportamentos estranhos em altas rotações. Defina como 0 para desabilitar e sempre usar o comando para ajudar a sincronizar o virabrequim.
	 * units: rpm
	 * offset 3108
	 */
	scaled_channel<uint8_t, 1, 50> maxCamPhaseResolveRpm;
	/**
	 * Atraso antes de cortar o combustível. Defina como 0 para cortar imediatamente sem atraso. Pode causar estouros e pipocos no escapamento...
	 * units: seg
	 * offset 3109
	 */
	scaled_channel<uint8_t, 10, 1> dfcoDelay;
	/**
	 * Atraso antes de acionar o compressor do ar-condicionado. Defina como 0 para acionar imediatamente sem atraso. Use isto para evitar queda de rotação em marcha lenta ao acionar o ar-condicionado.
	 * units: seg
	 * offset 3110
	 */
	scaled_channel<uint8_t, 10, 1> acDelay;
	/**
	 * offset 3111
	 */
	tChargeMode_e tChargeMode;
	/**
	 * units: mg
	 * offset 3112
	 */
	scaled_channel<uint16_t, 1000, 1> fordInjectorSmallPulseBreakPoint;
	/**
	 * Limite de erro do ETB (alvo vs. real) acima do qual o temporizador de travamento é iniciado. Se o temporizador atingir o tempo especificado no período de detecção de travamento, a borboleta é considerada travada e a operação do motor é limitada.
	 * units: %
	 * offset 3114
	 */
	uint8_t etbJamDetectThreshold;
	/**
	 * units: lóbulo/comando
	 * offset 3115
	 */
	uint8_t hpfpCamLobes;
	/**
	 * offset 3116
	 */
	hpfp_cam_e hpfpCam;
	/**
	 * Baixa rotação do motor para o A/C. Motores maiores podem suportar valores menores
	 * units: RPM
	 * offset 3117
	 */
	scaled_channel<int8_t, 1, 10> acLowRpmLimit;
	/**
	 * Se o tempo de ativação solicitado estiver abaixo deste ângulo, não acione a bomba
	 * units: graus
	 * offset 3118
	 */
	uint8_t hpfpMinAngle;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3119
	 */
	uint8_t alignmentFill_at_3119[1] = {};
	/**
	 * Tamanho da câmara da bomba em cc. Típica Bosch HDP5 tem 9,0mm de diâmetro, típico BMW N* curso é 4,4mm.
	 * units: cc
	 * offset 3120
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpPumpVolume;
	/**
	 * Tempo de ativação da válvula (para permitir que a bomba gere pressão e mantenha a válvula aberta sozinha)
	 * units: graus
	 * offset 3122
	 */
	uint8_t hpfpActivationAngle;
	/**
	 * offset 3123
	 */
	uint8_t issFilterReciprocal;
	/**
	 * units: %/kPa
	 * offset 3124
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpPidP;
	/**
	 * units: %/kPa/lóbulo
	 * offset 3126
	 */
	scaled_channel<uint16_t, 100000, 1> hpfpPidI;
	/**
	 * A taxa mais rápida que a pressão alvo pode ser reduzida. Isso porque HPFP não tem como aliviar pressão além de injetar combustível.
	 * units: kPa/s
	 * offset 3128
	 */
	uint16_t hpfpTargetDecay;
	/**
	 * offset 3130
	 */
	output_pin_e stepper_raw_output[4] = {};
	/**
	 * units: ratio
	 * offset 3138
	 */
	scaled_channel<uint16_t, 100, 1> gearRatio[TCU_GEAR_COUNT] = {};
	/**
	 * We need to give engine time to build oil pressure without diverting it to VVT
	 * units: ms
	 * offset 3158
	 */
	uint16_t vvtActivationDelayMs;
	/**
	 * units: Nm
	 * offset 3160
	 */
	scaled_channel<uint8_t, 1, 10> torqueTable[TORQUE_CURVE_SIZE][TORQUE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 3196
	 */
	uint16_t torqueRpmBins[TORQUE_CURVE_SIZE] = {};
	/**
	 * units: Load
	 * offset 3208
	 */
	uint16_t torqueLoadBins[TORQUE_CURVE_SIZE] = {};
	/**
	 * offset 3220
	 */
	GearControllerMode gearControllerMode;
	/**
	 * offset 3221
	 */
	TransmissionControllerMode transmissionControllerMode;
	/**
	 * During revolution where ACR should be disabled at what specific angle to disengage
	 * units: deg
	 * offset 3222
	 */
	uint16_t acrDisablePhase;
	/**
	 * offset 3224
	 */
	linear_sensor_s auxLinear1;
	/**
	 * offset 3244
	 */
	linear_sensor_s auxLinear2;
	/**
	 * offset 3264
	 */
	output_pin_e tcu_tcc_onoff_solenoid;
	/**
	 * offset 3266
	 */
	pin_output_mode_e tcu_tcc_onoff_solenoid_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3267
	 */
	uint8_t alignmentFill_at_3267[1] = {};
	/**
	 * offset 3268
	 */
	output_pin_e tcu_tcc_pwm_solenoid;
	/**
	 * offset 3270
	 */
	pin_output_mode_e tcu_tcc_pwm_solenoid_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3271
	 */
	uint8_t alignmentFill_at_3271[1] = {};
	/**
	 * offset 3272
	 */
	pwm_freq_t tcu_tcc_pwm_solenoid_freq;
	/**
	 * offset 3274
	 */
	output_pin_e tcu_pc_solenoid_pin;
	/**
	 * offset 3276
	 */
	pin_output_mode_e tcu_pc_solenoid_pin_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3277
	 */
	uint8_t alignmentFill_at_3277[1] = {};
	/**
	 * offset 3278
	 */
	pwm_freq_t tcu_pc_solenoid_freq;
	/**
	 * offset 3280
	 */
	output_pin_e tcu_32_solenoid_pin;
	/**
	 * offset 3282
	 */
	pin_output_mode_e tcu_32_solenoid_pin_mode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3283
	 */
	uint8_t alignmentFill_at_3283[1] = {};
	/**
	 * offset 3284
	 */
	pwm_freq_t tcu_32_solenoid_freq;
	/**
	 * offset 3286
	 */
	output_pin_e acrPin2;
	/**
	 * Set a minimum allowed target position to avoid slamming/driving against the hard mechanical stop in the throttle.
	 * units: %
	 * offset 3288
	 */
	scaled_channel<uint8_t, 10, 1> etbMinimumPosition;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3289
	 */
	uint8_t alignmentFill_at_3289[1] = {};
	/**
	 * offset 3290
	 */
	uint16_t tuneHidingKey;
	/**
	 * Individual charaters are accessible using vin(index) Lua function
	 * offset 3292
	 */
	vin_number_t vinNumber;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3309
	 */
	uint8_t alignmentFill_at_3309[1] = {};
	/**
	 * offset 3310
	 */
	uint16_t highSpeedOffsets[HIGH_SPEED_COUNT] = {};
	/**
	 * offset 3374
	 */
	fuel_pressure_sensor_mode_e fuelPressureSensorMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3375
	 */
	uint8_t alignmentFill_at_3375[1] = {};
	/**
	 * offset 3376
	 */
	switch_input_pin_e luaDigitalInputPins[LUA_DIGITAL_INPUT_COUNT] = {};
	/**
	 * units: rpm
	 * offset 3392
	 */
	int16_t ALSMinRPM;
	/**
	 * units: rpm
	 * offset 3394
	 */
	int16_t ALSMaxRPM;
	/**
	 * units: sec
	 * offset 3396
	 */
	int16_t ALSMaxDuration;
	/**
	 * units: C
	 * offset 3398
	 */
	int8_t ALSMinCLT;
	/**
	 * units: C
	 * offset 3399
	 */
	int8_t ALSMaxCLT;
	/**
	 * offset 3400
	 */
	uint8_t alsMinTimeBetween;
	/**
	 * offset 3401
	 */
	uint8_t alsEtbPosition;
	/**
	 * units: %
	 * offset 3402
	 */
	uint8_t acRelayAlternatorDutyAdder;
	/**
	 * If you have SENT TPS sensor please select type. For analog TPS leave None
	 * offset 3403
	 */
	SentEtbType sentEtbType;
	/**
	 * offset 3404
	 */
	uint16_t customSentTpsMin;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3406
	 */
	uint8_t alignmentFill_at_3406[2] = {};
	/**
	 * units: %
	 * offset 3408
	 */
	int ALSIdleAdd;
	/**
	 * units: %
	 * offset 3412
	 */
	int ALSEtbAdd;
	/**
	 * offset 3416
	 */
	float ALSSkipRatio;
	/**
	 * Hysterisis: if Pressure High Disable is 240kpa, and acPressureEnableHyst is 20, when the ECU sees 240kpa, A/C will be disabled, and stay disabled until 240-20=220kpa is reached
	 * units: kPa (absolute)
	 * offset 3420
	 */
	scaled_channel<uint8_t, 2, 1> acPressureEnableHyst;
	/**
	 * offset 3421
	 */
	pin_input_mode_e ALSActivatePinMode;
	/**
	 * For Ford TPS, use 53%. For Toyota ETCS-i, use ~65%
	 * units: %
	 * offset 3422
	 */
	scaled_channel<uint8_t, 2, 1> tpsSecondaryMaximum;
	/**
	 * For Toyota ETCS-i, use ~69%
	 * units: %
	 * offset 3423
	 */
	scaled_channel<uint8_t, 2, 1> ppsSecondaryMaximum;
	/**
	 * offset 3424
	 */
	pin_input_mode_e luaDigitalInputPinModes[LUA_DIGITAL_INPUT_COUNT] = {};
	/**
	 * offset 3432
	 */
	uint16_t customSentTpsMax;
	/**
	 * offset 3434
	 */
	uint16_t kLineBaudRate;
	/**
	 * offset 3436
	 */
	CanGpioType canGpioType;
	/**
	 * offset 3437
	 */
	UiMode uiMode;
	/**
	 * Crank angle ATDC of first lobe peak
	 * units: deg
	 * offset 3438
	 */
	int16_t hpfpPeakPos;
	/**
	 * units: us
	 * offset 3440
	 */
	int16_t kLinePeriodUs;
	/**
	 * Window that the correction will be added throughout (example, if rpm limit is 7000, and rpmSoftLimitWindowSize is 200, the corrections activate at 6800RPM, creating a 200rpm window)
	 * units: RPM
	 * offset 3442
	 */
	scaled_channel<uint8_t, 1, 10> rpmSoftLimitWindowSize;
	/**
	 * Degrees of timing REMOVED from actual timing during soft RPM limit window
	 * units: deg
	 * offset 3443
	 */
	scaled_channel<uint8_t, 5, 1> rpmSoftLimitTimingRetard;
	/**
	 * % of fuel ADDED during window
	 * units: %
	 * offset 3444
	 */
	scaled_channel<uint8_t, 5, 1> rpmSoftLimitFuelAdded;
	/**
	 * Hysterisis: if the hard limit is 7200rpm and rpmHardLimitHyst is 200rpm, then when the ECU sees 7200rpm, fuel/ign will cut, and stay cut until 7000rpm (7200-200) is reached
	 * units: RPM
	 * offset 3445
	 */
	scaled_channel<uint8_t, 1, 10> rpmHardLimitHyst;
	/**
	 * Time between bench test pulses
	 * units: ms
	 * offset 3446
	 */
	scaled_channel<uint16_t, 10, 1> benchTestOffTime;
	/**
	 * Hysterisis: if hard cut is 240kpa, and boostCutPressureHyst is 20, when the ECU sees 240kpa, fuel/ign will cut, and stay cut until 240-20=220kpa is reached
	 * units: kPa (absolute)
	 * offset 3448
	 */
	scaled_channel<uint8_t, 2, 1> boostCutPressureHyst;
	/**
	 * Boost duty cycle modified by gear
	 * units: %
	 * offset 3449
	 */
	scaled_channel<int8_t, 2, 1> gearBasedOpenLoopBoostAdder[TCU_GEAR_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3459
	 */
	uint8_t alignmentFill_at_3459[1] = {};
	/**
	 * How many test bench pulses do you want
	 * offset 3460
	 */
	uint32_t benchTestCount;
	/**
	 * How long initial idle adder is held before starting to decay.
	 * units: seconds
	 * offset 3464
	 */
	scaled_channel<uint8_t, 10, 1> iacByTpsHoldTime;
	/**
	 * How long it takes to remove initial IAC adder to return to normal idle.
	 * units: seconds
	 * offset 3465
	 */
	scaled_channel<uint8_t, 10, 1> iacByTpsDecayTime;
	/**
	 * offset 3466
	 */
	switch_input_pin_e tcu_rangeInput[RANGE_INPUT_COUNT] = {};
	/**
	 * offset 3478
	 */
	pin_input_mode_e tcu_rangeInputMode[RANGE_INPUT_COUNT] = {};
	/**
	 * Scale the reported vehicle speed value from CAN. Example: Parameter set to 1.1, CAN VSS reports 50kph, ECU will report 55kph instead.
	 * units: ratio
	 * offset 3484
	 */
	scaled_channel<uint16_t, 10000, 1> canVssScaling;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3486
	 */
	uint8_t alignmentFill_at_3486[2] = {};
	/**
	 * offset 3488
	 */
	ThermistorConf oilTempSensor;
	/**
	 * offset 3520
	 */
	ThermistorConf fuelTempSensor;
	/**
	 * offset 3552
	 */
	ThermistorConf ambientTempSensor;
	/**
	 * offset 3584
	 */
	ThermistorConf compressorDischargeTemperature;
	/**
	 * Place the sensor before the throttle, but after any turbocharger/supercharger and intercoolers if fitted. Uses the same calibration as the MAP sensor.
	 * offset 3616
	 */
	adc_channel_e throttleInletPressureChannel;
	/**
	 * Place the sensor after the turbocharger/supercharger, but before any intercoolers if fitted. Uses the same calibration as the MAP sensor.
	 * offset 3617
	 */
	adc_channel_e compressorDischargePressureChannel;
	/**
	 * offset 3618
	 */
	Gpio dacOutputPins[DAC_OUTPUT_COUNT] = {};
	/**
	 * offset 3622
	 */
	output_pin_e speedometerOutputPin;
	/**
	 * Number of speedometer pulses per kilometer travelled.
	 * offset 3624
	 */
	uint16_t speedometerPulsePerKm;
	/**
	 * offset 3626
	 */
	uint8_t simulatorCamPosition[CAM_INPUTS_COUNT] = {};
	/**
	 * offset 3630
	 */
	adc_channel_e ignKeyAdcChannel;
	/**
	 * offset 3631
	 */
	pin_mode_e spi6MisoMode;
	/**
	 * units: ratio
	 * offset 3632
	 */
	float triggerVVTGapOverrideFrom[VVT_TRACKING_LENGTH] = {};
	/**
	 * units: ratio
	 * offset 3648
	 */
	float triggerVVTGapOverrideTo[VVT_TRACKING_LENGTH] = {};
	/**
	 * units: %
	 * offset 3664
	 */
	int8_t tractionControlEtbDrop[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * If injector duty cycle hits this value, instantly cut fuel.
	 * units: %
	 * offset 3700
	 */
	uint8_t maxInjectorDutyInstant;
	/**
	 * If injector duty cycle hits this value for the specified delay time, cut fuel.
	 * units: %
	 * offset 3701
	 */
	uint8_t maxInjectorDutySustained;
	/**
	 * Timeout period for duty cycle over the sustained limit to trigger duty cycle protection.
	 * units: sec
	 * offset 3702
	 */
	scaled_channel<uint8_t, 10, 1> maxInjectorDutySustainedTimeout;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3703
	 */
	uint8_t alignmentFill_at_3703[1] = {};
	/**
	 * offset 3704
	 */
	output_pin_e injectionPinsStage2[MAX_CYLINDER_COUNT] = {};
	/**
	 * units: Deg
	 * offset 3728
	 */
	int8_t tractionControlTimingDrop[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * units: %
	 * offset 3764
	 */
	int8_t tractionControlIgnitionSkip[TRACTION_CONTROL_ETB_DROP_SIZE][TRACTION_CONTROL_ETB_DROP_SIZE] = {};
	/**
	 * offset 3800
	 */
	float auxSpeed1Multiplier;
	/**
	 * offset 3804
	 */
	float brakeMeanEffectivePressureDifferential;
	/**
	 * offset 3808
	 */
	Gpio spi4mosiPin;
	/**
	 * offset 3810
	 */
	Gpio spi4misoPin;
	/**
	 * offset 3812
	 */
	Gpio spi4sckPin;
	/**
	 * offset 3814
	 */
	Gpio spi5mosiPin;
	/**
	 * offset 3816
	 */
	Gpio spi5misoPin;
	/**
	 * offset 3818
	 */
	Gpio spi5sckPin;
	/**
	 * offset 3820
	 */
	Gpio spi6mosiPin;
	/**
	 * offset 3822
	 */
	Gpio spi6misoPin;
	/**
	 * offset 3824
	 */
	Gpio spi6sckPin;
	/**
	 * offset 3826
	 */
	pin_mode_e spi4SckMode;
	/**
	 * offset 3827
	 */
	pin_mode_e spi4MosiMode;
	/**
	 * offset 3828
	 */
	pin_mode_e spi4MisoMode;
	/**
	 * offset 3829
	 */
	pin_mode_e spi5SckMode;
	/**
	 * offset 3830
	 */
	pin_mode_e spi5MosiMode;
	/**
	 * offset 3831
	 */
	pin_mode_e spi5MisoMode;
	/**
	 * offset 3832
	 */
	pin_mode_e spi6SckMode;
	/**
	 * offset 3833
	 */
	pin_mode_e spi6MosiMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3834
	 */
	uint8_t alignmentFill_at_3834[2] = {};
	/**
	 * Secondary TTL channel baud rate
	 * units: BPs
	 * offset 3836
	 */
	uint32_t tunerStudioSerialSpeed;
	/**
	 * offset 3840
	 */
	Gpio camSimulatorPin;
	/**
	 * offset 3842
	 */
	pin_output_mode_e camSimulatorPinMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3843
	 */
	uint8_t alignmentFill_at_3843[1] = {};
	/**
	 * offset 3844
	 */
	int anotherCiTest;
	/**
	 * offset 3848
	 */
	uint32_t device_uid[3] = {};
	/**
	 * offset 3860
	 */
	adc_channel_e tcu_rangeAnalogInput[RANGE_INPUT_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3866
	 */
	uint8_t alignmentFill_at_3866[2] = {};
	/**
	 * units: Ohm
	 * offset 3868
	 */
	float tcu_rangeSensorBiasResistor;
	/**
	 * offset 3872
	 */
	MsIoBox_config_s msIoBox0;
	/**
	 * Nominal coil charge current, 0.25A step
	 * units: A
	 * offset 3876
	 */
	scaled_channel<uint8_t, 4, 1> mc33810Nomi;
	/**
	 * Maximum coil charge current, 1A step
	 * units: A
	 * offset 3877
	 */
	uint8_t mc33810Maxi;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3878
	 */
	uint8_t alignmentFill_at_3878[2] = {};
	/**
	 * offset 3880
	 */
	linear_sensor_s acPressure;
	/**
	 * value of A/C pressure in kPa before that compressor is disengaged
	 * units: kPa
	 * offset 3900
	 */
	uint16_t minAcPressure;
	/**
	 * value of A/C pressure in kPa after that compressor is disengaged
	 * units: kPa
	 * offset 3902
	 */
	uint16_t maxAcPressure;
	/**
	 * Delay before cutting fuel due to low oil pressure. Use this to ignore short pressure blips and sensor noise.
	 * units: sec
	 * offset 3904
	 */
	scaled_channel<uint8_t, 10, 1> minimumOilPressureTimeout;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3905
	 */
	uint8_t alignmentFill_at_3905[3] = {};
	/**
	 * offset 3908
	 */
	linear_sensor_s auxLinear3;
	/**
	 * offset 3928
	 */
	linear_sensor_s auxLinear4;
	/**
	 * Below TPS value all knock suppression will be disabled.
	 * units: %
	 * offset 3948
	 */
	scaled_channel<uint8_t, 1, 1> knockSuppressMinTps;
	/**
	 * Fuel to odd when a knock event occurs. Advice: 5% (mild), 10% (turbo/high comp.), 15% (high knock, e.g. GDI), 20% (spicy lump),
	 * units: %
	 * offset 3949
	 */
	scaled_channel<uint8_t, 10, 1> knockFuelTrimAggression;
	/**
	 * After a knock event, reapply fuel at this rate.
	 * units: 1%/s
	 * offset 3950
	 */
	scaled_channel<uint8_t, 10, 1> knockFuelTrimReapplyRate;
	/**
	 * Fuel trim when knock, max 30%
	 * units: %
	 * offset 3951
	 */
	scaled_channel<uint8_t, 1, 1> knockFuelTrim;
	/**
	 * units: sense
	 * offset 3952
	 */
	float knockSpectrumSensitivity;
	/**
	 * "Estimated knock frequency, ignore cylinderBore if this one > 0"
	 * units: Hz
	 * offset 3956
	 */
	float knockFrequency;
	/**
	 * Nenhum = I have a MAP-referenced fuel pressure regulator
	 * Fixed rail pressure = I have an atmosphere-referenced fuel pressure regulator (returnless, typically)
	 * Sensed rail pressure = I have a fuel pressure sensor
	 * offset 3960
	 */
	injector_compensation_mode_e secondaryInjectorCompensationMode;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3961
	 */
	uint8_t alignmentFill_at_3961[3] = {};
	/**
	 * This is the pressure at which your injector flow is known.
	 * For example if your injectors flow 400cc/min at 3.5 bar, enter 350kpa here.
	 * units: kPa
	 * offset 3964
	 */
	float secondaryInjectorFuelReferencePressure;
	/**
	 * SENT input connected to ETB
	 * offset 3968
	 */
	SentInput EtbSentInput;
	/**
	 * SENT input used for high pressure fuel sensor
	 * offset 3969
	 */
	SentInput FuelHighPressureSentInput;
	/**
	 * If you have SENT High Pressure Fuel Sensor please select type. For analog TPS leave None
	 * offset 3970
	 */
	SentFuelHighPressureType FuelHighPressureSentType;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3971
	 */
	uint8_t alignmentFill_at_3971[1] = {};
	/**
	offset 3972 bit 0 */
	bool nitrousControlEnabled : 1 {};
	/**
	offset 3972 bit 1 */
	bool unusedFancy2 : 1 {};
	/**
	offset 3972 bit 2 */
	bool unusedFancy3 : 1 {};
	/**
	offset 3972 bit 3 */
	bool unusedFancy4 : 1 {};
	/**
	offset 3972 bit 4 */
	bool unusedFancy5 : 1 {};
	/**
	offset 3972 bit 5 */
	bool unusedFancy6 : 1 {};
	/**
	offset 3972 bit 6 */
	bool unusedFancy7 : 1 {};
	/**
	offset 3972 bit 7 */
	bool unusedFancy8 : 1 {};
	/**
	offset 3972 bit 8 */
	bool unusedFancy9 : 1 {};
	/**
	offset 3972 bit 9 */
	bool unusedFancy10 : 1 {};
	/**
	offset 3972 bit 10 */
	bool unusedFancy11 : 1 {};
	/**
	offset 3972 bit 11 */
	bool unusedFancy12 : 1 {};
	/**
	offset 3972 bit 12 */
	bool unusedFancy13 : 1 {};
	/**
	offset 3972 bit 13 */
	bool unusedFancy14 : 1 {};
	/**
	offset 3972 bit 14 */
	bool unusedFancy15 : 1 {};
	/**
	offset 3972 bit 15 */
	bool unusedFancy16 : 1 {};
	/**
	offset 3972 bit 16 */
	bool unusedFancy17 : 1 {};
	/**
	offset 3972 bit 17 */
	bool unusedFancy18 : 1 {};
	/**
	offset 3972 bit 18 */
	bool unusedFancy19 : 1 {};
	/**
	offset 3972 bit 19 */
	bool unusedFancy20 : 1 {};
	/**
	offset 3972 bit 20 */
	bool unusedFancy21 : 1 {};
	/**
	offset 3972 bit 21 */
	bool unusedFancy22 : 1 {};
	/**
	offset 3972 bit 22 */
	bool unusedFancy23 : 1 {};
	/**
	offset 3972 bit 23 */
	bool unusedFancy24 : 1 {};
	/**
	offset 3972 bit 24 */
	bool unusedFancy25 : 1 {};
	/**
	offset 3972 bit 25 */
	bool unusedFancy26 : 1 {};
	/**
	offset 3972 bit 26 */
	bool unusedFancy27 : 1 {};
	/**
	offset 3972 bit 27 */
	bool unusedFancy28 : 1 {};
	/**
	offset 3972 bit 28 */
	bool unusedFancy29 : 1 {};
	/**
	offset 3972 bit 29 */
	bool unusedFancy30 : 1 {};
	/**
	offset 3972 bit 30 */
	bool unusedFancy31 : 1 {};
	/**
	offset 3972 bit 31 */
	bool unusedFancy32 : 1 {};
	/**
	 * offset 3976
	 */
	nitrous_arming_method_e nitrousControlArmingMethod;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3977
	 */
	uint8_t alignmentFill_at_3977[1] = {};
	/**
	 * Pin that activates nitrous control
	 * offset 3978
	 */
	switch_input_pin_e nitrousControlTriggerPin;
	/**
	 * offset 3980
	 */
	pin_input_mode_e nitrousControlTriggerPinMode;
	/**
	 * offset 3981
	 */
	lua_gauge_e nitrousLuaGauge;
	/**
	 * offset 3982
	 */
	lua_gauge_meaning_e nitrousLuaGaugeMeaning;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3983
	 */
	uint8_t alignmentFill_at_3983[1] = {};
	/**
	 * offset 3984
	 */
	float nitrousLuaGaugeArmingValue;
	/**
	 * offset 3988
	 */
	int nitrousMinimumTps;
	/**
	 * units: deg C
	 * offset 3992
	 */
	uint8_t nitrousMinimumClt;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3993
	 */
	uint8_t alignmentFill_at_3993[1] = {};
	/**
	 * units: kPa
	 * offset 3994
	 */
	int16_t nitrousMaximumMap;
	/**
	 * units: afr
	 * offset 3996
	 */
	scaled_channel<uint8_t, 10, 1> nitrousMaximumAfr;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 3997
	 */
	uint8_t alignmentFill_at_3997[1] = {};
	/**
	 * units: rpm
	 * offset 3998
	 */
	uint16_t nitrousActivationRpm;
	/**
	 * units: rpm
	 * offset 4000
	 */
	uint16_t nitrousDeactivationRpm;
	/**
	 * units: rpm
	 * offset 4002
	 */
	uint16_t nitrousDeactivationRpmWindow;
	/**
	 * Retard timing by this amount during DFCO. Smooths the transition back from fuel cut. After fuel is restored, ramp timing back in over the period specified.
	 * units: deg
	 * offset 4004
	 */
	uint8_t dfcoRetardDeg;
	/**
	 * Smooths the transition back from fuel cut. After fuel is restored, ramp timing back in over the period specified.
	 * units: s
	 * offset 4005
	 */
	scaled_channel<uint8_t, 10, 1> dfcoRetardRampInTime;
	/**
	 * offset 4006
	 */
	output_pin_e nitrousRelayPin;
	/**
	 * offset 4008
	 */
	pin_output_mode_e nitrousRelayPinMode;
	/**
	 * units: %
	 * offset 4009
	 */
	int8_t nitrousFuelAdderPercent;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4010
	 */
	uint8_t alignmentFill_at_4010[2] = {};
	/**
	 * Retard timing to remove from actual final timing (after all corrections) due to additional air.
	 * units: deg
	 * offset 4012
	 */
	float nitrousIgnitionRetard;
	/**
	 * units: Kph
	 * offset 4016
	 */
	uint16_t nitrousMinimumVehicleSpeed;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4018
	 */
	uint8_t alignmentFill_at_4018[2] = {};
	/**
	 * Exponential Average Alpha filtering parameter
	 * offset 4020
	 */
	float fuelLevelAveragingAlpha;
	/**
	 * How often do we update fuel level gauge
	 * units: seconds
	 * offset 4024
	 */
	float fuelLevelUpdatePeriodSec;
	/**
	 * Error below specified value
	 * units: v
	 * offset 4028
	 */
	float fuelLevelLowThresholdVoltage;
	/**
	 * Error above specified value
	 * units: v
	 * offset 4032
	 */
	float fuelLevelHighThresholdVoltage;
	/**
	 * offset 4036
	 */
	float mapExpAverageAfr;
	/**
	 * Compensates for trigger delay due to belt stretch, or other electromechanical issues. beware that raising this value advances ignition timing!
	 * units: uS
	 * offset 4040
	 */
	scaled_channel<uint8_t, 1, 1> sparkHardwareLatencyCorrection;
	/**
	 * Delay before cutting fuel due to extra high oil pressure. Use this to ignore short pressure blips and sensor noise.
	 * units: sec
	 * offset 4041
	 */
	scaled_channel<uint8_t, 10, 1> maxOilPressureTimeout;
	/**
	 * Maximum allowed oil pressure. If oil pressure exceed this level within <timeout> seconds, fuel will be cut. Set to 0 to disable.
	 * units: kPa
	 * offset 4042
	 */
	scaled_channel<uint8_t, 1, 10> maxOilPressure;
	/**
	 * units: units
	 * offset 4043
	 */
	uint8_t unusedOftenChangesDuringFirmwareUpdate[END_OF_CALIBRATION_PADDING] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 4094
	 */
	uint8_t alignmentFill_at_4094[2] = {};
};
static_assert(sizeof(engine_configuration_s) == 4096);

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
	 * offset 4096
	 */
	float postCrankingFactor[CRANKING_ENRICH_COUNT][CRANKING_ENRICH_COUNT] = {};
	/**
	 * units: count
	 * offset 4240
	 */
	uint16_t postCrankingDurationBins[CRANKING_ENRICH_COUNT] = {};
	/**
	 * units: C
	 * offset 4252
	 */
	int16_t postCrankingCLTBins[CRANKING_ENRICH_COUNT] = {};
	/**
	 * target TPS value, 0 to 100%
	 * TODO: use int8 data date once we template interpolation method
	 * units: target TPS position
	 * offset 4264
	 */
	float etbBiasBins[ETB_BIAS_CURVE_LENGTH] = {};
	/**
	 * PWM bias, open loop component of PID closed loop control
	 * units: ETB duty cycle bias
	 * offset 4296
	 */
	float etbBiasValues[ETB_BIAS_CURVE_LENGTH] = {};
	/**
	 * units: %
	 * offset 4328
	 */
	scaled_channel<uint8_t, 20, 1> iacPidMultTable[IAC_PID_MULT_SIZE][IAC_PID_MULT_SIZE] = {};
	/**
	 * units: Load
	 * offset 4392
	 */
	uint8_t iacPidMultLoadBins[IAC_PID_MULT_SIZE] = {};
	/**
	 * units: RPM
	 * offset 4400
	 */
	scaled_channel<uint8_t, 1, 10> iacPidMultRpmBins[IAC_PID_MULT_SIZE] = {};
	/**
	 * On Single Coil or Wasted Spark setups you have to lower dwell at high RPM
	 * units: RPM
	 * offset 4408
	 */
	uint16_t sparkDwellRpmBins[DWELL_CURVE_SIZE] = {};
	/**
	 * units: ms
	 * offset 4424
	 */
	scaled_channel<uint16_t, 100, 1> sparkDwellValues[DWELL_CURVE_SIZE] = {};
	/**
	 * CLT-based target RPM for automatic idle controller
	 * units: C
	 * offset 4440
	 */
	scaled_channel<int8_t, 1, 2> cltIdleRpmBins[CLT_CURVE_SIZE] = {};
	/**
	 * See idleRpmPid
	 * units: RPM
	 * offset 4456
	 */
	scaled_channel<uint8_t, 1, 20> cltIdleRpm[CLT_CURVE_SIZE] = {};
	/**
	 * units: deg
	 * offset 4472
	 */
	scaled_channel<int16_t, 10, 1> ignitionCltCorrTable[CLT_TIMING_CURVE_SIZE][CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * CLT-based timing correction
	 * units: C
	 * offset 4522
	 */
	int8_t ignitionCltCorrTempBins[CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * units: Load
	 * offset 4527
	 */
	scaled_channel<uint8_t, 1, 5> ignitionCltCorrLoadBins[CLT_TIMING_CURVE_SIZE] = {};
	/**
	 * units: x
	 * offset 4532
	 */
	float scriptCurve1Bins[SCRIPT_CURVE_16] = {};
	/**
	 * units: y
	 * offset 4596
	 */
	float scriptCurve1[SCRIPT_CURVE_16] = {};
	/**
	 * units: x
	 * offset 4660
	 */
	float scriptCurve2Bins[SCRIPT_CURVE_16] = {};
	/**
	 * units: y
	 * offset 4724
	 */
	float scriptCurve2[SCRIPT_CURVE_16] = {};
	/**
	 * units: x
	 * offset 4788
	 */
	float scriptCurve3Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4820
	 */
	float scriptCurve3[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4852
	 */
	float scriptCurve4Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4884
	 */
	float scriptCurve4[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4916
	 */
	float scriptCurve5Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 4948
	 */
	float scriptCurve5[SCRIPT_CURVE_8] = {};
	/**
	 * units: x
	 * offset 4980
	 */
	float scriptCurve6Bins[SCRIPT_CURVE_8] = {};
	/**
	 * units: y
	 * offset 5012
	 */
	float scriptCurve6[SCRIPT_CURVE_8] = {};
	/**
	 * units: kPa
	 * offset 5044
	 */
	float baroCorrPressureBins[BARO_CORR_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5060
	 */
	float baroCorrRpmBins[BARO_CORR_SIZE] = {};
	/**
	 * units: ratio
	 * offset 5076
	 */
	float baroCorrTable[BARO_CORR_SIZE][BARO_CORR_SIZE] = {};
	/**
	 * Cranking fuel correction coefficient based on TPS
	 * units: Ratio
	 * offset 5140
	 */
	float crankingTpsCoef[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: %
	 * offset 5172
	 */
	float crankingTpsBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Cranking (see useSeparateAdvanceForCranking)
	 * units: RPM
	 * offset 5204
	 */
	uint16_t crankingAdvanceBins[CRANKING_ADVANCE_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Cranking (see useSeparateAdvanceForCranking)
	 * units: deg
	 * offset 5212
	 */
	scaled_channel<int16_t, 100, 1> crankingAdvance[CRANKING_ADVANCE_CURVE_SIZE] = {};
	/**
	 * offset 5220
	 */
	scaled_channel<uint8_t, 2, 1> boostTableOpenLoop[BOOST_LOAD_COUNT][BOOST_RPM_COUNT] = {};
	/**
	 * units: RPM
	 * offset 5284
	 */
	scaled_channel<uint8_t, 1, 100> boostRpmBins[BOOST_RPM_COUNT] = {};
	/**
	 * offset 5292
	 */
	uint16_t boostOpenLoopLoadBins[BOOST_LOAD_COUNT] = {};
	/**
	 * offset 5308
	 */
	scaled_channel<uint8_t, 1, 2> boostTableClosedLoop[BOOST_LOAD_COUNT][BOOST_RPM_COUNT] = {};
	/**
	 * offset 5372
	 */
	uint16_t boostClosedLoopLoadBins[BOOST_LOAD_COUNT] = {};
	/**
	 * units: %
	 * offset 5388
	 */
	uint8_t pedalToTpsTable[PEDAL_TO_TPS_SIZE][PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %
	 * offset 5452
	 */
	uint8_t pedalToTpsPedalBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %
	 * offset 5460
	 */
	uint8_t pedalToTpsPedalSpeedBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5468
	 */
	scaled_channel<uint8_t, 1, 100> pedalToTpsRpmBins[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %/s
	 * offset 5476
	 */
	scaled_channel<uint8_t, 1, 10> etbMaxSpeedOpen[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * units: %/s
	 * offset 5484
	 */
	scaled_channel<uint8_t, 1, 10> etbMaxSpeedClose[PEDAL_TO_TPS_SIZE] = {};
	/**
	 * CLT-based cranking position multiplier for simple manual idle controller
	 * units: C
	 * offset 5492
	 */
	float cltCrankingCorrBins[CLT_CRANKING_CURVE_SIZE] = {};
	/**
	 * CLT-based cranking position multiplier for simple manual idle controller
	 * units: %
	 * offset 5524
	 */
	float cltCrankingCorr[CLT_CRANKING_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Idle (see useSeparateAdvanceForIdle)
	 * units: RPM
	 * offset 5556
	 */
	scaled_channel<uint8_t, 1, 50> idleAdvanceBins[IDLE_ADVANCE_CURVE_SIZE] = {};
	/**
	 * Optional timing advance table for Idle (see useSeparateAdvanceForIdle)
	 * units: deg
	 * offset 5564
	 */
	float idleAdvance[IDLE_ADVANCE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 5596
	 */
	scaled_channel<uint8_t, 1, 10> idleVeRpmBins[IDLE_VE_SIZE] = {};
	/**
	 * units: load
	 * offset 5600
	 */
	uint8_t idleVeLoadBins[IDLE_VE_SIZE] = {};
	/**
	 * units: %
	 * offset 5604
	 */
	scaled_channel<uint16_t, 10, 1> idleVeTable[IDLE_VE_SIZE][IDLE_VE_SIZE] = {};
	/**
	 * offset 5636
	 */
	lua_script_t luaScript;
	/**
	 * units: C
	 * offset 15636
	 */
	float cltFuelCorrBins[CLT_FUEL_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 15700
	 */
	float cltFuelCorr[CLT_FUEL_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 15764
	 */
	float iatFuelCorrBins[IAT_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 15828
	 */
	float iatFuelCorr[IAT_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 15892
	 */
	float crankingFuelCoef[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 15924
	 */
	float crankingFuelBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: counter
	 * offset 15956
	 */
	float crankingCycleBins[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 15988
	 */
	int16_t crankingCycleFuelCltBins[CRANKING_CYCLE_CLT_SIZE] = {};
	/**
	 * units: mult
	 * offset 15996
	 */
	float crankingCycleFuelCoef[CRANKING_CYCLE_CLT_SIZE][CRANKING_CURVE_SIZE] = {};
	/**
	 * RPM-based idle position
	 * units: RPM
	 * offset 16124
	 */
	scaled_channel<uint8_t, 1, 50> rpmIdleCorrBins[CLT_CURVE_SIZE] = {};
	/**
	 * CLT-based idle position for simple manual idle controller
	 * units: C
	 * offset 16140
	 */
	uint16_t cltIdleCorrBins[CLT_CURVE_SIZE] = {};
	/**
	 * CLT-based idle position for simple manual idle controller
	 * units: %
	 * offset 16172
	 */
	scaled_channel<uint16_t, 10, 1> cltIdleCorrTable[CLT_CURVE_SIZE][CLT_CURVE_SIZE] = {};
	/**
	 * Long Term Idle Trim (LTIT) multiplicativo para idle open loop
	 * units: %
	 * offset 16684
	 */
	scaled_channel<uint16_t, 10, 1> ltitTable[CLT_CURVE_SIZE] = {};
	/**
	 * Also known as MAF transfer function.
	 * kg/hour value.
	 * By the way 2.081989116 kg/h = 1 ft3/m
	 * units: kg/hour
	 * offset 16716
	 */
	float mafDecoding[MAF_DECODING_COUNT] = {};
	/**
	 * units: V
	 * offset 16844
	 */
	float mafDecodingBins[MAF_DECODING_COUNT] = {};
	/**
	 * units: deg
	 * offset 16972
	 */
	scaled_channel<int16_t, 10, 1> ignitionIatCorrTable[IAT_IGN_CORR_COUNT][IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: C
	 * offset 17100
	 */
	int8_t ignitionIatCorrTempBins[IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: Load
	 * offset 17108
	 */
	scaled_channel<uint8_t, 1, 5> ignitionIatCorrLoadBins[IAT_IGN_CORR_COUNT] = {};
	/**
	 * units: deg
	 * offset 17116
	 */
	int16_t injectionPhase[INJ_PHASE_LOAD_COUNT][INJ_PHASE_RPM_COUNT] = {};
	/**
	 * units: Load
	 * offset 17628
	 */
	uint16_t injPhaseLoadBins[INJ_PHASE_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 17660
	 */
	uint16_t injPhaseRpmBins[INJ_PHASE_RPM_COUNT] = {};
	/**
	 * units: onoff
	 * offset 17692
	 */
	uint8_t tcuSolenoidTable[TCU_SOLENOID_COUNT][TCU_GEAR_COUNT] = {};
	/**
	 * units: kPa
	 * offset 17752
	 */
	scaled_channel<uint16_t, 100, 1> mapEstimateTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * units: % TPS
	 * offset 18264
	 */
	scaled_channel<uint16_t, 100, 1> mapEstimateTpsBins[FUEL_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 18296
	 */
	uint16_t mapEstimateRpmBins[FUEL_RPM_COUNT] = {};
	/**
	 * units: value
	 * offset 18328
	 */
	int8_t vvtTable1[VVT_TABLE_SIZE][VVT_TABLE_SIZE] = {};
	/**
	 * units: L
	 * offset 18392
	 */
	uint16_t vvtTable1LoadBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 18408
	 */
	uint16_t vvtTable1RpmBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: value
	 * offset 18424
	 */
	int8_t vvtTable2[VVT_TABLE_SIZE][VVT_TABLE_SIZE] = {};
	/**
	 * units: L
	 * offset 18488
	 */
	uint16_t vvtTable2LoadBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 18504
	 */
	uint16_t vvtTable2RpmBins[VVT_TABLE_SIZE] = {};
	/**
	 * units: deg
	 * offset 18520
	 */
	scaled_channel<int16_t, 10, 1> ignitionTable[IGN_LOAD_COUNT][IGN_RPM_COUNT] = {};
	/**
	 * units: Load
	 * offset 19032
	 */
	uint16_t ignitionLoadBins[IGN_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 19064
	 */
	uint16_t ignitionRpmBins[IGN_RPM_COUNT] = {};
	/**
	 * units: %
	 * offset 19096
	 */
	scaled_channel<uint16_t, 10, 1> veTable[VE_LOAD_COUNT][VE_RPM_COUNT] = {};
	/**
	 * units: {bitStringValue(fuelUnits, fuelAlgorithm) }
	 * offset 19608
	 */
	uint16_t veLoadBins[VE_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 19640
	 */
	uint16_t veRpmBins[VE_RPM_COUNT] = {};
	/**
	 * units: %
	 * offset 19672
	 */
	scaled_channel<uint16_t, 10, 1> ltftTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * units: 10x%/s
	 * offset 20184
	 */
	scaled_channel<uint16_t, 10, 1> ltftCorrectionRate[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	offset 20696 bit 0 */
	bool ltftEnabled : 1 {};
	/**
	offset 20696 bit 1 */
	bool unusedBit_94_1 : 1 {};
	/**
	offset 20696 bit 2 */
	bool unusedBit_94_2 : 1 {};
	/**
	offset 20696 bit 3 */
	bool unusedBit_94_3 : 1 {};
	/**
	offset 20696 bit 4 */
	bool unusedBit_94_4 : 1 {};
	/**
	offset 20696 bit 5 */
	bool unusedBit_94_5 : 1 {};
	/**
	offset 20696 bit 6 */
	bool unusedBit_94_6 : 1 {};
	/**
	offset 20696 bit 7 */
	bool unusedBit_94_7 : 1 {};
	/**
	offset 20696 bit 8 */
	bool unusedBit_94_8 : 1 {};
	/**
	offset 20696 bit 9 */
	bool unusedBit_94_9 : 1 {};
	/**
	offset 20696 bit 10 */
	bool unusedBit_94_10 : 1 {};
	/**
	offset 20696 bit 11 */
	bool unusedBit_94_11 : 1 {};
	/**
	offset 20696 bit 12 */
	bool unusedBit_94_12 : 1 {};
	/**
	offset 20696 bit 13 */
	bool unusedBit_94_13 : 1 {};
	/**
	offset 20696 bit 14 */
	bool unusedBit_94_14 : 1 {};
	/**
	offset 20696 bit 15 */
	bool unusedBit_94_15 : 1 {};
	/**
	offset 20696 bit 16 */
	bool unusedBit_94_16 : 1 {};
	/**
	offset 20696 bit 17 */
	bool unusedBit_94_17 : 1 {};
	/**
	offset 20696 bit 18 */
	bool unusedBit_94_18 : 1 {};
	/**
	offset 20696 bit 19 */
	bool unusedBit_94_19 : 1 {};
	/**
	offset 20696 bit 20 */
	bool unusedBit_94_20 : 1 {};
	/**
	offset 20696 bit 21 */
	bool unusedBit_94_21 : 1 {};
	/**
	offset 20696 bit 22 */
	bool unusedBit_94_22 : 1 {};
	/**
	offset 20696 bit 23 */
	bool unusedBit_94_23 : 1 {};
	/**
	offset 20696 bit 24 */
	bool unusedBit_94_24 : 1 {};
	/**
	offset 20696 bit 25 */
	bool unusedBit_94_25 : 1 {};
	/**
	offset 20696 bit 26 */
	bool unusedBit_94_26 : 1 {};
	/**
	offset 20696 bit 27 */
	bool unusedBit_94_27 : 1 {};
	/**
	offset 20696 bit 28 */
	bool unusedBit_94_28 : 1 {};
	/**
	offset 20696 bit 29 */
	bool unusedBit_94_29 : 1 {};
	/**
	offset 20696 bit 30 */
	bool unusedBit_94_30 : 1 {};
	/**
	offset 20696 bit 31 */
	bool unusedBit_94_31 : 1 {};
	/**
	 * units: value
	 * offset 20700
	 */
	uint8_t ltftCRC;
	/**
	 * Minimum temperature to start correcting ltft tables
	 * units: deg C
	 * offset 20701
	 */
	uint8_t ltftMinModTemp;
	/**
	 * Minimum temperature to start using ltft tables
	 * units: deg C
	 * offset 20702
	 */
	uint8_t ltftMinTemp;
	/**
	 * How much long term fuel trim should act to reduce short term fuel trim, 100 should keep stft in about 3%, 255 in 8% and 33 in 1%, and
	 * units: %
	 * offset 20703
	 */
	uint8_t ltftPermissivity;
	/**
	 * units: %
	 * offset 20704
	 */
	uint8_t ltftMaxCorrection;
	/**
	 * units: %
	 * offset 20705
	 */
	uint8_t ltftMinCorrection;
	/**
	 * Constante de tempo do filtro EMA para STFT usado no LTFT
	 * units: 0-255
	 * offset 20706
	 */
	uint8_t ltftEmaAlpha;
	/**
	 * Limiar de rejeição para STFT (em %)
	 * units: %
	 * offset 20707
	 */
	uint8_t ltftStftRejectThreshold;
	/**
	 * Delay pós-ignição ON para aprendizado/aplicação do LTFT
	 * units: s
	 * offset 20708
	 */
	uint8_t ltftIgnitionOnDelay;
	/**
	 * Delay após ignição OFF para salvamento do LTFT
	 * units: s
	 * offset 20709
	 */
	uint8_t ltftIgnitionOffSaveDelay;
	/**
	 * units: lambda
	 * offset 20710
	 */
	scaled_channel<uint8_t, 147, 1> lambdaTable[FUEL_LOAD_COUNT][FUEL_RPM_COUNT] = {};
	/**
	 * offset 20966
	 */
	uint16_t lambdaLoadBins[FUEL_LOAD_COUNT] = {};
	/**
	 * units: RPM
	 * offset 20998
	 */
	uint16_t lambdaRpmBins[FUEL_RPM_COUNT] = {};
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 21030
	 */
	uint8_t alignmentFill_at_21030[2] = {};
	/**
	 * units: value
	 * offset 21032
	 */
	float tpsTpsAccelTable[TPS_TPS_ACCEL_TABLE][TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: from
	 * offset 21288
	 */
	float tpsTpsAccelFromRpmBins[TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: to
	 * offset 21320
	 */
	float tpsTpsAccelToRpmBins[TPS_TPS_ACCEL_TABLE] = {};
	/**
	 * units: value
	 * offset 21352
	 */
	float scriptTable1[SCRIPT_TABLE_8][SCRIPT_TABLE_8] = {};
	/**
	 * units: L
	 * offset 21608
	 */
	int16_t scriptTable1LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 21624
	 */
	int16_t scriptTable1RpmBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: value
	 * offset 21640
	 */
	float scriptTable2[TABLE_2_LOAD_SIZE][TABLE_2_RPM_SIZE] = {};
	/**
	 * units: L
	 * offset 21896
	 */
	int16_t scriptTable2LoadBins[TABLE_2_LOAD_SIZE] = {};
	/**
	 * units: RPM
	 * offset 21912
	 */
	int16_t scriptTable2RpmBins[TABLE_2_RPM_SIZE] = {};
	/**
	 * units: value
	 * offset 21928
	 */
	uint8_t scriptTable3[SCRIPT_TABLE_8][SCRIPT_TABLE_8] = {};
	/**
	 * units: L
	 * offset 21992
	 */
	int16_t scriptTable3LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 22008
	 */
	int16_t scriptTable3RpmBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: value
	 * offset 22024
	 */
	uint8_t scriptTable4[SCRIPT_TABLE_8][TABLE_4_RPM] = {};
	/**
	 * units: L
	 * offset 22104
	 */
	int16_t scriptTable4LoadBins[SCRIPT_TABLE_8] = {};
	/**
	 * units: RPM
	 * offset 22120
	 */
	int16_t scriptTable4RpmBins[TABLE_4_RPM] = {};
	/**
	 * offset 22140
	 */
	uint16_t ignTrimLoadBins[IGN_TRIM_SIZE] = {};
	/**
	 * units: rpm
	 * offset 22148
	 */
	uint16_t ignTrimRpmBins[IGN_TRIM_SIZE] = {};
	/**
	 * offset 22156
	 */
	ign_cyl_trim_s ignTrims[12] = {};
	/**
	 * offset 22348
	 */
	uint16_t fuelTrimLoadBins[FUEL_TRIM_SIZE] = {};
	/**
	 * units: rpm
	 * offset 22356
	 */
	uint16_t fuelTrimRpmBins[FUEL_TRIM_SIZE] = {};
	/**
	 * offset 22364
	 */
	fuel_cyl_trim_s fuelTrims[12] = {};
	/**
	 * units: ratio
	 * offset 22556
	 */
	scaled_channel<uint16_t, 100, 1> crankingFuelCoefE100[CRANKING_CURVE_SIZE] = {};
	/**
	 * units: Airmass
	 * offset 22572
	 */
	scaled_channel<uint8_t, 1, 5> tcu_pcAirmassBins[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22580
	 */
	uint8_t tcu_pcValsR[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22588
	 */
	uint8_t tcu_pcValsN[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22596
	 */
	uint8_t tcu_pcVals1[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22604
	 */
	uint8_t tcu_pcVals2[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22612
	 */
	uint8_t tcu_pcVals3[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22620
	 */
	uint8_t tcu_pcVals4[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22628
	 */
	uint8_t tcu_pcVals12[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22636
	 */
	uint8_t tcu_pcVals23[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22644
	 */
	uint8_t tcu_pcVals34[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22652
	 */
	uint8_t tcu_pcVals21[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22660
	 */
	uint8_t tcu_pcVals32[TCU_TABLE_WIDTH] = {};
	/**
	 * units: %
	 * offset 22668
	 */
	uint8_t tcu_pcVals43[TCU_TABLE_WIDTH] = {};
	/**
	 * units: TPS
	 * offset 22676
	 */
	uint8_t tcu_tccTpsBins[8] = {};
	/**
	 * units: MPH
	 * offset 22684
	 */
	uint8_t tcu_tccLockSpeed[8] = {};
	/**
	 * units: MPH
	 * offset 22692
	 */
	uint8_t tcu_tccUnlockSpeed[8] = {};
	/**
	 * units: KPH
	 * offset 22700
	 */
	uint8_t tcu_32SpeedBins[8] = {};
	/**
	 * units: %
	 * offset 22708
	 */
	uint8_t tcu_32Vals[8] = {};
	/**
	 * units: %
	 * offset 22716
	 */
	scaled_channel<int8_t, 10, 1> throttle2TrimTable[ETB2_TRIM_SIZE][ETB2_TRIM_SIZE] = {};
	/**
	 * units: %
	 * offset 22752
	 */
	uint8_t throttle2TrimTpsBins[ETB2_TRIM_SIZE] = {};
	/**
	 * units: RPM
	 * offset 22758
	 */
	scaled_channel<uint8_t, 1, 100> throttle2TrimRpmBins[ETB2_TRIM_SIZE] = {};
	/**
	 * units: deg
	 * offset 22764
	 */
	scaled_channel<uint8_t, 4, 1> maxKnockRetardTable[KNOCK_TABLE_SIZE][KNOCK_TABLE_SIZE] = {};
	/**
	 * units: %
	 * offset 22800
	 */
	uint8_t maxKnockRetardLoadBins[KNOCK_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 22806
	 */
	scaled_channel<uint8_t, 1, 100> maxKnockRetardRpmBins[KNOCK_TABLE_SIZE] = {};
	/**
	 * units: deg
	 * offset 22812
	 */
	scaled_channel<int16_t, 10, 1> ALSTimingRetardTable[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 22844
	 */
	uint16_t alsIgnRetardLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 22852
	 */
	uint16_t alsIgnRetardrpmBins[ALS_SIZE] = {};
	/**
	 * units: percent
	 * offset 22860
	 */
	scaled_channel<int16_t, 10, 1> ALSFuelAdjustment[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 22892
	 */
	uint16_t alsFuelAdjustmentLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 22900
	 */
	uint16_t alsFuelAdjustmentrpmBins[ALS_SIZE] = {};
	/**
	 * units: ratio
	 * offset 22908
	 */
	scaled_channel<int16_t, 1, 10> ALSIgnSkipTable[ALS_SIZE][ALS_SIZE] = {};
	/**
	 * units: TPS
	 * offset 22940
	 */
	uint16_t alsIgnSkipLoadBins[ALS_SIZE] = {};
	/**
	 * units: RPM
	 * offset 22948
	 */
	uint16_t alsIgnSkiprpmBins[ALS_SIZE] = {};
	/**
	 * offset 22956
	 */
	blend_table_s ignBlends[IGN_BLEND_COUNT] = {};
	/**
	 * offset 23332
	 */
	blend_table_s veBlends[VE_BLEND_COUNT] = {};
	/**
	 * units: %
	 * offset 24084
	 */
	scaled_channel<uint16_t, 10, 1> throttleEstimateEffectiveAreaBins[THR_EST_SIZE] = {};
	/**
	 * In units of g/s normalized to choked flow conditions
	 * units: g/s
	 * offset 24108
	 */
	scaled_channel<uint16_t, 10, 1> throttleEstimateEffectiveAreaValues[THR_EST_SIZE] = {};
	/**
	 * offset 24132
	 */
	blend_table_s boostOpenLoopBlends[BOOST_BLEND_COUNT] = {};
	/**
	 * offset 24508
	 */
	blend_table_s boostClosedLoopBlends[BOOST_BLEND_COUNT] = {};
	/**
	 * units: level
	 * offset 24884
	 */
	float tcu_rangeP[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 24908
	 */
	float tcu_rangeR[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 24932
	 */
	float tcu_rangeN[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 24956
	 */
	float tcu_rangeD[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 24980
	 */
	float tcu_rangeM[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25004
	 */
	float tcu_rangeM3[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25028
	 */
	float tcu_rangeM2[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25052
	 */
	float tcu_rangeM1[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25076
	 */
	float tcu_rangePlus[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25100
	 */
	float tcu_rangeMinus[RANGE_INPUT_COUNT] = {};
	/**
	 * units: level
	 * offset 25124
	 */
	float tcu_rangeLow[RANGE_INPUT_COUNT] = {};
	/**
	 * units: lambda
	 * offset 25148
	 */
	scaled_channel<uint8_t, 100, 1> lambdaMaxDeviationTable[LAM_SIZE][LAM_SIZE] = {};
	/**
	 * offset 25164
	 */
	uint16_t lambdaMaxDeviationLoadBins[LAM_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25172
	 */
	uint16_t lambdaMaxDeviationRpmBins[LAM_SIZE] = {};
	/**
	 * units: %
	 * offset 25180
	 */
	uint8_t injectorStagingTable[INJ_STAGING_COUNT][INJ_STAGING_COUNT] = {};
	/**
	 * offset 25216
	 */
	uint16_t injectorStagingLoadBins[INJ_STAGING_COUNT] = {};
	/**
	 * units: RPM
	 * offset 25228
	 */
	uint16_t injectorStagingRpmBins[INJ_STAGING_COUNT] = {};
	/**
	 * units: deg C
	 * offset 25240
	 */
	int8_t wwCltBins[WWAE_TABLE_SIZE] = {};
	/**
	 * offset 25248
	 */
	scaled_channel<uint8_t, 100, 1> wwTauCltValues[WWAE_TABLE_SIZE] = {};
	/**
	 * offset 25256
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaCltValues[WWAE_TABLE_SIZE] = {};
	/**
	 * units: kPa
	 * offset 25264
	 */
	int8_t wwMapBins[WWAE_TABLE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25272
	 */
	scaled_channel<uint8_t, 1, 50> wwRpmBins[WWAE_RPM_SIZE] = {};
	/**
	 * offset 25280
	 */
	scaled_channel<uint8_t, 100, 1> wwTauMapRpmValues[WWAE_RPM_SIZE][WWAE_TABLE_SIZE] = {};
	/**
	 * offset 25344
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaMapRpmValues[WWAE_RPM_SIZE][WWAE_TABLE_SIZE] = {};
	/**
	 * units: kPa
	 * offset 25408
	 */
	scaled_channel<uint8_t, 1, 1> wwCorrectionMapBins[WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25416
	 */
	scaled_channel<uint8_t, 1, 50> wwCorrectionRpmBins[WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: Tau correction multiplier
	 * offset 25424
	 */
	scaled_channel<uint8_t, 100, 1> wwTauCorrection[WWAE_CORRECTION_SIZE][WWAE_CORRECTION_SIZE] = {};
	/**
	 * units: Beta correction multiplier
	 * offset 25488
	 */
	scaled_channel<uint8_t, 100, 1> wwBetaCorrection[WWAE_CORRECTION_SIZE][WWAE_CORRECTION_SIZE] = {};
	/**
	 * 200
	 * units: Wall Wetting Buffer Size
	 * offset 25552
	 */
	uint16_t wwBufferSize;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 25554
	 */
	uint8_t alignmentFill_at_25554[2] = {};
	/**
	 * "Enable directional wall wetting corrections"
	offset 25556 bit 0 */
	bool wwDirectionalCorrections : 1 {};
	/**
	offset 25556 bit 1 */
	bool unusedBit_231_1 : 1 {};
	/**
	offset 25556 bit 2 */
	bool unusedBit_231_2 : 1 {};
	/**
	offset 25556 bit 3 */
	bool unusedBit_231_3 : 1 {};
	/**
	offset 25556 bit 4 */
	bool unusedBit_231_4 : 1 {};
	/**
	offset 25556 bit 5 */
	bool unusedBit_231_5 : 1 {};
	/**
	offset 25556 bit 6 */
	bool unusedBit_231_6 : 1 {};
	/**
	offset 25556 bit 7 */
	bool unusedBit_231_7 : 1 {};
	/**
	offset 25556 bit 8 */
	bool unusedBit_231_8 : 1 {};
	/**
	offset 25556 bit 9 */
	bool unusedBit_231_9 : 1 {};
	/**
	offset 25556 bit 10 */
	bool unusedBit_231_10 : 1 {};
	/**
	offset 25556 bit 11 */
	bool unusedBit_231_11 : 1 {};
	/**
	offset 25556 bit 12 */
	bool unusedBit_231_12 : 1 {};
	/**
	offset 25556 bit 13 */
	bool unusedBit_231_13 : 1 {};
	/**
	offset 25556 bit 14 */
	bool unusedBit_231_14 : 1 {};
	/**
	offset 25556 bit 15 */
	bool unusedBit_231_15 : 1 {};
	/**
	offset 25556 bit 16 */
	bool unusedBit_231_16 : 1 {};
	/**
	offset 25556 bit 17 */
	bool unusedBit_231_17 : 1 {};
	/**
	offset 25556 bit 18 */
	bool unusedBit_231_18 : 1 {};
	/**
	offset 25556 bit 19 */
	bool unusedBit_231_19 : 1 {};
	/**
	offset 25556 bit 20 */
	bool unusedBit_231_20 : 1 {};
	/**
	offset 25556 bit 21 */
	bool unusedBit_231_21 : 1 {};
	/**
	offset 25556 bit 22 */
	bool unusedBit_231_22 : 1 {};
	/**
	offset 25556 bit 23 */
	bool unusedBit_231_23 : 1 {};
	/**
	offset 25556 bit 24 */
	bool unusedBit_231_24 : 1 {};
	/**
	offset 25556 bit 25 */
	bool unusedBit_231_25 : 1 {};
	/**
	offset 25556 bit 26 */
	bool unusedBit_231_26 : 1 {};
	/**
	offset 25556 bit 27 */
	bool unusedBit_231_27 : 1 {};
	/**
	offset 25556 bit 28 */
	bool unusedBit_231_28 : 1 {};
	/**
	offset 25556 bit 29 */
	bool unusedBit_231_29 : 1 {};
	/**
	offset 25556 bit 30 */
	bool unusedBit_231_30 : 1 {};
	/**
	offset 25556 bit 31 */
	bool unusedBit_231_31 : 1 {};
	/**
	 * units: %
	 * offset 25560
	 */
	scaled_channel<uint8_t, 2, 1> hpfpLobeProfileQuantityBins[HPFP_LOBE_PROFILE_SIZE] = {};
	/**
	 * units: deg
	 * offset 25576
	 */
	scaled_channel<uint8_t, 2, 1> hpfpLobeProfileAngle[HPFP_LOBE_PROFILE_SIZE] = {};
	/**
	 * units: volts
	 * offset 25592
	 */
	uint8_t hpfpDeadtimeVoltsBins[HPFP_DEADTIME_SIZE] = {};
	/**
	 * units: ms
	 * offset 25600
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpDeadtimeMS[HPFP_DEADTIME_SIZE] = {};
	/**
	 * units: kPa
	 * offset 25616
	 */
	uint16_t hpfpTarget[HPFP_TARGET_SIZE][HPFP_TARGET_SIZE] = {};
	/**
	 * units: load
	 * offset 25816
	 */
	scaled_channel<uint16_t, 10, 1> hpfpTargetLoadBins[HPFP_TARGET_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25836
	 */
	scaled_channel<uint16_t, 1, 50> hpfpTargetRpmBins[HPFP_TARGET_SIZE] = {};
	/**
	 * units: %
	 * offset 25856
	 */
	int8_t hpfpCompensation[HPFP_COMPENSATION_SIZE][HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: cc/lobe
	 * offset 25956
	 */
	scaled_channel<uint16_t, 1000, 1> hpfpCompensationLoadBins[HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25976
	 */
	scaled_channel<uint16_t, 1, 50> hpfpCompensationRpmBins[HPFP_COMPENSATION_SIZE] = {};
	/**
	 * units: RPM
	 * offset 25996
	 */
	uint16_t knockNoiseRpmBins[ENGINE_NOISE_CURVE_SIZE] = {};
	/**
	 * Knock sensor output knock detection threshold depending on current RPM.
	 * units: dB
	 * offset 26028
	 */
	scaled_channel<int8_t, 2, 1> knockBaseNoise[ENGINE_NOISE_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 26044
	 */
	scaled_channel<uint8_t, 1, 50> tpsTspCorrValuesBins[TPS_TPS_ACCEL_RPM_CORR_TABLE] = {};
	/**
	 * units: multiplier
	 * offset 26048
	 */
	scaled_channel<uint8_t, 50, 1> tpsTspCorrValues[TPS_TPS_ACCEL_RPM_CORR_TABLE] = {};
	/**
	 * units: C
	 * offset 26052
	 */
	scaled_channel<uint8_t, 1, 1> tpsAcelEctBins[TPS_TPS_ACCEL_CLT_CORR_TABLE] = {};
	/**
	 * units: multiplier
	 * offset 26060
	 */
	scaled_channel<uint8_t, 50, 1> tpsAcelEctValues[TPS_TPS_ACCEL_CLT_CORR_TABLE] = {};
	/**
	 * units: C
	 * offset 26068
	 */
	int8_t cltRevLimitRpmBins[CLT_LIMITER_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 26072
	 */
	uint16_t cltRevLimitRpm[CLT_LIMITER_CURVE_SIZE] = {};
	/**
	 * units: volt
	 * offset 26080
	 */
	scaled_channel<uint16_t, 1000, 1> fuelLevelBins[FUEL_LEVEL_TABLE_COUNT] = {};
	/**
	 * units: %
	 * offset 26096
	 */
	uint8_t fuelLevelValues[FUEL_LEVEL_TABLE_COUNT] = {};
	/**
	 * units: volts
	 * offset 26104
	 */
	scaled_channel<uint8_t, 10, 1> dwellVoltageCorrVoltBins[DWELL_CURVE_SIZE] = {};
	/**
	 * units: multiplier
	 * offset 26112
	 */
	scaled_channel<uint8_t, 50, 1> dwellVoltageCorrValues[DWELL_CURVE_SIZE] = {};
	/**
	 * units: %
	 * offset 26120
	 */
	scaled_channel<uint8_t, 1, 1> tcu_shiftTpsBins[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26128
	 */
	uint8_t tcu_shiftSpeed12[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26136
	 */
	uint8_t tcu_shiftSpeed23[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26144
	 */
	uint8_t tcu_shiftSpeed34[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26152
	 */
	uint8_t tcu_shiftSpeed21[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26160
	 */
	uint8_t tcu_shiftSpeed32[TCU_TABLE_WIDTH] = {};
	/**
	 * units: MPH
	 * offset 26168
	 */
	uint8_t tcu_shiftSpeed43[TCU_TABLE_WIDTH] = {};
	/**
	 * units: ms
	 * offset 26176
	 */
	float tcu_shiftTime;
	/**
	 * units: Volts
	 * offset 26180
	 */
	scaled_channel<int16_t, 10, 1> alternatorVoltageTargetTable[ALTERNATOR_VOLTAGE_TARGET_SIZE][ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: Load
	 * offset 26212
	 */
	uint16_t alternatorVoltageTargetLoadBins[ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: RPM
	 * offset 26220
	 */
	uint16_t alternatorVoltageTargetRpmBins[ALTERNATOR_VOLTAGE_TARGET_SIZE] = {};
	/**
	 * units: C
	 * offset 26228
	 */
	float cltBoostCorrBins[BOOST_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 26248
	 */
	float cltBoostCorr[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 26268
	 */
	float iatBoostCorrBins[BOOST_CURVE_SIZE] = {};
	/**
	 * units: ratio
	 * offset 26288
	 */
	float iatBoostCorr[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 26308
	 */
	float cltBoostAdderBins[BOOST_CURVE_SIZE] = {};
	/**
	 * offset 26328
	 */
	float cltBoostAdder[BOOST_CURVE_SIZE] = {};
	/**
	 * units: C
	 * offset 26348
	 */
	float iatBoostAdderBins[BOOST_CURVE_SIZE] = {};
	/**
	 * offset 26368
	 */
	float iatBoostAdder[BOOST_CURVE_SIZE] = {};
	/**
	 * units: RPM
	 * offset 26388
	 */
	scaled_channel<uint8_t, 1, 100> minimumOilPressureBins[8] = {};
	/**
	 * units: kPa
	 * offset 26396
	 */
	scaled_channel<uint8_t, 1, 10> minimumOilPressureValues[8] = {};
	/**
	 * offset 26404
	 */
	blend_table_s targetAfrBlends[TARGET_AFR_BLEND_COUNT] = {};
	/**
	 * units: RPM
	 * offset 26780
	 */
	scaled_channel<uint8_t, 1, 100> trimRpmBins[FUEL_TRIM_RPM_COUNT] = {};
	/**
	 * offset 26788
	 */
	uint16_t trimLoadBins[FUEL_TRIM_LOAD_COUNT] = {};
	/**
	 * @@DYNO_RPM_STEP_TOOLTIP@@
	 * units: Rpm
	 * offset 26804
	 */
	scaled_channel<uint8_t, 1, 1> dynoRpmStep;
	/**
	 * @@DYNO_SAE_TEMPERATURE_C_TOOLTIP@@
	 * units: C
	 * offset 26805
	 */
	scaled_channel<int8_t, 1, 1> dynoSaeTemperatureC;
	/**
	 * @@DYNO_SAE_RELATIVE_HUMIDITY_TOOLTIP@@
	 * units: %
	 * offset 26806
	 */
	scaled_channel<uint8_t, 1, 1> dynoSaeRelativeHumidity;
	/**
	 * need 4 byte alignment
	 * units: units
	 * offset 26807
	 */
	uint8_t alignmentFill_at_26807[1] = {};
	/**
	 * @@DYNO_SAE_BARO_TOOLTIP@@
	 * units: KPa
	 * offset 26808
	 */
	scaled_channel<float, 1, 1> dynoSaeBaro;
	/**
	 * @@DYNO_CAR_WHEEL_DIA_INCH_TOOLTIP@@
	 * units: Inch
	 * offset 26812
	 */
	scaled_channel<int8_t, 1, 1> dynoCarWheelDiaInch;
	/**
	 * @@DYNO_CAR_WHEEL_ASPECT_RATIO_TOOLTIP@@
	 * units: Aspect Ratio (height)
	 * offset 26813
	 */
	scaled_channel<int8_t, 1, 1> dynoCarWheelAspectRatio;
	/**
	 * @@DYNO_CAR_WHEEL_TIRE_WIDTH_TOOLTIP@@
	 * units: Width mm
	 * offset 26814
	 */
	scaled_channel<int16_t, 1, 1> dynoCarWheelTireWidthMm;
	/**
	 * @@DYNO_CAR_GEAR_PRIMARY_REDUCTION_TOOLTIP@@
	 * units: Units
	 * offset 26816
	 */
	scaled_channel<float, 1, 1> dynoCarGearPrimaryReduction;
	/**
	 * @@DYNO_CAR_GEAR_RATIO_TOOLTIP@@
	 * units: Units
	 * offset 26820
	 */
	scaled_channel<float, 1, 1> dynoCarGearRatio;
	/**
	 * @@DYNO_CAR_GEAR_FINAL_DRIVE_TOOLTIP@@
	 * units: Units
	 * offset 26824
	 */
	scaled_channel<float, 1, 1> dynoCarGearFinalDrive;
	/**
	 * @@DYNO_CAR_CAR_MASS_TOOLTIP@@
	 * units: Kg
	 * offset 26828
	 */
	scaled_channel<int16_t, 1, 1> dynoCarCarMassKg;
	/**
	 * @@DYNO_CAR_CARGO_MASS_TOOLTIP@@
	 * units: Kg
	 * offset 26830
	 */
	scaled_channel<int16_t, 1, 1> dynoCarCargoMassKg;
	/**
	 * @@DYNO_CAR_COEFF_OF_DRAG_TOOLTIP@@
	 * units: Coeff
	 * offset 26832
	 */
	scaled_channel<float, 1, 1> dynoCarCoeffOfDrag;
	/**
	 * @@DYNO_CAR_FRONTAL_AREA_TOOLTIP@@
	 * units: m2
	 * offset 26836
	 */
	scaled_channel<float, 1, 1> dynoCarFrontalAreaM2;
	/**
	 * units: deg
	 * offset 26840
	 */
	scaled_channel<int8_t, 10, 1> trailingSparkTable[TRAILING_SPARK_SIZE][TRAILING_SPARK_SIZE] = {};
	/**
	 * units: rpm
	 * offset 26856
	 */
	scaled_channel<uint8_t, 1, 50> trailingSparkRpmBins[TRAILING_SPARK_SIZE] = {};
	/**
	 * units: Load
	 * offset 26860
	 */
	scaled_channel<uint8_t, 1, 5> trailingSparkLoadBins[TRAILING_SPARK_SIZE] = {};
};
static_assert(sizeof(persistent_config_s) == 26864);

// end
// this section was generated automatically by rusEFI tool config_definition-all.jar based on gen_config.sh integration/evotech_config.txt
