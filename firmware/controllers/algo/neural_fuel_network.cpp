#include "pch.h"
#include "neural_fuel_network.h"
#include <rusefi/math.h>

#if EFI_ENGINE_CONTROL

// =============================================================================
// MATHEMATICAL UTILITY FUNCTIONS
// =============================================================================

/**
 * Compute linear regression slope for temporal analysis
 * Used for lambda slope analysis (short and long term trends)
 */
float computeLinearRegressionSlope(const float* y, const float* x, size_t n) {
    if (n < 2) return 0.0f;
    
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    
    for (size_t i = 0; i < n; i++) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    float denominator = n * sumX2 - sumX * sumX;
    if (fabsf(denominator) < 1e-6f) {
        return 0.0f;
    }
    
    return (n * sumXY - sumX * sumY) / denominator;
}

/**
 * Compute variance of float array
 * Used for lambda error variance calculation
 */
float computeVariance(const float* data, size_t n) {
    if (n < 2) return 0.0f;
    
    // Calculate mean
    float sum = 0.0f;
    for (size_t i = 0; i < n; i++) {
        sum += data[i];
    }
    float mean = sum / n;
    
    // Calculate variance
    float variance = 0.0f;
    for (size_t i = 0; i < n; i++) {
        float diff = data[i] - mean;
        variance += diff * diff;
    }
    
    return variance / (n - 1);
}

/**
 * Compute moving average trend
 * Used for lambda trend analysis
 */
float computeMovingAverageTrend(const float* data, size_t n, size_t windowSize) {
    if (n < windowSize || windowSize < 2) return 0.0f;
    
    // Calculate first half average
    float firstHalfSum = 0.0f;
    size_t firstHalfSize = windowSize / 2;
    for (size_t i = 0; i < firstHalfSize; i++) {
        firstHalfSum += data[i];
    }
    float firstHalfAvg = firstHalfSum / firstHalfSize;
    
    // Calculate second half average
    float secondHalfSum = 0.0f;
    size_t secondHalfSize = windowSize - firstHalfSize;
    for (size_t i = firstHalfSize; i < windowSize; i++) {
        secondHalfSum += data[n - windowSize + i];
    }
    float secondHalfAvg = secondHalfSum / secondHalfSize;
    
    // Return trend (positive = increasing, negative = decreasing)
    return secondHalfAvg - firstHalfAvg;
}

// =============================================================================
// TEMPORAL BUFFER UTILITY FUNCTIONS
// =============================================================================

/**
 * Get lambda error at specific time offset from current position
 * timeOffsetMs: offset in milliseconds (e.g., 100 for T-100ms)
 * samplingRate: buffer sampling rate in Hz (e.g., 50 for 50Hz)
 */
float getLambdaErrorAtTimeOffset(const temporal_buffer_s& buffer, float timeOffsetMs, float samplingRate) {
    if (!buffer.bufferFull) return 0.0f;
    
    // Calculate sample offset
    int sampleOffset = (int)(timeOffsetMs * samplingRate / 1000.0f);
    if (sampleOffset >= TEMPORAL_BUFFER_SIZE) return 0.0f;
    
    // Calculate index (going backwards from current position)
    int targetIndex = (int)buffer.currentIndex - sampleOffset - 1;
    if (targetIndex < 0) {
        targetIndex += TEMPORAL_BUFFER_SIZE;
    }
    
    return buffer.lambdaErrors[targetIndex];
}

/**
 * Extract time window from buffer for analysis
 */
void extractTimeWindow(const temporal_buffer_s& buffer, float* output, size_t outputSize, 
                      float startTimeMs, float endTimeMs, float samplingRate) {
    if (!buffer.bufferFull || outputSize == 0) return;
    
    // Calculate sample range
    int startSampleOffset = (int)(startTimeMs * samplingRate / 1000.0f);
    int endSampleOffset = (int)(endTimeMs * samplingRate / 1000.0f);
    
    // Extract samples
    for (size_t i = 0; i < outputSize; i++) {
        int sampleOffset = startSampleOffset + (int)((endSampleOffset - startSampleOffset) * i / outputSize);
        if (sampleOffset >= TEMPORAL_BUFFER_SIZE) break;
        
        int targetIndex = (int)buffer.currentIndex - sampleOffset - 1;
        if (targetIndex < 0) {
            targetIndex += TEMPORAL_BUFFER_SIZE;
        }
        
        output[i] = buffer.lambdaErrors[targetIndex];
    }
}

// =============================================================================
// QUANTIZATION UTILITY FUNCTIONS
// =============================================================================

/**
 * Quantize float to int16 with specified scale
 */
int16_t quantizeFloat(float value, int16_t scale) {
    // Clamp to prevent overflow
    float scaledValue = value * scale;
    scaledValue = clampF(-32767.0f, scaledValue, 32767.0f);
    return (int16_t)scaledValue;
}

/**
 * Dequantize int16 to float with specified scale
 */
float dequantizeInt(int16_t value, int16_t scale) {
    return (float)value / (float)scale;
}

/**
 * Quantize array of floats to int16 array
 */
void quantizeFloatArray(const float* input, int16_t* output, size_t count, int16_t scale) {
    for (size_t i = 0; i < count; i++) {
        output[i] = quantizeFloat(input[i], scale);
    }
}

/**
 * Dequantize array of int16 to float array
 */
void dequantizeIntArray(const int16_t* input, float* output, size_t count, int16_t scale) {
    for (size_t i = 0; i < count; i++) {
        output[i] = dequantizeInt(input[i], scale);
    }
}

// =============================================================================
// NEURAL NETWORK MATRIX OPERATIONS (QUANTIZED)
// =============================================================================

/**
 * Quantized matrix-vector multiplication: result = weights * input + bias
 * Uses int32 for intermediate calculations to prevent overflow
 */
void quantizedMatVec(const int16_t weights[][NEURAL_INPUT_COUNT], const int16_t* input, 
                    const int16_t* bias, int16_t* output, size_t outputSize) {
    for (size_t i = 0; i < outputSize; i++) {
        int32_t sum = 0;
        
        // Matrix multiplication
        for (size_t j = 0; j < NEURAL_INPUT_COUNT; j++) {
            sum += (int32_t)weights[i][j] * (int32_t)input[j];
        }
        
        // Add bias (scaled appropriately)
        sum += (int32_t)bias[i] << 12;  // Bias scale adjustment
        
        // Scale down and clamp
        output[i] = (int16_t)clampI(-32767, sum >> 12, 32767);
    }
}

/**
 * Quantized hidden-state matrix multiplication for LSTM gates
 */
void quantizedHiddenMatVec(const int16_t weights[][NEURAL_HIDDEN_SIZE], const int16_t* hiddenState,
                          const int16_t* inputContribution, int16_t* output, size_t outputSize) {
    for (size_t i = 0; i < outputSize; i++) {
        int32_t sum = (int32_t)inputContribution[i];  // Start with input contribution
        
        // Add hidden state contribution
        for (size_t j = 0; j < NEURAL_HIDDEN_SIZE; j++) {
            sum += (int32_t)weights[i][j] * (int32_t)hiddenState[j];
        }
        
        // Scale and clamp
        output[i] = (int16_t)clampI(-32767, sum >> 12, 32767);
    }
}

// =============================================================================
// LSTM CELL IMPLEMENTATION (QUANTIZED)
// =============================================================================

/**
 * Process single LSTM cell forward pass with quantized arithmetic
 * This is the core of the neural network computation
 */
void processLSTMCell(const neural_weights_s& weights, const int16_t* inputs, 
                    lstm_state_s& state) {
    
    // Temporary arrays for gate computations
    int16_t forgetGate[NEURAL_HIDDEN_SIZE];
    int16_t inputGate[NEURAL_HIDDEN_SIZE];
    int16_t candidateValues[NEURAL_HIDDEN_SIZE];
    int16_t outputGate[NEURAL_HIDDEN_SIZE];
    
    // Temporary arrays for input contributions
    int16_t forgetInput[NEURAL_HIDDEN_SIZE];
    int16_t inputInput[NEURAL_HIDDEN_SIZE];
    int16_t candidateInput[NEURAL_HIDDEN_SIZE];
    int16_t outputInput[NEURAL_HIDDEN_SIZE];
    
    // === STEP 1: Compute input contributions for all gates ===
    quantizedMatVec(weights.forgetU, inputs, weights.forgetBias, forgetInput, NEURAL_HIDDEN_SIZE);
    quantizedMatVec(weights.inputU, inputs, weights.inputBias, inputInput, NEURAL_HIDDEN_SIZE);
    quantizedMatVec(weights.candidateU, inputs, weights.candidateBias, candidateInput, NEURAL_HIDDEN_SIZE);
    quantizedMatVec(weights.outputU, inputs, weights.outputBias, outputInput, NEURAL_HIDDEN_SIZE);
    
    // === STEP 2: Add hidden state contributions and apply activations ===
    
    // Forget gate: f_t = sigmoid(W_f * h_{t-1} + U_f * x_t + b_f)
    quantizedHiddenMatVec(weights.forgetW, state.hiddenState, forgetInput, forgetGate, NEURAL_HIDDEN_SIZE);
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        forgetGate[i] = quantizedSigmoid(forgetGate[i]);
    }
    
    // Input gate: i_t = sigmoid(W_i * h_{t-1} + U_i * x_t + b_i)
    quantizedHiddenMatVec(weights.inputW, state.hiddenState, inputInput, inputGate, NEURAL_HIDDEN_SIZE);
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        inputGate[i] = quantizedSigmoid(inputGate[i]);
    }
    
    // Candidate values: g_t = tanh(W_g * h_{t-1} + U_g * x_t + b_g)
    quantizedHiddenMatVec(weights.candidateW, state.hiddenState, candidateInput, candidateValues, NEURAL_HIDDEN_SIZE);
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        candidateValues[i] = quantizedTanh(candidateValues[i]);
    }
    
    // Output gate: o_t = sigmoid(W_o * h_{t-1} + U_o * x_t + b_o)
    quantizedHiddenMatVec(weights.outputW, state.hiddenState, outputInput, outputGate, NEURAL_HIDDEN_SIZE);
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        outputGate[i] = quantizedSigmoid(outputGate[i]);
    }
    
    // === STEP 3: Update cell state ===
    // C_t = f_t * C_{t-1} + i_t * g_t
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        int32_t forgetTerm = ((int32_t)forgetGate[i] * (int32_t)state.cellState[i]) >> 15;
        int32_t inputTerm = ((int32_t)inputGate[i] * (int32_t)candidateValues[i]) >> 15;
        state.cellState[i] = (int16_t)clampI(-32767, forgetTerm + inputTerm, 32767);
    }
    
    // === STEP 4: Update hidden state ===
    // h_t = o_t * tanh(C_t)
    for (size_t i = 0; i < NEURAL_HIDDEN_SIZE; i++) {
        int16_t cellTanh = quantizedTanh(state.cellState[i]);
        int32_t hiddenNew = ((int32_t)outputGate[i] * (int32_t)cellTanh) >> 15;
        state.hiddenState[i] = (int16_t)clampI(-32767, hiddenNew, 32767);
    }
    
    // Mark as initialized
    state.initialized = true;
}

// =============================================================================
// OUTPUT LAYER PROCESSING
// =============================================================================

/**
 * Process output layer: final linear transformation from hidden states to predictions
 */
void processOutputLayer(const neural_weights_s& weights, const lstm_state_s& state, 
                       int16_t* outputs) {
    for (size_t i = 0; i < NEURAL_OUTPUT_COUNT; i++) {
        int32_t sum = 0;
        
        // Linear combination of hidden states
        for (size_t j = 0; j < NEURAL_HIDDEN_SIZE; j++) {
            sum += (int32_t)weights.outputLayerW[i][j] * (int32_t)state.hiddenState[j];
        }
        
        // Add bias
        sum += (int32_t)weights.outputLayerBias[i] << 12;
        
        // Scale and clamp result
        outputs[i] = (int16_t)clampI(-32767, sum >> 12, 32767);
    }
}

// =============================================================================
// CONFIDENCE CALCULATION
// =============================================================================

/**
 * Calculate confidence score based on output consistency and magnitude
 * Higher confidence when outputs are stable and within expected ranges
 */
float calculateNeuralConfidence(const float* outputs, const neural_fuel_telemetry_s& telemetry) {
    // Base confidence from output magnitude (neither too large nor too small)
    float betaMagnitude = fabsf(outputs[0]);  // Beta prediction
    float tauMagnitude = fabsf(outputs[1]);   // Tau prediction
    
    // Ideal range confidence (peak at moderate corrections)
    float betaConfidence = 1.0f - clampF(0.0f, (betaMagnitude - 0.1f) / 0.4f, 1.0f);
    float tauConfidence = 1.0f - clampF(0.0f, (tauMagnitude - 0.1f) / 0.4f, 1.0f);
    
    // Combine magnitude confidences
    float magnitudeConfidence = (betaConfidence + tauConfidence) * 0.5f;
    
    // Historical success rate factor
    float successRate = telemetry.successRate;
    float historyConfidence = clampF(0.1f, successRate, 1.0f);
    
    // Temporal consistency (reduce confidence if predictions are changing rapidly)
    float previousBeta = telemetry.betaPrediction;
    float previousTau = telemetry.tauPrediction;
    float betaChange = fabsf(outputs[0] - previousBeta);
    float tauChange = fabsf(outputs[1] - previousTau);
    float stabilityConfidence = 1.0f - clampF(0.0f, (betaChange + tauChange) / 0.2f, 0.5f);
    
    // Combine all confidence factors
    float finalConfidence = magnitudeConfidence * 0.4f + 
                           historyConfidence * 0.4f + 
                           stabilityConfidence * 0.2f;
    
    return clampF(0.0f, finalConfidence, 1.0f);
}

// =============================================================================
// NEURAL NETWORK VALIDATION
// =============================================================================

/**
 * Validate neural network outputs for safety
 */
bool validateNeuralOutputs(const float* outputs) {
    // Check for NaN or infinite values
    for (size_t i = 0; i < NEURAL_OUTPUT_COUNT; i++) {
        if (!isfinite(outputs[i])) {
            return false;
        }
    }
    
    // Check ranges
    if (fabsf(outputs[0]) > 0.5f) return false;  // Beta correction range
    if (fabsf(outputs[1]) > 0.5f) return false;  // Tau correction range
    if (outputs[2] < 0.0f || outputs[2] > 1.0f) return false;  // Confidence range
    if (outputs[3] < 0.0f || outputs[3] > 1.0f) return false;  // Learning rate range
    
    return true;
}

/**
 * Sanitize neural outputs (clamp to safe ranges)
 */
void sanitizeNeuralOutputs(float* outputs) {
    outputs[0] = clampF(-0.5f, outputs[0], 0.5f);   // Beta correction
    outputs[1] = clampF(-0.5f, outputs[1], 0.5f);   // Tau correction
    outputs[2] = clampF(0.0f, outputs[2], 1.0f);    // Confidence
    outputs[3] = clampF(0.0f, outputs[3], 1.0f);    // Learning rate
}

#endif // EFI_ENGINE_CONTROL