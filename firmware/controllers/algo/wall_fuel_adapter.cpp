#include "pch.h"
#include "wall_fuel_adapter.h"
#include "wall_fuel.h"
#include "engine.h"
#include "engine_math.h"
#include "sensor.h"
#include "lambda_sensor.h"
#include "matrix_helpers.h" // Assuming matrix functions exist or will be added

// Define stoichiometric A/F ratio (should be configurable or accessible globally)
#define STOICH_AFR 14.7f

// Global instance (consider better placement/management)
static WallFuelAdapter wallFuelAdapterInstance;

WallFuelAdapter* getWallFuelAdapter() {
	return &wallFuelAdapterInstance;
}

WallFuelAdapter::WallFuelAdapter() : PeriodicTimerController(10) { // Run every 10ms (100Hz)
	// Initialize buffer or other members if needed
}

void WallFuelAdapter::init(WallFuelController *wallFuelController) {
	// Store pointer to the main wall fuel controller to access current alpha/beta
	// Note: This creates a dependency. Ensure proper initialization order.
	// Consider if accessing engineConfiguration directly is better for base values.
	// m_wallFuelController = wallFuelController; // Revisit this dependency

	// Initialize correction factors
	// TODO: Load these from persistent storage?
	// m_tauCorrection = engineConfiguration->wwAdaptTauCorrection;
	// m_xCorrection = engineConfiguration->wwAdaptXCorrection;
}

// Periodic function called by the scheduler
void WallFuelAdapter::PeriodicTask() {
	ScopePerf perf(PE::WallFuelAdaptation);

	// Check if adaptation is enabled
	// TODO: Use the actual config value: if (!engineConfiguration->wwAdaptEnable) return;
	if (!config->ww_adapt.wwAdaptEnable) {
		// If disabled, reset state?
		// m_isTransientActive = false;
		// m_bufferIndex = 0;
		return;
	}

	// Get current sensor values
	float currentTime = getTimeNowS();
	float currentLambda = Sensor::get(SensorType::Lambda).value_or(1.0f);
	float currentMap = Sensor::get(SensorType::Map).value_or(100.0f);
	float currentRpm = Sensor::getOrZero(SensorType::Rpm);
	float currentMfi = 0; // TODO: Get actual injected/commanded fuel mass for this cycle/previous cycle
	float currentMac = 0; // TODO: Get actual air mass for this cycle
	float currentClt = Sensor::get(SensorType::Clt).value_or(90.0f);

	// Run transient detection logic
	 detectTransient(currentTime, currentLambda, currentMap);

	// If a transient is active, buffer data
	 if (m_isTransientActive) {
		 bufferData(currentTime, currentLambda, currentMap, currentRpm, currentMfi, currentMac, currentClt);
	 }
}

void WallFuelAdapter::detectTransient(float currentTime, float currentLambda, float currentMap) {
	// TODO: Use actual config values
	float lambdaOnThreshold = config->ww_adapt.wwAdaptLambdaOnThreshold; // 0.05f;
	float lambdaOffThreshold = config->ww_adapt.wwAdaptLambdaOffThreshold; // 0.02f;

	// Calculate deviation from target lambda (assuming target is 1.0 for now)
	// TODO: Use actual target lambda from getTargetLambda()
	float lambdaError = absF(currentLambda - 1.0f);

	// Binary relay logic (hysteresis)
	 if (!m_isTransientActive) {
		 // If not active, check if we should start
		 if (lambdaError > lambdaOnThreshold) {
			 startDataBuffering(currentTime);
		 }
	 } else {
		 // If active, check if we should stop
		 if (lambdaError < lambdaOffThreshold) {
			 stopDataBufferingAndAdapt(currentTime);
		 }
	 }
}

void WallFuelAdapter::startDataBuffering(float currentTime) {
	 m_isTransientActive = true;
	 m_bufferIndex = 0;
	 m_transientStartTime = currentTime;
	 // Optional: Log transient start
}

void WallFuelAdapter::bufferData(float currentTime, float currentLambda, float currentMap, float currentRpm, float currentMfi, float currentMac, float currentClt) {
	 if (m_bufferIndex < WW_ADAPT_BUFFER_SIZE) {
		 WwAdaptSample& sample = m_dataBuffer[m_bufferIndex];
		 sample.timestamp = currentTime;
		 sample.lambda = currentLambda;
		 sample.map = currentMap;
		 sample.rpm = currentRpm;
		 sample.m_fi = currentMfi; // TODO: Ensure this is the correct fuel value
		 sample.m_ac = currentMac; // TODO: Ensure this is the correct air mass value
		 sample.clt = currentClt;

		 m_bufferIndex++;
	 } else {
		 // Buffer full, stop transient (or handle overflow?)
		 stopDataBufferingAndAdapt(currentTime);
		 // Optional: Log buffer overflow
	 }
}

void WallFuelAdapter::stopDataBufferingAndAdapt(float currentTime) {
	 m_isTransientActive = false;
	 int transientSampleCount = m_bufferIndex;
	 m_bufferIndex = 0; // Reset buffer index

	 // Optional: Log transient end

	 // Check if enough data was collected
	 if (transientSampleCount < 10) { // Minimum samples required? Configurable?
		 // Optional: Log insufficient data
		 return;
	 }

	 // Perform secondary check: Pman variation
	 if (!checkPmanVariation()) {
		 // Optional: Log Pman check failed
		 return;
	 }

	 // Estimate parameters using Least Squares
	 float tauEstimate, xEstimate;
	 if (estimateParameters(tauEstimate, xEstimate)) {
		 // Calculate average operating point during transient
		 float avgRpm = 0, avgMap = 0, avgClt = 0;
		 for (int i = 0; i < transientSampleCount; ++i) {
			 avgRpm += m_dataBuffer[i].rpm;
			 avgMap += m_dataBuffer[i].map;
			 avgClt += m_dataBuffer[i].clt;
		 }
		 avgRpm /= transientSampleCount;
		 avgMap /= transientSampleCount;
		 avgClt /= transientSampleCount;

		 // Update correction factors
		 updateCorrections(tauEstimate, xEstimate, avgRpm, avgMap, avgClt);
		 // Optional: Log successful adaptation
	 } else {
		 // Optional: Log estimation failure
	 }
}

bool WallFuelAdapter::checkPmanVariation() {
	 if (m_bufferIndex < 2) return false; // Need at least 2 points

	 float minPman = m_dataBuffer[0].map;
	 float maxPman = m_dataBuffer[0].map;

	 for (uint32_t i = 1; i < m_bufferIndex; ++i) {
		 minPman = minF(minPman, m_dataBuffer[i].map);
		 maxPman = maxF(maxPman, m_dataBuffer[i].map);
	 }

	 // TODO: Use actual config value
	 float pmanThreshold = config->ww_adapt.wwAdaptPmanThreshold; // 10.0f;
	 return (maxPman - minPman) > pmanThreshold;
}

float WallFuelAdapter::estimateDelayMs(float rpm) {
	// Simple lookup table interpolation
	// TODO: Use actual config tables
	// return interpolateMsg(&engineConfiguration->wwAdaptDelayRpmBins[0], &engineConfiguration->wwAdaptDelayValues[0], rpm);
	return interpolateMsg(config->ww_adapt.wwAdaptDelayRpmBins, config->ww_adapt.wwAdaptDelayValues, rpm);
}

// Placeholder for Least Squares implementation
bool WallFuelAdapter::estimateParameters(float& tauEstimate, float& xEstimate) {
	 ScopePerf perf(PE::WallFuelEstimate);
	 int n = m_bufferIndex; // Number of samples

	 // 1. Estimate Delay and Synchronize Data
	 // Create temporary synchronized buffers or work with offsets
	 // For each sample k, find the corresponding index j for lambda such that
	 // timestamp[k] approx equals lambda_timestamp[j] + delay[k]
	 // This part is complex and needs careful implementation.
	 // For now, we assume data is magically synchronized for the structure.

	 // 2. Calculate z and s vectors (based on PDF equations 10, 13)
	 // Need time delta (dt) between samples
	 // Need engine speed N (rpm) for each sample
	 // Need stoichiometric ratio S
	 float z[WW_ADAPT_BUFFER_SIZE];
	 float s[WW_ADAPT_BUFFER_SIZE];
	 float m_fi_sync[WW_ADAPT_BUFFER_SIZE]; // Synchronized m_fi
	 float m_ac_sync[WW_ADAPT_BUFFER_SIZE]; // Synchronized m_ac
	 float lambda_sync[WW_ADAPT_BUFFER_SIZE]; // Synchronized lambda
	 float rpm_sync[WW_ADAPT_BUFFER_SIZE]; // Synchronized rpm

	 // TODO: Populate synchronized buffers based on estimated delay
	 // This is a major simplification - actual synchronization is needed!
	 for (int k = 0; k < n; ++k) {
		 m_fi_sync[k] = m_dataBuffer[k].m_fi;
		 m_ac_sync[k] = m_dataBuffer[k].m_ac;
		 lambda_sync[k] = m_dataBuffer[k].lambda;
		 rpm_sync[k] = m_dataBuffer[k].rpm;
	 }

	 s[0] = 0;
	 for (int k = 0; k < n; ++k) {
		 // Eq 10: z = (m_ac * lambda / S) - m_fi
		 z[k] = (m_ac_sync[k] * lambda_sync[k] / STOICH_AFR) - m_fi_sync[k];

		 if (k > 0) {
			 float dt = m_dataBuffer[k].timestamp - m_dataBuffer[k-1].timestamp;
			 // Eq 13 (discrete approximation): s[k] = s[k-1] + z[k] * (6 * N[k] * dt) ??? Check PDF units/basis (angle vs time)
			 // PDF uses angle-based integration. Let's try a time-based approximation:
			 // Integral(z * dtheta/dt * dt) = Integral(z * omega * dt) = Integral(z * 6N * dt) where N is RPM?
			 // Let's assume s is the integral of (m_fi - m_ac*lambda/S) dt for simplicity first.
			 // s[k] = s[k-1] - z[k] * dt; // Integral of -z dt
			 s[k] = s[k-1] + (m_fi_sync[k] - m_ac_sync[k] * lambda_sync[k] / STOICH_AFR) * dt;
		 }
	 }

	 // 3. Formulate Regressor Matrix Phi (n x 2) and Measurement Vector S (n x 1)
	 // Phi_k = [ -(z[k] - z[0]), (m_fi[k] - m_fi[0]) ] (Eq 16b, 16c)
	 // S_k = s[k] (Eq 16a)
	 float PhiT_Phi[2][2] = {{0, 0}, {0, 0}};
	 float PhiT_S[2] = {0, 0};

	 float z0 = z[0];
	 float mfi0 = m_fi_sync[0];

	 for (int k = 1; k < n; ++k) { // Start from k=1 as per PDF?
		 float phi_k[2];
		 phi_k[0] = -(z[k] - z0);
		 phi_k[1] = m_fi_sync[k] - mfi0;

		 float s_k = s[k]; // Or s[k] - s[0]? PDF implies s(theta_0) = 0

		 // Calculate Phi^T * Phi incrementally
		 PhiT_Phi[0][0] += phi_k[0] * phi_k[0];
		 PhiT_Phi[0][1] += phi_k[0] * phi_k[1];
		 PhiT_Phi[1][0] += phi_k[1] * phi_k[0];
		 PhiT_Phi[1][1] += phi_k[1] * phi_k[1];

		 // Calculate Phi^T * S incrementally
		 PhiT_S[0] += phi_k[0] * s_k;
		 PhiT_S[1] += phi_k[1] * s_k;
	 }

	 // 4. Solve gamma = (Phi^T Phi)^(-1) * Phi^T S (Eq 21)
	 float det = PhiT_Phi[0][0] * PhiT_Phi[1][1] - PhiT_Phi[0][1] * PhiT_Phi[1][0];

	 // Check for singularity
	 if (absF(det) < 1e-9) { // Threshold for singularity
		 return false;
	 }

	 float invDet = 1.0f / det;
	 float PhiT_Phi_Inv[2][2];
	 PhiT_Phi_Inv[0][0] = invDet * PhiT_Phi[1][1];
	 PhiT_Phi_Inv[0][1] = -invDet * PhiT_Phi[0][1];
	 PhiT_Phi_Inv[1][0] = -invDet * PhiT_Phi[1][0];
	 PhiT_Phi_Inv[1][1] = invDet * PhiT_Phi[0][0];

	 float gamma[2];
	 gamma[0] = PhiT_Phi_Inv[0][0] * PhiT_S[0] + PhiT_Phi_Inv[0][1] * PhiT_S[1];
	 gamma[1] = PhiT_Phi_Inv[1][0] * PhiT_S[0] + PhiT_Phi_Inv[1][1] * PhiT_S[1];

	 // 5. Extract tau_f and X estimates (Eq 22)
	 float estimatedTau = gamma[0];
	 // Check for division by zero
	 if (absF(estimatedTau) < 1e-6) {
		 return false;
	 }
	 float estimatedX = gamma[1] / estimatedTau;

	 // Basic sanity checks
	 if (estimatedTau <= 0 || estimatedX < 0 || estimatedX > 1) {
		 return false;
	 }

	 tauEstimate = estimatedTau;
	 xEstimate = estimatedX;

	 return true;
}

void WallFuelAdapter::updateCorrections(float tauEstimate, float xEstimate, float avgRpm, float avgMap, float avgClt) {
	 ScopePerf perf(PE::WallFuelUpdate);

	 // Get current model parameters for the average operating point
	 // Need access to the functions that calculate alpha/beta based on RPM, MAP, CLT
	 // This requires either duplicating logic or accessing WallFuelController
	 // float currentTau = m_wallFuelController->computeTau(...); // Needs avg values
	 // float currentBeta = m_wallFuelController->computeBeta(...); // Needs avg values
	 // float currentAlpha = expf_taylor(-120 / (avgRpm * currentTau));
	 // float currentX = currentBeta;

	 // Placeholder: Assume we have currentTau and currentX for the avg op point
	 float currentTau = 1.0f; // TODO: Replace with actual calculation
	 float currentX = 0.5f; // TODO: Replace with actual calculation

	 // Calculate correction delta (how much the estimate differs from the current model)
	 float tauCorrectionDelta = 0;
	 if (currentTau > 1e-6) {
		 tauCorrectionDelta = (tauEstimate / currentTau) - 1.0f;
	 }
	 float xCorrectionDelta = 0;
	 if (currentX > 1e-6 && currentX < 1.0f) { // Avoid division by zero or extreme values
		 xCorrectionDelta = (xEstimate / currentX) - 1.0f;
	 }

	 // Apply low-pass filter (gain)
	 // TODO: Use actual config value
	 float gain = config->ww_adapt.wwAdaptGain; // 0.1f;

	 m_tauCorrection = (1.0f - gain) * m_tauCorrection + gain * tauCorrectionDelta;
	 m_xCorrection = (1.0f - gain) * m_xCorrection + gain * xCorrectionDelta;

	 // Clamp corrections to reasonable limits? e.g., +/- 50%?
	 m_tauCorrection = clampF(-0.5f, m_tauCorrection, 0.5f);
	 m_xCorrection = clampF(-0.5f, m_xCorrection, 0.5f);

	 // TODO: Store updated corrections persistently?
	 // engineConfiguration->wwAdaptTauCorrection = m_tauCorrection;
	 // engineConfiguration->wwAdaptXCorrection = m_xCorrection;
	 // requestSaveConfiguration(); // Or similar mechanism
}

float WallFuelAdapter::getTauCorrection() const {
	// Return the correction factor (applied as 1 + factor)
	// TODO: Use actual config value: if (!engineConfiguration->wwAdaptEnable) return 0;
	 if (!config->ww_adapt.wwAdaptEnable) return 0;
	 return m_tauCorrection;
}

float WallFuelAdapter::getXCorrection() const {
	// Return the correction factor (applied as 1 + factor)
	// TODO: Use actual config value: if (!engineConfiguration->wwAdaptEnable) return 0;
	 if (!config->ww_adapt.wwAdaptEnable) return 0;
	 return m_xCorrection;
}

// TODO:
// 1. Implement proper data synchronization based on delay.
// 2. Get actual m_fi and m_ac values.
// 3. Implement matrix helper functions if not available.
// 4. Integrate WallFuelAdapter instance creation and periodic call.
// 5. Modify WallFuelController::computeTau/computeBeta to apply corrections.
// 6. Add PE:: defines for new perf scopes.
// 7. Add persistent storage for correction factors.
// 8. Refine PDF equation interpretation (angle vs time base).
// 9. Add TunerStudio configuration and output channels.

