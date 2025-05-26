#include "pch.h"
#include "closed_loop_fuel.h"
#include "closed_loop_fuel_cell.h"
#include "deadband.h"
#include "tunerstudio.h"
#include "engine_math.h"
#include "efitime.h"
#include "ignition_controller.h"

#if EFI_ENGINE_CONTROL

struct FuelingBank {
	ClosedLoopFuelCellImpl cells[STFT_CELL_COUNT];
};

static FuelingBank banks[STFT_BANK_COUNT];

static Deadband<25> idleDeadband;
static Deadband<2> overrunDeadband;
static Deadband<2> loadDeadband;

void LongTermFuelTrim::resetLtftTimer() {
	m_updateTimer.reset();
}

float LongTermFuelTrim::filterStft(float stftRaw) {
	// Filtro EMA: stftEma = alpha * stftRaw + (1 - alpha) * stftEma
	float alpha = (float)config->ltftEmaAlpha / 255.0f;
	stftEma = alpha * stftRaw + (1.0f - alpha) * stftEma;
	return stftEma;
}

void LongTermFuelTrim::resetStftFilter() {
	// Reset EMA filter to neutral value when LTFT is enabled/disabled
	stftEma = 0.0f;
}

float LongTermFuelTrim::computeCorrection(float stftFiltered, float correctionRate, float permissivity) {
	// Evitar divisão por zero e valores muito pequenos
	if (fabsf(stftFiltered) < 0.001f) return 0.0f;
	
	// Taxa de aprendizado adaptativa baseada na magnitude do erro
	// Usa exponencial negativa para suavizar a resposta
	float adaptiveRate = correctionRate * (1.0f - expf(-fabsf(stftFiltered) * permissivity / 100.0f));
	
	// Aplicar correção proporcional com saturação suave
	float correction = stftFiltered * adaptiveRate * 0.1f; // 10% por ciclo máximo
	
	// Limitar correção máxima por ciclo para estabilidade
	return clampF(-0.05f, correction, 0.05f); // ±5% máximo por ciclo
}

bool LongTermFuelTrim::canLearn() {
	// Só aprende se passou tempo suficiente desde ignição ON
	if (!m_ignitionOnTimer.hasElapsedSec(config->ltftIgnitionOnDelay)) {
        return false;
    }
    
	// Outras condições já existentes (temperatura, etc)
	if (!config->ltftEnabled) return false;
	if ((Sensor::get(SensorType::Clt)).value_or(0) < float(config->ltftMinModTemp)) return false;
	return true;
}

void LongTermFuelTrim::applyRegionalCorrection(float load, float rpm, float correction) {
	// Aplicar correção regional apenas se configurada e com erro significativo
	if(load < 10.0f || config->ltftRegionalIntensity == 0) return;
	
	auto binLoad = priv::getBin(load, config->veLoadBins);
	auto binRpm = priv::getBin(rpm, config->veRpmBins);
	
	int centerLoad = binLoad.Idx;
	int centerRpm = binRpm.Idx;
	int window = config->ltftRegionalWindow;
	float intensity = (float)config->ltftRegionalIntensity / 100.0f;
	
	// Aplicar correção regional com intensidade reduzida baseada na distância
	for (int i = 0; i < FUEL_LOAD_COUNT; i++) {
		for (int j = 0; j < FUEL_RPM_COUNT; j++) {
			int loadDist = abs(i - centerLoad);
			int rpmDist = abs(j - centerRpm);
			
			// Aplicar apenas dentro da janela configurada
			if (loadDist <= window/2 && rpmDist <= window/2) {
				float loadWeight = 1.0f - (float)loadDist / (window/2 + 1);
				float rpmWeight = 1.0f - (float)rpmDist / (window/2 + 1);
				float totalWeight = loadWeight * rpmWeight * intensity;
				
				// Aplicar correção ponderada
				ltftTableHelper[i][j] *= (1.0f + correction * totalWeight);
			}
		}
	}
}

void LongTermFuelTrim::smoothHoles() {
	// Ativar suavização se configurada
	if (!config->ltftEnabled || config->ltftSmoothingIntensity == 0) return;
	
	float intensity = (float)config->ltftSmoothingIntensity / 100.0f;
	smoothTable<float, FUEL_LOAD_COUNT, FUEL_RPM_COUNT>(ltftTableHelper, intensity);
}

// Função utilitária para checar se a ignição está ligada
/*
static bool isIgnitionOn() {
	auto ign = engine->module<IgnitionController>()->secondsSinceIgnVoltage() > 1.0f;
	return ign;
}
*/

void LongTermFuelTrim::updateLtft(float load, float rpm) {
	if (!canLearn()) return;
	
	// Verificar tempo mínimo entre atualizações (1 segundo)
	float updateIntervalSec = 1.0f;
	if (!m_updateTimer.hasElapsedSec(updateIntervalSec)) {
		return;
	}
	m_updateTimer.reset();
	
	auto binLoad = priv::getBin(load, config->veLoadBins);
	auto binRpm = priv::getBin(rpm, config->veRpmBins);
	int lowLoad = binLoad.Idx;
	float fracLoad = binLoad.Frac;
	int lowRpm = binRpm.Idx;
	float fracRpm = binRpm.Frac;
	
	// Validação adequada usando constantes do sistema
	if (lowLoad >= (FUEL_LOAD_COUNT-1) || lowRpm >= (FUEL_RPM_COUNT-1) ||
		fracLoad < 0.01f || fracLoad > 0.99f ||
		fracRpm < 0.01f || fracRpm > 0.99f) {
		return;
	}
	
	float stftRaw = engine->engineState.stftCorrection[0] - 1.0f;
	float stftFiltered = filterStft(stftRaw);
	
	// Rejeitar STFT muito grandes (ruído/transientes)
	if (fabsf(stftFiltered) > (float)config->ltftStftRejectThreshold / 100.0f) return;
	
	// Validação de consistência com lambda
	auto lambda = Sensor::get(SensorType::Lambda1);
	if (lambda.Valid) {
		float lambdaError = lambda.Value - engine->fuelComputer.targetLambda;
		// Verificar se STFT e lambda error têm sinais opostos (inconsistente)
		if ((stftFiltered > 0.0f && lambdaError < 0.0f) || (stftFiltered < 0.0f && lambdaError > 0.0f)) {
			return;
		}
	}
	
	float correctionRate = interpolate3d(
		config->ltftCorrectionRate,
		config->veLoadBins, load,
		config->veRpmBins, rpm
	) * 0.01f;
	
	// Usar nova fórmula de correção robusta
	float correction = computeCorrection(stftFiltered, correctionRate, config->ltftPermissivity);
	
	// Aplicar correção apenas se estiver dentro de limites razoáveis
	if (fabsf(correction) > 0.001f && fabsf(correction) <= 0.2f) {
		// Correção regional se padrão consistente
		if (fabsf(stftFiltered) > 0.05f) {
			applyRegionalCorrection(load, rpm, correction);
		}
		
		// Correção bilinear padrão
		ltftTableHelper[lowLoad][lowRpm]     *= (1 + correction * (1-fracLoad) * (1-fracRpm));
		ltftTableHelper[lowLoad+1][lowRpm]   *= (1 + correction * (fracLoad) * (1-fracRpm));
		ltftTableHelper[lowLoad][lowRpm+1]   *= (1 + correction * (1-fracLoad) * (fracRpm));
		ltftTableHelper[lowLoad+1][lowRpm+1] *= (1 + correction * (fracLoad) * (fracRpm));
		
		// Clamping usando constantes de configuração
		for(int i = 0; i < 2; i++){
			for (int j = 0; j < 2; j++) {
				float maxValue = 100.0f + (float)config->ltftMaxCorrection;
				float minValue = 100.0f - (float)config->ltftMinCorrection;
				ltftTableHelper[lowLoad+i][lowRpm+j] = clampF(minValue, ltftTableHelper[lowLoad+i][lowRpm+j], maxValue);
			}
		}
		
		// Após atualização da tabela, marcar aprendizado pendente
		m_pendingSave = true;
		smoothHoles();
	}
}

void LongTermFuelTrim::onIgnitionStateChanged(bool ignitionState) {
	m_ignitionState = ignitionState;
	
	if (ignitionState) {
		// Reset timers quando ignição liga
		m_ignitionOnTimer.reset();
		isLearnConditionsMet = false;
		m_pendingSave = false;
		
		// Reset filtro EMA se LTFT foi desabilitado e reabilitado
		if (config->ltftEnabled) {
			resetStftFilter();
		}
	} else if (m_pendingSave) {
		// Iniciar timer para delay de salvamento
		m_ignitionOffTimer.reset();
	}
}

void LongTermFuelTrim::onSlowCallback() {
	// Implementar salvamento com delay adequado após ignição OFF
	if (m_pendingSave && !m_ignitionState) {
		float saveDelaySeconds = config->ltftIgnitionOffSaveDelay;
		if (saveDelaySeconds <= 0) {
			saveDelaySeconds = 5.0f; // Valor padrão de 5 segundos
		}
		
		if (m_ignitionOffTimer.hasElapsedSec(saveDelaySeconds)) {
			copyTable(config->ltftTable, ltftTableHelper);
			setNeedToWriteConfiguration();
			m_pendingSave = false;
		}
	}
}

namespace {
	SensorType getSensorForBankIndex(size_t index) {
		switch (index) {
			case 0: return SensorType::Lambda1;
			case 1: return SensorType::Lambda2;
			default: return SensorType::Invalid;
		}
	}
}

size_t computeStftBin(float rpm, float load, stft_s& cfg) {
	// Low RPM -> idle
	if (idleDeadband.lt(rpm, cfg.maxIdleRegionRpm))
	{
		return 0;
	}

	// Low load -> overrun
	if (overrunDeadband.lt(load, cfg.maxOverrunLoad))
	{
		return 1;
	}

	// High load -> power
	if (loadDeadband.gt(load, cfg.minPowerLoad))
	{
		return 2;
	}

	// Default -> normal "in the middle" cell
	return 3;
}

namespace {
	bool checkIfTuningIsNow() {
#if EFI_TUNER_STUDIO
		const bool result = isTuningNow();
#else
		const bool result = false;
#endif /* EFI_TUNER_STUDIO */
		engine->outputChannels.isTuningNow = result;
		return result;
	}

	bool shouldCorrect() {
		const auto& cfg = engineConfiguration->stft;

		// User disable bit
		if (!engineConfiguration->fuelClosedLoopCorrectionEnabled) {
			return false;
		}

		// Don't correct if tuning seens to be happening
		if (checkIfTuningIsNow()) {
			return false;
		}

		// Don't correct if not running
		if (!engine->rpmCalculator.isRunning()) {
			return false;
		}

		// Startup delay - allow O2 sensor to warm up, etc
		if (cfg.startupDelay > engine->fuelComputer.running.timeSinceCrankingInSecs) {
			return false;
		}

		// Check that the engine is hot enough (and clt not failed)
		auto clt = Sensor::get(SensorType::Clt);
		if (!clt.Valid || clt.Value < cfg.minClt) {
			return false;
		}

		// If all was well, then we're enabled!
		return true;
	}
}

bool shouldUpdateCorrection(SensorType sensor) {
	const auto& cfg = engineConfiguration->stft;

	// Pause (but don't reset) correction if the AFR is off scale.
	// It's probably a transient and poorly tuned transient correction
	auto afr = Sensor::getOrZero(sensor) * STOICH_RATIO;
	if (!afr || afr < cfg.minAfr || afr > cfg.maxAfr) {
		return false;
	}

	// Pause correction if DFCO was active recently
	auto timeSinceDfco = engine->module<DfcoController>()->getTimeSinceCut();
	if (timeSinceDfco < engineConfiguration->noFuelTrimAfterDfcoTime) {
		return false;
	}

	// Pause correction if Accel enrichment was active recently
	auto timeSinceAccel = engine->module<TpsAccelEnrichment>()->getTimeSinceAcell();
	if (timeSinceAccel < engineConfiguration->noFuelTrimAfterAccelTime) {
		return false;
	}

	// Pause if some other cut was active recently
	auto timeSinceFuelCut = engine->module<LimpManager>()->getTimeSinceAnyCut();
	// TODO: should duration this be configurable?
	if (timeSinceFuelCut < 2) {
		return false;
	}

	return true;
}

ClosedLoopFuelResult fuelStftClosedLoopCorrection() {
	if (!shouldCorrect()) {
		return {};
	}

	ClosedLoopFuelResult result;

	for (size_t i = 0; i < STFT_BANK_COUNT; i++) {
		auto sensor = getSensorForBankIndex(i);

		if (!shouldUpdateCorrection(sensor)) {
			// TODO: should we leave this cell alone, or run it but not update it?
			continue;
		}

		if (!Sensor::hasSensor(sensor)) {
			continue;
		}

		auto tps = Sensor::get(SensorType::Tps1);
		auto rpm = Sensor::get(SensorType::Rpm);
		float load = getFuelingLoad();

		if (!(rpm.Valid && tps.Valid && !cisnan(load))) {
			continue;
		}

		auto binIdx = computeStftBin(rpm.Value, load, engineConfiguration->stft);

		auto& cell = banks[i].cells[binIdx];

		result.banks[i] = cell.getCorrection(sensor);
	}

	return result;
}

float LongTermFuelTrim::getLtft(float load, float rpm) {

	if(config->ltftCRC != 132) {
		setTable(config->ltftTable, 100);
		config->ltftCRC = 132;
		ltftTableHelperInit = false;
		setNeedToWriteConfiguration();
	}

	if(!ltftTableHelperInit){
		copyTable(ltftTableHelper, config->ltftTable, 1);
		ltftTableHelperInit = true;
	}

	if(shouldUpdateCorrection(getSensorForBankIndex(0)) && shouldCorrect()) {
		updateLtft(load, rpm);
	} else {
		resetLtftTimer();
	}

	if(config->ltftEnabled && config->ltftCRC == 132 && (Sensor::get(SensorType::Clt)).value_or(0) > float(config->ltftMinTemp)) {
		float ltft = interpolate3d(ltftTableHelper,
			  config->veLoadBins, load,
			  config->veRpmBins, rpm
		) * 0.01f;

		return ltft;
	} else {
		return 1.00f;
	}
}

LongTermFuelTrim::LongTermFuelTrim() {
	stftEma = 0.0f; // Inicializar filtro EMA em valor neutro
	m_ignitionOnTimer.reset();
	m_ignitionOffTimer.reset();
	m_updateTimer.reset();
	m_ignitionState = false;
	isLearnConditionsMet = false;
	ltftTableHelperInit = false;
	m_pendingSave = false;
}

#endif // EFI_ENGINE_CONTROL
