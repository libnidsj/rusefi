#include "pch.h"
#include "neural_fuel_controller.h"
#include "fuel_math.h"
#include "engine_math.h"
#include "sensor.h"
#include "efitime.h"

#if EFI_ENGINE_CONTROL

// =============================================================================
// CONSTRUCTOR AND INITIALIZATION
// =============================================================================

NeuralFuelController::NeuralFuelController() {
    initializeConfiguration();
    initializeNeuralWeights();
    resetAllTimers();
    resetNeuralState();
}

void NeuralFuelController::initializeConfiguration() {
    // Initialize with safe defaults
    m_config.enabled = false;
    m_config.learningRate = 0.001f;
    m_config.confidenceThreshold = 0.7f;
    m_config.updateIntervalMs = 20; // 50Hz
    m_config.weightScale = WEIGHT_SCALE;
    m_config.activationScale = ACTIVATION_SCALE;
    m_config.maxBetaCorrection = 0.5f;
    m_config.maxTauCorrection = 0.5f;
    m_config.minConfidenceThreshold = 0.3f;
}

void NeuralFuelController::initializeNeuralWeights() {
    m_weights.initializeDefault();
}

void NeuralFuelController::resetAllTimers() {
    m_updateTimer.reset();
    m_dataCollectionTimer.reset();
    m_ignitionOnTimer.reset();
    m_ignitionOffTimer.reset();
    m_recoveryTimer.reset();
    m_stateResetTimer.reset();
}

void NeuralFuelController::resetNeuralState() {
    m_lstmState.reset();
    m_temporalBuffer.reset();
    
    // Clear working buffers
    for (size_t i = 0; i < NEURAL_INPUT_COUNT; i++) {
        m_currentInputs[i] = 0.0f;
        m_quantizedInputs[i] = 0;
    }
    
    for (size_t i = 0; i < NEURAL_OUTPUT_COUNT; i++) {
        m_quantizedOutputs[i] = 0;
        m_currentOutputs[i] = 0.0f;
    }
    
    // Reset state tracking
    m_consecutiveFailures = 0;
    m_lastValidConfidence = 0.0f;
    m_emergencyMode = false;
    
    // Reset telemetry
    m_telemetry = neural_fuel_telemetry_s{};
    m_telemetry.isEnabled = m_config.enabled;
}

// =============================================================================
// ENGINE MODULE CALLBACKS
// =============================================================================

void NeuralFuelController::onFastCallback() {
    if (!m_config.enabled) {
        return;
    }
    
    // Collect temporal data at 200Hz
    if (m_dataCollectionTimer.hasElapsedSec(DATA_COLLECTION_INTERVAL)) {
        collectTemporalData();
        m_dataCollectionTimer.reset();
    }
    
    // Update AWW integration data if available
    if (m_adaptiveWW) {
        collectAWWData();
    }
}

void NeuralFuelController::onSlowCallback() {
    if (!m_config.enabled) {
        return;
    }
    
    // Process neural network at 50Hz
    if (m_updateTimer.hasElapsedSec(NEURAL_UPDATE_INTERVAL)) {
        // Check operational conditions
        if (checkOperationalConditions()) {
            // Update temporal features
            updateTemporalFeatures();
            
            // Process neural inference
            processNeuralInference();
            
            // Apply predictions to integrated systems
            if (m_telemetry.currentConfidence > m_config.confidenceThreshold) {
                applyPredictionsToAWW();
                applyPredictionsToLTFT();
            }
            
            m_telemetry.isActive = true;
        } else {
            m_telemetry.isActive = false;
            
            // Handle failure if needed
            if (m_telemetry.fallbackMode) {
                handleNeuralFailure();
            }
        }
        
        // Update telemetry
        updateTelemetry();
        m_updateTimer.reset();
    }
    
    // Periodic state reset for stability (every 5 minutes)
    if (m_stateResetTimer.hasElapsedSec(STATE_RESET_INTERVAL)) {
        performStateReset();
        m_stateResetTimer.reset();
    }
    
    // Recovery attempts
    if (m_emergencyMode && m_recoveryTimer.hasElapsedSec(RECOVERY_ATTEMPT_INTERVAL)) {
        attemptRecovery();
        m_recoveryTimer.reset();
    }
}

void NeuralFuelController::onIgnitionStateChanged(bool ignitionOn) {
    m_ignitionState = ignitionOn;
    
    if (ignitionOn) {
        // Reset on ignition on
        m_ignitionOnTimer.reset();
        resetNeuralState();
        m_emergencyMode = false;
    } else {
        // Track ignition off time
        m_ignitionOffTimer.reset();
    }
}

// =============================================================================
// DATA COLLECTION AND PREPROCESSING
// =============================================================================

void NeuralFuelController::collectTemporalData() {
    // Get lambda error using correct target lambda (critical fix from memories)
    auto lambda = Sensor::get(SensorType::Lambda1);
    if (!lambda.Valid) {
        return;
    }
    
    // CRITICAL: Use engine->fuelComputer.targetLambda, NOT Sensor::get(SensorType::LambdaTarget)
    float targetLambda = engine->fuelComputer.targetLambda;
    float lambdaError = lambda.Value - targetLambda;
    
    // Add to temporal buffer with timestamp
    float currentTime = getTimeNowNt() * 1e-6f; // Convert to seconds
    m_temporalBuffer.addSample(lambdaError, currentTime);
}

void NeuralFuelController::updateTemporalFeatures() {
    if (!m_temporalBuffer.isReady()) {
        return;
    }
    
    // Compute temporal slopes
    computeTemporalSlopes();
    
    // Compute statistical features
    computeStatisticalFeatures();
    
    // Update contextual inputs
    updateContextualInputs();
}

void NeuralFuelController::computeTemporalSlopes() {
    const float samplingRate = 50.0f; // 50Hz buffer
    
    // Short-term slope (200ms = 10 samples)
    constexpr size_t shortTermSamples = 10;
    float shortTermY[shortTermSamples];
    float shortTermX[shortTermSamples];
    
    // Extract recent samples for short-term analysis
    for (size_t i = 0; i < shortTermSamples; i++) {
        int index = (int)m_temporalBuffer.currentIndex - (int)i - 1;
        if (index < 0) index += TEMPORAL_BUFFER_SIZE;
        
        shortTermY[i] = m_temporalBuffer.lambdaErrors[index];
        shortTermX[i] = m_temporalBuffer.timestamps[index];
    }
    
    m_temporalBuffer.shortSlope = computeLinearRegressionSlope(shortTermY, shortTermX, shortTermSamples);
    
    // Long-term slope (1000ms = 50 samples = full buffer)
    m_temporalBuffer.longSlope = computeLinearRegressionSlope(
        m_temporalBuffer.lambdaErrors, 
        m_temporalBuffer.timestamps, 
        TEMPORAL_BUFFER_SIZE
    );
}

void NeuralFuelController::computeStatisticalFeatures() {
    // Compute variance of lambda errors
    m_temporalBuffer.lambdaVariance = computeVariance(
        m_temporalBuffer.lambdaErrors, 
        TEMPORAL_BUFFER_SIZE
    );
    
    // Compute moving average trend
    m_temporalBuffer.lambdaTrend = computeMovingAverageTrend(
        m_temporalBuffer.lambdaErrors, 
        TEMPORAL_BUFFER_SIZE, 
        25  // Half buffer window
    );
}

void NeuralFuelController::updateContextualInputs() {
    // Get current operating conditions
    float rpm = Sensor::getOrZero(SensorType::Rpm);
    float load = getFuelingLoad(); // Use existing rusefi function
    
    // Normalize for neural network input
    float rpmNormalized = clampF(0.0f, rpm / RPM_NORMALIZATION_MAX, 1.0f);
    float loadNormalized = clampF(0.0f, load / LOAD_NORMALIZATION_MAX, 1.0f);
    
    // Store in telemetry for diagnostics
    m_telemetry.rpmNormalized = rpmNormalized;
    m_telemetry.loadNormalized = loadNormalized;
    
    // Get transient magnitude from AWW if available
    if (m_adaptiveWW && m_adaptiveWW->isActive()) {
        m_temporalBuffer.awwTransientMagnitude = 0.5f; // Placeholder - would get from AWW
    } else {
        m_temporalBuffer.awwTransientMagnitude = 0.0f;
    }
}

void NeuralFuelController::collectAWWData() {
    if (!m_adaptiveWW) return;
    
    // Update AWW state in buffer
    m_temporalBuffer.awwState = (uint8_t)m_adaptiveWW->getState();
    
    // Get transient information
    if (m_adaptiveWW->isActive()) {
        m_temporalBuffer.awwTransientMagnitude = 0.5f; // Would get actual magnitude
    }
}

void NeuralFuelController::collectLTFTData() {
    // Currently, LTFT integration is through the existing getLtft() interface
    // Additional integration can be added here if needed
}

// =============================================================================
// NEURAL NETWORK PROCESSING
// =============================================================================

void NeuralFuelController::prepareNeuralInputs() {
    if (!m_temporalBuffer.isReady()) return;
    
    const float samplingRate = 50.0f;
    
    // Extract temporal lambda errors (T1-T5)
    m_currentInputs[0] = getLambdaErrorAtTimeOffset(m_temporalBuffer, 0.0f, samplingRate);    // T1 (current)
    m_currentInputs[1] = getLambdaErrorAtTimeOffset(m_temporalBuffer, 100.0f, samplingRate);  // T2 (-100ms)
    m_currentInputs[2] = getLambdaErrorAtTimeOffset(m_temporalBuffer, 200.0f, samplingRate);  // T3 (-200ms)
    m_currentInputs[3] = getLambdaErrorAtTimeOffset(m_temporalBuffer, 500.0f, samplingRate);  // T4 (-500ms)
    m_currentInputs[4] = getLambdaErrorAtTimeOffset(m_temporalBuffer, 1000.0f, samplingRate); // T5 (-1000ms)
    
    // Slope analysis inputs
    m_currentInputs[5] = m_temporalBuffer.shortSlope;
    m_currentInputs[6] = m_temporalBuffer.longSlope;
    
    // Statistical features
    m_currentInputs[7] = m_temporalBuffer.lambdaVariance;
    m_currentInputs[8] = m_temporalBuffer.lambdaTrend;
    
    // Contextual inputs
    m_currentInputs[9] = m_telemetry.loadNormalized;
    m_currentInputs[10] = m_telemetry.rpmNormalized;
    m_currentInputs[11] = m_temporalBuffer.awwTransientMagnitude;
    
    // Normalize all inputs to [-1, 1] range for neural network
    for (size_t i = 0; i < 9; i++) { // Lambda-related inputs
        m_currentInputs[i] = clampF(-1.0f, m_currentInputs[i] / LAMBDA_ERROR_NORMALIZATION_MAX, 1.0f);
    }
    
    // Slope inputs already normalized
    m_currentInputs[5] = clampF(-1.0f, m_currentInputs[5] / SLOPE_NORMALIZATION_MAX, 1.0f);
    m_currentInputs[6] = clampF(-1.0f, m_currentInputs[6] / SLOPE_NORMALIZATION_MAX, 1.0f);
    
    // Copy to telemetry for diagnostics
    m_telemetry.lambdaErrorT1 = m_currentInputs[0];
    m_telemetry.lambdaErrorT2 = m_currentInputs[1];
    m_telemetry.lambdaErrorT3 = m_currentInputs[2];
    m_telemetry.lambdaErrorT4 = m_currentInputs[3];
    m_telemetry.lambdaErrorT5 = m_currentInputs[4];
    m_telemetry.lambdaSlopeShort = m_currentInputs[5];
    m_telemetry.lambdaSlopeLong = m_currentInputs[6];
    m_telemetry.lambdaVariance = m_currentInputs[7];
    m_telemetry.lambdaTrend = m_currentInputs[8];
    m_telemetry.transientMagnitude = m_currentInputs[11];
}

void NeuralFuelController::processNeuralInference() {
    // Prepare inputs
    prepareNeuralInputs();
    
    // Validate inputs
    if (!validateInputs()) {
        handleNeuralFailure();
        return;
    }
    
    // Quantize inputs
    quantizeInputs(m_currentInputs, m_quantizedInputs);
    
    // Run LSTM forward pass
    runLSTMForward();
    
    // Process output layer
    processOutputLayer(m_weights, m_lstmState, m_quantizedOutputs);
    
    // Dequantize outputs
    dequantizeOutputs(m_quantizedOutputs, m_currentOutputs);
    
    // Post-process and validate outputs
    postProcessOutputs();
    
    // Update telemetry
    updateOutputTelemetry();
    
    // Track performance
    trackPerformanceMetrics();
}

void NeuralFuelController::runLSTMForward() {
    // Use the LSTM implementation from neural_fuel_network.cpp
    processLSTMCell(m_weights, m_quantizedInputs, m_lstmState);
}

void NeuralFuelController::postProcessOutputs() {
    // Validate outputs
    if (!validateNeuralOutputs(m_currentOutputs)) {
        sanitizeNeuralOutputs(m_currentOutputs);
        updateFailureTracking();
    }
    
    // Calculate confidence
    float confidence = calculateNeuralConfidence(m_currentOutputs, m_telemetry);
    m_currentOutputs[2] = confidence; // Override network confidence with calculated
    
    // Update predictions in telemetry
    m_telemetry.betaPrediction = m_currentOutputs[0];
    m_telemetry.tauPrediction = m_currentOutputs[1];
    m_telemetry.currentConfidence = m_currentOutputs[2];
    m_telemetry.adaptiveLearningRate = m_currentOutputs[3];
    
    // Track last valid confidence
    if (confidence > MIN_OPERATIONAL_CONFIDENCE) {
        m_lastValidConfidence = confidence;
        m_consecutiveFailures = 0;
    }
}

// =============================================================================
// QUANTIZATION FUNCTIONS
// =============================================================================

void NeuralFuelController::quantizeInputs(const float* inputs, int16_t* quantized) {
    quantizeFloatArray(inputs, quantized, NEURAL_INPUT_COUNT, m_config.activationScale);
}

void NeuralFuelController::dequantizeOutputs(const int16_t* quantized, float* outputs) {
    dequantizeIntArray(quantized, outputs, NEURAL_OUTPUT_COUNT, m_config.activationScale);
}

int16_t NeuralFuelController::quantizeFloat(float value, int16_t scale) {
    return ::quantizeFloat(value, scale);
}

float NeuralFuelController::dequantizeInt(int16_t value, int16_t scale) {
    return ::dequantizeInt(value, scale);
}

// =============================================================================
// INTEGRATION WITH EXISTING SYSTEMS
// =============================================================================

void NeuralFuelController::integrateWithAdaptiveWW(AdaptiveWallWettingController* aww) {
    m_adaptiveWW = aww;
    m_telemetry.awwIntegrated = (aww != nullptr);
}

void NeuralFuelController::integrateWithLTFT(LongTermFuelTrim* ltft) {
    m_ltft = ltft;
    m_telemetry.ltftIntegrated = (ltft != nullptr);
}

void NeuralFuelController::applyPredictionsToAWW() {
    if (!m_adaptiveWW || m_telemetry.currentConfidence < m_config.confidenceThreshold) {
        return;
    }
    
    // This would require adding neural assist interface to AWW
    // For now, just track the predictions
    m_telemetry.neuralWeightAWW = m_telemetry.currentConfidence * MAX_NEURAL_INFLUENCE;
}

void NeuralFuelController::applyPredictionsToLTFT() {
    if (!m_ltft || m_telemetry.currentConfidence < m_config.confidenceThreshold) {
        return;
    }
    
    // This would require adding neural assist interface to LTFT
    // For now, just track the predictions
    m_telemetry.neuralWeightLTFT = m_telemetry.currentConfidence * MAX_NEURAL_INFLUENCE;
}

// =============================================================================
// VALIDATION AND SAFETY
// =============================================================================

bool NeuralFuelController::checkOperationalConditions() {
    // Check basic sensor validity
    if (!validateSensorData()) {
        m_telemetry.fallbackMode = true;
        return false;
    }
    
    // Check if buffer is ready
    if (!m_temporalBuffer.isReady()) {
        return false;
    }
    
    // Check system health
    if (!checkSystemHealth()) {
        m_telemetry.fallbackMode = true;
        return false;
    }
    
    // Check if in emergency mode
    if (m_emergencyMode) {
        return false;
    }
    
    // Check consecutive failures
    if (m_consecutiveFailures > MAX_CONSECUTIVE_FAILURES) {
        enterEmergencyMode();
        return false;
    }
    
    m_telemetry.fallbackMode = false;
    return true;
}

bool NeuralFuelController::validateSensorData() {
    // Check lambda sensor
    auto lambda = Sensor::get(SensorType::Lambda1);
    if (!lambda.Valid || lambda.Value < 0.5f || lambda.Value > 2.0f) {
        return false;
    }
    
    // Check other critical sensors
    auto rpm = Sensor::get(SensorType::Rpm);
    if (!rpm.Valid || rpm.Value < 300 || rpm.Value > 8000) {
        return false;
    }
    
    // Check that we have valid target lambda
    if (engine->fuelComputer.targetLambda < 0.5f || engine->fuelComputer.targetLambda > 2.0f) {
        return false;
    }
    
    return true;
}

bool NeuralFuelController::validateInputs() {
    // Check for NaN or infinite values in inputs
    for (size_t i = 0; i < NEURAL_INPUT_COUNT; i++) {
        if (!isfinite(m_currentInputs[i])) {
            return false;
        }
    }
    
    // Check input ranges
    for (size_t i = 0; i < NEURAL_INPUT_COUNT; i++) {
        if (fabsf(m_currentInputs[i]) > 10.0f) { // Sanity check
            return false;
        }
    }
    
    return true;
}

bool NeuralFuelController::checkSystemHealth() {
    // Check if engine is running normally
    if (!engine->rpmCalculator.isRunning()) {
        return false;
    }
    
    // Check temperature ranges
    auto clt = Sensor::get(SensorType::Clt);
    if (clt.Valid && (clt.Value < 60 || clt.Value > 110)) {
        return false;
    }
    
    // Check if any critical systems are in error state
    if (m_adaptiveWW && !m_adaptiveWW->isActive()) {
        // AWW integration available but not active - might be OK
    }
    
    return true;
}

// =============================================================================
// FAILSAFE AND RECOVERY
// =============================================================================

void NeuralFuelController::handleNeuralFailure() {
    m_consecutiveFailures++;
    
    if (m_consecutiveFailures > MAX_CONSECUTIVE_FAILURES) {
        enterEmergencyMode();
    }
    
    // Reset neural state to try to recover
    if (m_consecutiveFailures % 3 == 0) {
        resetNeuralState();
    }
}

void NeuralFuelController::enterEmergencyMode() {
    m_emergencyMode = true;
    m_telemetry.emergencyMode = true;
    m_telemetry.fallbackMode = true;
    m_telemetry.isActive = false;
    
    // Reset recovery timer
    m_recoveryTimer.reset();
}

void NeuralFuelController::attemptRecovery() {
    if (!shouldAttemptRecovery()) {
        return;
    }
    
    // Reset everything and try again
    resetNeuralState();
    m_consecutiveFailures = 0;
    m_emergencyMode = false;
    m_telemetry.emergencyMode = false;
}

bool NeuralFuelController::shouldAttemptRecovery() {
    // Only attempt recovery if basic conditions are met
    return validateSensorData() && checkSystemHealth();
}

void NeuralFuelController::performStateReset() {
    // Periodic state reset for long-term stability
    // Only reset if not currently active or in emergency
    if (!m_telemetry.isActive && !m_emergencyMode) {
        m_lstmState.reset();
    }
}

// =============================================================================
// TELEMETRY AND DIAGNOSTICS
// =============================================================================

void NeuralFuelController::updateTelemetry() {
    updateInputTelemetry();
    updateOutputTelemetry();
    updateStatusTelemetry();
}

void NeuralFuelController::updateInputTelemetry() {
    // Input telemetry is updated in prepareNeuralInputs()
    m_telemetry.bufferReady = m_temporalBuffer.isReady();
}

void NeuralFuelController::updateOutputTelemetry() {
    // Output telemetry is updated in postProcessOutputs()
    
    // Update hidden states for diagnostics (first 4 states)
    if (m_lstmState.initialized) {
        m_telemetry.hiddenState0 = dequantizeInt(m_lstmState.hiddenState[0], m_config.activationScale);
        m_telemetry.hiddenState1 = dequantizeInt(m_lstmState.hiddenState[1], m_config.activationScale);
        m_telemetry.hiddenState2 = dequantizeInt(m_lstmState.hiddenState[2], m_config.activationScale);
        m_telemetry.hiddenState3 = dequantizeInt(m_lstmState.hiddenState[3], m_config.activationScale);
    }
}

void NeuralFuelController::updateStatusTelemetry() {
    m_telemetry.isEnabled = m_config.enabled;
    m_telemetry.consecutiveFailures = m_consecutiveFailures;
    
    // Update timing information
    m_telemetry.timeSinceIgnitionOn = m_ignitionOnTimer.getElapsedSeconds();
    m_telemetry.timeSinceLastUpdate = m_updateTimer.getElapsedSeconds();
}

void NeuralFuelController::trackPerformanceMetrics() {
    // Increment inference count
    m_telemetry.totalInferences++;
    
    // Determine if this was a successful prediction
    bool successful = (m_telemetry.currentConfidence > m_config.confidenceThreshold) && 
                     !m_telemetry.fallbackMode;
    
    if (successful) {
        m_telemetry.successfulPredictions++;
    }
    
    // Update success rate
    m_telemetry.updateSuccessRate();
}

void NeuralFuelController::updateFailureTracking() {
    m_consecutiveFailures++;
    // Failure tracking is handled in handleNeuralFailure()
}

#endif // EFI_ENGINE_CONTROL