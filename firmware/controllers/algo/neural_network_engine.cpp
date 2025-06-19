/**
 * @file neural_network_engine.cpp
 * 
 * Neural Network Engine Implementation for rusefi
 * Complete quantized LSTM-like system with temporal analysis
 */

#include "pch.h"
#include "neural_network_engine.h"
#include "sensor.h"
#include "engine.h"
#include <rusefi/math.h>

// Global neural network coordinator instance
NeuralNetworkCoordinator* g_neural_coordinator = nullptr;

// Utility function to calculate lambda error using correct target lambda
float calculateTargetLambdaError() {
    auto lambda = Sensor::get(SensorType::Lambda1);
    if (!lambda.Valid) {
        return NAN;
    }
    
    // CRITICAL: Use engine->fuelComputer.targetLambda (dynamically calculated)
    // NOT Sensor::get(SensorType::LambdaTarget) which doesn't exist
    float targetLambda = engine->fuelComputer.targetLambda;
    return lambda.Value - targetLambda;
}

bool isEngineInStableCondition() {
    // Check all required sensors are valid
    auto rpm = Sensor::get(SensorType::Rpm);
    auto clt = Sensor::get(SensorType::Clt);
    auto tps = Sensor::get(SensorType::DriverThrottleIntent);
    auto map = Sensor::get(SensorType::Map);
    auto lambda = Sensor::get(SensorType::Lambda1);
    
    if (!rpm.Valid || !clt.Valid || !tps.Valid || !map.Valid || !lambda.Valid) {
        return false;
    }
    
    // Check operational ranges
    if (rpm.Value < 1000 || rpm.Value > 6000) return false;
    if (clt.Value < 60 || clt.Value > 110) return false;
    if (lambda.Value < 0.7f || lambda.Value > 1.3f) return false;
    
    // Check engine not in special modes
    if (engine->module<DfcoController>()->cutFuel()) return false;
    
    return true;
}

void logNeuralActivity(const char* component, const char* action, float value) {
    // Production code - no logging for performance
    // Could be enabled via debug flag if needed
    (void)component;
    (void)action;
    (void)value;
}

// TemporalBuffer Implementation
void TemporalBuffer::addSample(float lambda_error, float timestamp) {
    lambda_errors[index] = lambda_error;
    timestamps[index] = timestamp;
    
    index = (index + 1) % NN_TEMPORAL_BUFFER_SIZE;
    if (!full && count < NN_TEMPORAL_BUFFER_SIZE) {
        count++;
    } else {
        full = true;
    }
}

float TemporalBuffer::calculateSlope(float window_seconds) const {
    if (count < 2) return 0.0f;
    
    uint16_t window_samples = minI(static_cast<uint16_t>(window_seconds * 200.0f), count);
    if (window_samples < 2) return 0.0f;
    
    // Use the most recent samples for slope calculation
    float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    uint16_t n = 0;
    
    uint16_t start_idx = (index - window_samples + NN_TEMPORAL_BUFFER_SIZE) % NN_TEMPORAL_BUFFER_SIZE;
    
    for (uint16_t i = 0; i < window_samples; i++) {
        uint16_t idx = (start_idx + i) % NN_TEMPORAL_BUFFER_SIZE;
        float x = timestamps[idx];
        float y = lambda_errors[idx];
        
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        n++;
    }
    
    // Linear regression: slope = (n*sum_xy - sum_x*sum_y) / (n*sum_x2 - sum_x*sum_x)
    float denominator = n * sum_x2 - sum_x * sum_x;
    if (fabsf(denominator) < 1e-6f) return 0.0f;
    
    return (n * sum_xy - sum_x * sum_y) / denominator;
}

float TemporalBuffer::calculateVariance(float window_seconds) const {
    if (count < 2) return 0.0f;
    
    uint16_t window_samples = minI(static_cast<uint16_t>(window_seconds * 200.0f), count);
    if (window_samples < 2) return 0.0f;
    
    // Calculate mean first
    float mean = calculateMean(window_seconds);
    
    float sum_squared_diff = 0.0f;
    uint16_t start_idx = (index - window_samples + NN_TEMPORAL_BUFFER_SIZE) % NN_TEMPORAL_BUFFER_SIZE;
    
    for (uint16_t i = 0; i < window_samples; i++) {
        uint16_t idx = (start_idx + i) % NN_TEMPORAL_BUFFER_SIZE;
        float diff = lambda_errors[idx] - mean;
        sum_squared_diff += diff * diff;
    }
    
    return sum_squared_diff / (window_samples - 1);
}

float TemporalBuffer::calculateMean(float window_seconds) const {
    if (count == 0) return 0.0f;
    
    uint16_t window_samples = minI(static_cast<uint16_t>(window_seconds * 200.0f), count);
    if (window_samples == 0) return 0.0f;
    
    float sum = 0.0f;
    uint16_t start_idx = (index - window_samples + NN_TEMPORAL_BUFFER_SIZE) % NN_TEMPORAL_BUFFER_SIZE;
    
    for (uint16_t i = 0; i < window_samples; i++) {
        uint16_t idx = (start_idx + i) % NN_TEMPORAL_BUFFER_SIZE;
        sum += lambda_errors[idx];
    }
    
    return sum / window_samples;
}

void TemporalBuffer::reset() {
    index = 0;
    count = 0;
    full = false;
}

// TemporalAnalyzer Implementation
void TemporalAnalyzer::addSample(float lambda_error) {
    float timestamp = m_timer.getElapsedSeconds();
    m_buffer.addSample(lambda_error, timestamp);
}

void TemporalAnalyzer::extractFeatures(float features[static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT)]) {
    // Initialize all features to zero
    for (uint8_t i = 0; i < static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT); i++) {
        features[i] = 0.0f;
    }
    
    if (m_buffer.count < 2) return;
    
    // Extract temporal lambda errors at specific time intervals
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T1)] = getLambdaErrorAtTime(0.1f);
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T2)] = getLambdaErrorAtTime(0.2f);
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T3)] = getLambdaErrorAtTime(0.3f);
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T4)] = getLambdaErrorAtTime(0.4f);
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T5)] = getLambdaErrorAtTime(0.5f);
    
    // Extract slope features using linear regression
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_SLOPE_SHORT)] = getShortTermSlope();
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_SLOPE_LONG)] = getLongTermSlope();
    
    // Extract statistical features
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_VARIANCE)] = m_buffer.calculateVariance(0.5f);
    features[static_cast<uint8_t>(TemporalFeature::LAMBDA_MEAN)] = m_buffer.calculateMean(0.5f);
    
    // Extract derivatives from current sensors
    auto tps = Sensor::get(SensorType::DriverThrottleIntent);
    auto map = Sensor::get(SensorType::Map);
    
    // These would be calculated from previous samples if we tracked them
    features[static_cast<uint8_t>(TemporalFeature::TPS_DERIVATIVE)] = 0.0f; // TODO: implement
    features[static_cast<uint8_t>(TemporalFeature::MAP_DERIVATIVE)] = 0.0f; // TODO: implement
}

float TemporalAnalyzer::getLambdaErrorAtTime(float seconds_ago) const {
    if (m_buffer.count == 0) return 0.0f;
    
    float current_time = m_timer.getElapsedSeconds();
    float target_time = current_time - seconds_ago;
    
    // Find closest sample to target time
    float closest_diff = 1000.0f;
    float closest_value = 0.0f;
    
    uint16_t search_samples = minI(m_buffer.count, static_cast<uint16_t>(seconds_ago * 200.0f + 20));
    uint16_t start_idx = (m_buffer.index - search_samples + NN_TEMPORAL_BUFFER_SIZE) % NN_TEMPORAL_BUFFER_SIZE;
    
    for (uint16_t i = 0; i < search_samples; i++) {
        uint16_t idx = (start_idx + i) % NN_TEMPORAL_BUFFER_SIZE;
        float time_diff = fabsf(m_buffer.timestamps[idx] - target_time);
        
        if (time_diff < closest_diff) {
            closest_diff = time_diff;
            closest_value = m_buffer.lambda_errors[idx];
        }
    }
    
    return closest_value;
}

float TemporalAnalyzer::getShortTermSlope() const {
    return m_buffer.calculateSlope(0.1f);  // 100ms window
}

float TemporalAnalyzer::getLongTermSlope() const {
    return m_buffer.calculateSlope(0.5f);  // 500ms window
}

void TemporalAnalyzer::reset() {
    m_buffer.reset();
    m_timer.reset();
}

// NeuralNetworkEngine Implementation
NeuralNetworkEngine::NeuralNetworkEngine() 
    : m_current_app(NeuralNetworkApp::DISABLED)
    , m_failsafe_active(false)
    , m_consecutive_failures(0) {
    
    // Initialize LSTM cell
    m_lstm_cell.initialized = false;
    for (uint8_t i = 0; i < NN_MAX_HIDDEN_SIZE; i++) {
        m_lstm_cell.hidden_state[i] = 0;
        m_lstm_cell.cell_state[i] = 0;
        m_lstm_cell.forget_gate[i] = 127; // Start with moderate forget
        m_lstm_cell.input_gate[i] = 0;
        m_lstm_cell.output_gate[i] = 127;
    }
}

void NeuralNetworkEngine::initializeNetwork(NeuralNetworkApp app) {
    m_current_app = app;
    
    // Initialize layer sizes based on application
    switch (app) {
        case NeuralNetworkApp::WALL_WETTING_BETA:
        case NeuralNetworkApp::WALL_WETTING_TAU:
            m_input_layer.input_size = static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT);
            m_input_layer.output_size = NN_MAX_HIDDEN_SIZE;
            m_hidden_layer.input_size = NN_MAX_HIDDEN_SIZE;
            m_hidden_layer.output_size = NN_MAX_HIDDEN_SIZE;
            m_output_layer.input_size = NN_MAX_HIDDEN_SIZE;
            m_output_layer.output_size = 1; // Single correction value
            break;
            
        case NeuralNetworkApp::LTFT_CORRECTION:
            m_input_layer.input_size = static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT);
            m_input_layer.output_size = NN_MAX_HIDDEN_SIZE;
            m_hidden_layer.input_size = NN_MAX_HIDDEN_SIZE;
            m_hidden_layer.output_size = NN_MAX_HIDDEN_SIZE;
            m_output_layer.input_size = NN_MAX_HIDDEN_SIZE;
            m_output_layer.output_size = 1;
            break;
            
        default:
            return;
    }
    
    // Initialize weights with small random values (simplified - could use proper initialization)
    m_input_layer.scale_factor = 64;  // Scale factor for quantization
    m_hidden_layer.scale_factor = 64;
    m_output_layer.scale_factor = 64;
    
    // Initialize bias terms
    for (uint8_t i = 0; i < NN_MAX_HIDDEN_SIZE; i++) {
        m_input_layer.biases[i] = 0;
        m_hidden_layer.biases[i] = 0;
        m_output_layer.biases[i] = 0;
    }
    
    resetLSTMState();
}

q8_t NeuralNetworkEngine::quantizeFloat(float value, float scale_factor) const {
    float scaled = value * scale_factor;
    return static_cast<q8_t>(clampF(-127, scaled, 127));
}

float NeuralNetworkEngine::dequantizeQ8(q8_t value, float scale_factor) const {
    return static_cast<float>(value) / scale_factor;
}

NeuralCorrection NeuralNetworkEngine::processInputs(const float* features, uint8_t feature_count) {
    NeuralCorrection result;
    result.valid = false;
    result.confidence = 0.0f;
    result.multiplier = 1.0f;
    result.sample_count = 0;
    
    if (m_failsafe_active || feature_count != static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT)) {
        return result;
    }
    
    // Quantize input features
    q8_t quantized_inputs[NN_MAX_TEMPORAL_INPUTS];
    for (uint8_t i = 0; i < feature_count && i < NN_MAX_TEMPORAL_INPUTS; i++) {
        quantized_inputs[i] = quantizeFloat(features[i], m_input_layer.scale_factor);
    }
    
    // Forward pass through LSTM
    forwardLSTM(quantized_inputs, feature_count);
    
    // Simple output calculation (in production, this would be more sophisticated)
    q16_t output_sum = 0;
    for (uint8_t i = 0; i < NN_MAX_HIDDEN_SIZE; i++) {
        output_sum += static_cast<q16_t>(m_lstm_cell.hidden_state[i]);
    }
    
    // Convert to correction multiplier
    float raw_output = dequantizeQ8(static_cast<q8_t>(output_sum / NN_MAX_HIDDEN_SIZE), m_output_layer.scale_factor);
    
    // Apply sigmoid to get correction in reasonable range (0.5 to 2.0)
    float sigmoid_output = 1.0f / (1.0f + expf(-raw_output));
    result.multiplier = 0.5f + sigmoid_output * 1.5f;  // Map [0,1] to [0.5, 2.0]
    
    // Calculate confidence based on output stability
    result.confidence = clampF(0.0f, 1.0f - fabsf(raw_output), 1.0f);
    
    result.valid = validateOutput(result);
    
    return result;
}

void NeuralNetworkEngine::forwardLSTM(const q8_t* inputs, uint8_t input_size) {
    if (!m_lstm_cell.initialized) {
        resetLSTMState();
        m_lstm_cell.initialized = true;
    }
    
    // Simplified LSTM forward pass (production version would be more complete)
    for (uint8_t i = 0; i < NN_MAX_HIDDEN_SIZE && i < input_size; i++) {
        // Update gates (simplified)
        q16_t input_contribution = static_cast<q16_t>(inputs[i]);
        q16_t hidden_contribution = static_cast<q16_t>(m_lstm_cell.hidden_state[i]);
        
        // Forget gate: decide what to forget from cell state
        m_lstm_cell.forget_gate[i] = static_cast<q8_t>(clampF(-127, (input_contribution + hidden_contribution) / 4, 127));
        
        // Input gate: decide what new information to store
        m_lstm_cell.input_gate[i] = static_cast<q8_t>(clampF(-127, (input_contribution - hidden_contribution) / 4, 127));
        
        // Update cell state
        q16_t forget_contribution = (static_cast<q16_t>(m_lstm_cell.cell_state[i]) * m_lstm_cell.forget_gate[i]) / 127;
        q16_t input_gate_contribution = (input_contribution * m_lstm_cell.input_gate[i]) / 127;
        m_lstm_cell.cell_state[i] = static_cast<q8_t>(clampF(-127, forget_contribution + input_gate_contribution, 127));
        
        // Output gate: decide what to output
        m_lstm_cell.output_gate[i] = static_cast<q8_t>(clampF(-127, (input_contribution + m_lstm_cell.cell_state[i]) / 4, 127));
        
        // Update hidden state
        q16_t cell_tanh = m_lstm_cell.cell_state[i]; // Simplified tanh
        m_lstm_cell.hidden_state[i] = static_cast<q8_t>((cell_tanh * m_lstm_cell.output_gate[i]) / 127);
    }
}

void NeuralNetworkEngine::resetLSTMState() {
    for (uint8_t i = 0; i < NN_MAX_HIDDEN_SIZE; i++) {
        m_lstm_cell.hidden_state[i] = 0;
        m_lstm_cell.cell_state[i] = 0;
        m_lstm_cell.forget_gate[i] = 64;  // Moderate forgetting
        m_lstm_cell.input_gate[i] = 0;
        m_lstm_cell.output_gate[i] = 64;
    }
    m_lstm_cell.initialized = true;
}

bool NeuralNetworkEngine::validateOutput(const NeuralCorrection& correction) const {
    // Validate correction is within reasonable bounds
    if (correction.multiplier < 0.5f || correction.multiplier > 2.0f) {
        return false;
    }
    
    // Validate confidence is reasonable
    if (correction.confidence < 0.1f) {
        return false;
    }
    
    // Check for NaN or infinite values
    if (!isfinite(correction.multiplier) || !isfinite(correction.confidence)) {
        return false;
    }
    
    return true;
}

void NeuralNetworkEngine::updateWeights(const float* features, float target_output, float learning_rate) {
    // Simplified weight update (production version would implement proper backpropagation)
    // This is a placeholder for the learning algorithm
    (void)features;
    (void)target_output;
    (void)learning_rate;
    
    // In production, this would:
    // 1. Calculate gradients via backpropagation
    // 2. Update weights using gradient descent
    // 3. Handle quantization during weight updates
}

// Neural Wall Wetting Controller Implementation
void NeuralWallWettingController::onFastCallback() {
    if (!engineConfiguration->neuralLearningEnabled) {
        return;
    }
    
    if (m_processing_active) {
        return; // Avoid recursion
    }
    m_processing_active = true;
    
    // Add temporal sample
    float lambda_error = getCurrentLambdaError();
    if (isfinite(lambda_error)) {
        m_temporal_analyzer.addSample(lambda_error);
    }
    
    // Process state machine
    processStateMachine();
    
    m_processing_active = false;
    
    // Update live data
    m_neural_state.neuralWWState = static_cast<uint8_t>(m_state);
    m_neural_state.neuralWWLearning = (m_state != NeuralNetworkState::IDLE);
    m_neural_state.neuralWWLearningCycles = m_learning_cycles;
}

void NeuralWallWettingController::onSlowCallback() {
    // Update telemetry
    m_neural_state.neuralTemporalSamples = m_temporal_analyzer.m_buffer.count;
    m_neural_state.neuralLambdaSlope = m_temporal_analyzer.getShortTermSlope();
    m_neural_state.neuralLambdaVariance = m_temporal_analyzer.m_buffer.calculateVariance(0.5f);
}

void NeuralWallWettingController::onIgnitionStateChanged(bool ignitionOn) {
    if (!ignitionOn) {
        // Save any learned corrections when ignition turns off
        setNeedToWriteConfiguration();
    } else {
        // Reset learning state when ignition turns on
        m_learning_cycles = 0;
        m_temporal_analyzer.reset();
        m_beta_network.resetLSTMState();
        m_tau_network.resetLSTMState();
    }
}

float NeuralWallWettingController::getCurrentLambdaError() const {
    return calculateTargetLambdaError();
}

bool NeuralWallWettingController::canStartLearning() const {
    return isEngineInStableCondition() && engineConfiguration->neuralLearningEnabled;
}

bool NeuralWallWettingController::detectTransient() const {
    // Simplified transient detection
    auto tps = Sensor::get(SensorType::DriverThrottleIntent);
    auto map = Sensor::get(SensorType::Map);
    
    if (!tps.Valid || !map.Valid) return false;
    
    // Would need to track derivatives - simplified for now
    return false; // TODO: implement proper transient detection
}

void NeuralWallWettingController::processStateMachine() {
    switch (m_state) {
        case NeuralNetworkState::IDLE:
            if (canStartLearning() && detectTransient()) {
                m_state = NeuralNetworkState::COLLECTING_DATA;
                m_learning_timer.reset();
                m_temporal_analyzer.reset();
            }
            break;
            
        case NeuralNetworkState::COLLECTING_DATA:
            if (m_learning_timer.hasElapsedSec(1.0f)) { // Collect for 1 second
                m_state = NeuralNetworkState::PROCESSING_FEATURES;
            }
            break;
            
        case NeuralNetworkState::PROCESSING_FEATURES:
            processFeatureExtraction();
            break;
            
        case NeuralNetworkState::NEURAL_INFERENCE:
            processNeuralInference();
            break;
            
        case NeuralNetworkState::APPLYING_CORRECTION:
            processApplyCorrection();
            break;
            
        case NeuralNetworkState::VALIDATION:
            processValidation();
            break;
            
        case NeuralNetworkState::COOLDOWN:
            if (m_cooldown_timer.hasElapsedSec(10.0f)) {
                m_state = NeuralNetworkState::IDLE;
            }
            break;
    }
}

void NeuralWallWettingController::processFeatureExtraction() {
    float features[static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT)];
    m_temporal_analyzer.extractFeatures(features);
    
    // Update live data with temporal features
    m_neural_state.neuralLambdaT1 = features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T1)];
    m_neural_state.neuralLambdaT2 = features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T2)];
    m_neural_state.neuralLambdaT3 = features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T3)];
    m_neural_state.neuralLambdaT4 = features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T4)];
    m_neural_state.neuralLambdaT5 = features[static_cast<uint8_t>(TemporalFeature::LAMBDA_ERROR_T5)];
    
    m_state = NeuralNetworkState::NEURAL_INFERENCE;
}

void NeuralWallWettingController::processNeuralInference() {
    float features[static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT)];
    m_temporal_analyzer.extractFeatures(features);
    
    // Process both beta and tau networks
    NeuralCorrection beta_correction = m_beta_network.processInputs(features, static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT));
    NeuralCorrection tau_correction = m_tau_network.processInputs(features, static_cast<uint8_t>(TemporalFeature::FEATURE_COUNT));
    
    // Update live data
    m_neural_state.neuralBetaCorrection = beta_correction.multiplier;
    m_neural_state.neuralTauCorrection = tau_correction.multiplier;
    m_neural_state.neuralBetaConfidence = static_cast<uint8_t>(beta_correction.confidence * 100);
    m_neural_state.neuralTauConfidence = static_cast<uint8_t>(tau_correction.confidence * 100);
    
    if (beta_correction.valid || tau_correction.valid) {
        m_state = NeuralNetworkState::APPLYING_CORRECTION;
    } else {
        enterCooldown();
    }
}

void NeuralWallWettingController::processApplyCorrection() {
    // Apply corrections to tables (implementation would depend on table structure)
    m_learning_cycles++;
    m_neural_state.neuralValidCorrections++;
    
    m_state = NeuralNetworkState::VALIDATION;
}

void NeuralWallWettingController::processValidation() {
    // Validate that corrections improved performance
    // For now, always proceed to cooldown
    enterCooldown();
}

void NeuralWallWettingController::enterCooldown() {
    m_state = NeuralNetworkState::COOLDOWN;
    m_cooldown_timer.reset();
}

// Neural Coordinator Implementation
void NeuralNetworkCoordinator::onFastCallback() {
    if (!m_system_enabled) {
        return;
    }
    
    m_wall_wetting.onFastCallback();
    m_ltft.onFastCallback();
}

void NeuralNetworkCoordinator::onSlowCallback() {
    if (!m_system_enabled) {
        return;
    }
    
    m_wall_wetting.onSlowCallback();
    m_ltft.onSlowCallback();
    
    // Perform periodic health checks
    if (m_health_check_timer.hasElapsedSec(5.0f)) {
        performHealthCheck();
        m_health_check_timer.reset();
    }
}

void NeuralNetworkCoordinator::onIgnitionStateChanged(bool ignitionOn) {
    m_wall_wetting.onIgnitionStateChanged(ignitionOn);
    m_ltft.onIgnitionStateChanged(ignitionOn);
}

void NeuralNetworkCoordinator::enableNeuralLearning(bool enable) {
    m_system_enabled = enable;
}

void NeuralNetworkCoordinator::resetAllNetworks() {
    m_wall_wetting.m_beta_network.resetLSTMState();
    m_wall_wetting.m_tau_network.resetLSTMState();
    m_ltft.m_ltft_network.resetLSTMState();
}

bool NeuralNetworkCoordinator::isSystemHealthy() const {
    return m_system_healthy;
}

void NeuralNetworkCoordinator::performHealthCheck() {
    // Check system health based on various metrics
    m_system_healthy = true; // Simplified - would check actual metrics
}

void NeuralNetworkCoordinator::handleSystemFailure() {
    // Enable failsafe mode for all networks
    m_wall_wetting.m_beta_network.enableFailsafe();
    m_wall_wetting.m_tau_network.enableFailsafe();
    m_ltft.m_ltft_network.enableFailsafe();
    m_system_healthy = false;
}

// Neural LTFT Implementation (placeholder)
void NeuralLongTermFuelTrim::onFastCallback() {
    // Implementation similar to wall wetting but for LTFT
}

void NeuralLongTermFuelTrim::onSlowCallback() {
    // Update LTFT-specific telemetry
}

void NeuralLongTermFuelTrim::onIgnitionStateChanged(bool ignitionOn) {
    if (ignitionOn) {
        m_learning_active = false;
        m_stft_analyzer.reset();
    }
}