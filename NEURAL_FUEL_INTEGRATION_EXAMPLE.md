# Neural Fuel System - Integration Example

This document shows how to integrate the Neural Fuel System with the main RusEFI engine code.

## 1. Engine Integration

### 1.1. Add to Engine Module Registration

In `firmware/controllers/algo/engine.cpp`, add the neural fuel controller:

```cpp
#include "neural_fuel_controller.h"

// In Engine constructor or initialization
void Engine::initializeControllers() {
    // ... existing controllers ...
    
    // Initialize neural fuel controller
    m_neuralFuelController = &engine->module<NeuralFuelController>();
    
    // Integrate with existing systems
    auto* awwController = &engine->module<AdaptiveWallWettingController>();
    auto* ltftController = &engine->module<LongTermFuelTrim>();
    
    m_neuralFuelController->integrateWithAdaptiveWW(awwController);
    m_neuralFuelController->integrateWithLTFT(ltftController);
}
```

### 1.2. Add to Engine Header

In `firmware/controllers/algo/engine.h`:

```cpp
class Engine {
    // ... existing members ...
    
private:
    NeuralFuelController* m_neuralFuelController = nullptr;
    
public:
    // Accessor for neural fuel controller
    NeuralFuelController* getNeuralFuelController() const {
        return m_neuralFuelController;
    }
};
```

## 2. Configuration Variables

### 2.1. Add to Engine Configuration

In the configuration structure (likely in a .txt file that generates config):

```cpp
// Neural Fuel System Configuration
bool neuralFuelEnabled = false;
float neuralFuelConfidenceThreshold = 0.7f;
float neuralFuelLearningRate = 0.001f;
uint16_t neuralFuelUpdateInterval = 20; // ms (50Hz)

// Neural network weights (read-only for diagnostics)
float[200] neuralFuelWeights;
float[20] neuralFuelBiases;

// Advanced configuration
float neuralFuelMaxBetaCorrection = 0.5f;
float neuralFuelMaxTauCorrection = 0.5f;
float neuralFuelMinConfidenceThreshold = 0.3f;
```

### 2.2. TunerStudio Integration

Add to TunerStudio .ini template:

```ini
[Neural Fuel System]
dialog = neuralFuelDialog, "Neural Fuel System"

field = "Enable Neural Fuel", neuralFuelEnabled
field = "Confidence Threshold", neuralFuelConfidenceThreshold, {neuralFuelEnabled}
field = "Learning Rate", neuralFuelLearningRate, {neuralFuelEnabled}
field = "Update Interval (ms)", neuralFuelUpdateInterval, {neuralFuelEnabled}

[Neural Fuel Live Data]
panel = neuralFuelLiveData
    ; Neural inputs
    gauge = lambdaErrorT1Gauge, "Lambda Error T1", "neuralFuel.lambdaErrorT1", -0.5, 0.5, -0.3, -0.1, 0.1, 0.3
    gauge = lambdaErrorT2Gauge, "Lambda Error T2", "neuralFuel.lambdaErrorT2", -0.5, 0.5, -0.3, -0.1, 0.1, 0.3
    gauge = confidenceGauge, "Neural Confidence", "neuralFuel.confidenceScore", 0, 1, 0.3, 0.5, 0.7, 0.9
    
    ; Neural outputs
    gauge = betaPredictionGauge, "Beta Prediction", "neuralFuel.betaPrediction", -0.5, 0.5, -0.3, -0.1, 0.1, 0.3
    gauge = tauPredictionGauge, "Tau Prediction", "neuralFuel.tauPrediction", -0.5, 0.5, -0.3, -0.1, 0.1, 0.3
    
    ; Status indicators
    indicator = {neuralFuel.isEnabled}, "Neural Enabled", red, green
    indicator = {neuralFuel.isActive}, "Neural Active", red, green
    indicator = {neuralFuel.fallbackMode}, "Fallback Mode", green, red
```

## 3. Usage Examples

### 3.1. Basic Enable/Disable

```cpp
// Enable neural fuel system
engine->getNeuralFuelController()->setEnabled(true);

// Set confidence threshold
engine->getNeuralFuelController()->setConfidenceThreshold(0.8f);

// Check if system is active
if (engine->getNeuralFuelController()->isActive()) {
    // Neural system is running and making predictions
}
```

### 3.2. Getting Neural Predictions

```cpp
// Get current neural predictions
auto* neuralController = engine->getNeuralFuelController();

if (neuralController->isEnabled() && !neuralController->isFallbackMode()) {
    float betaPrediction = neuralController->getBetaPrediction();
    float tauPrediction = neuralController->getTauPrediction();
    float confidence = neuralController->getConfidence();
    
    // Use predictions only if confidence is high enough
    if (confidence > 0.7f) {
        // Apply neural corrections to fuel calculations
        // This would be integrated into the wall wetting calculation
    }
}
```

### 3.3. Telemetry Access

```cpp
// Get complete telemetry for logging/diagnostics
const auto& telemetry = engine->getNeuralFuelController()->getTelemetry();

// Log important metrics
logf("Neural: Active=%d, Confidence=%.3f, Predictions: Beta=%.3f, Tau=%.3f", 
     telemetry.isActive, 
     telemetry.currentConfidence,
     telemetry.betaPrediction, 
     telemetry.tauPrediction);

// Check performance statistics
logf("Neural Stats: Success Rate=%.1f%%, Total Inferences=%d", 
     telemetry.successRate * 100.0f, 
     telemetry.totalInferences);
```

### 3.4. Error Handling

```cpp
auto* neuralController = engine->getNeuralFuelController();

// Check for emergency conditions
if (neuralController->isFallbackMode()) {
    // System has fallen back to classical methods
    // Neural predictions should not be used
}

// Manual recovery attempt
if (neuralController->isFallbackMode()) {
    neuralController->attemptRecovery();
}

// Emergency shutdown
if (criticalError) {
    neuralController->enterEmergencyMode();
}
```

## 4. Integration with Wall Wetting

### 4.1. Modified Wall Fuel Calculation

```cpp
// In wall_fuel.cpp - modified calculation with neural assistance
float WallFuelController::computeBetaWithNeural() const {
    float baseBeta = computeBeta(); // Existing calculation
    
    auto* neuralController = engine->getNeuralFuelController();
    
    // Apply neural correction if available and confident
    if (neuralController && neuralController->isActive()) {
        float confidence = neuralController->getConfidence();
        float neuralBeta = neuralController->getBetaPrediction();
        
        if (confidence > 0.7f) {
            // Blend classical and neural predictions
            float neuralWeight = confidence * 0.5f; // Max 50% neural influence
            return baseBeta * (1.0f - neuralWeight) + neuralBeta * neuralWeight;
        }
    }
    
    return baseBeta;
}
```

### 4.2. Live Data Update

```cpp
// In output channels update
void updateOutputChannels() {
    // ... existing output channels ...
    
    // Update neural fuel live data
    auto* neuralController = engine->getNeuralFuelController();
    if (neuralController) {
        const auto& telemetry = neuralController->getTelemetry();
        
        // Copy telemetry to output channels for TunerStudio
        engine->outputChannels.neuralFuel = neuralFuel_live_data_s{
            .lambdaErrorT1 = telemetry.lambdaErrorT1,
            .lambdaErrorT2 = telemetry.lambdaErrorT2,
            .lambdaErrorT3 = telemetry.lambdaErrorT3,
            .lambdaErrorT4 = telemetry.lambdaErrorT4,
            .lambdaErrorT5 = telemetry.lambdaErrorT5,
            .lambdaSlopeShort = telemetry.lambdaSlopeShort,
            .lambdaSlopeLong = telemetry.lambdaSlopeLong,
            .lambdaVariance = telemetry.lambdaVariance,
            .lambdaTrend = telemetry.lambdaTrend,
            .loadNormalized = telemetry.loadNormalized,
            .rpmNormalized = telemetry.rpmNormalized,
            .transientMagnitude = telemetry.transientMagnitude,
            .betaPrediction = telemetry.betaPrediction,
            .tauPrediction = telemetry.tauPrediction,
            .confidenceScore = telemetry.currentConfidence,
            .adaptiveLearningRate = telemetry.adaptiveLearningRate,
            .hiddenState0 = telemetry.hiddenState0,
            .hiddenState1 = telemetry.hiddenState1,
            .hiddenState2 = telemetry.hiddenState2,
            .hiddenState3 = telemetry.hiddenState3,
            .totalInferences = telemetry.totalInferences,
            .successfulPredictions = telemetry.successfulPredictions,
            .successRate = telemetry.successRate,
            .isEnabled = telemetry.isEnabled,
            .isActive = telemetry.isActive,
            .fallbackMode = telemetry.fallbackMode,
            .bufferReady = telemetry.bufferReady,
            .emergencyMode = telemetry.emergencyMode
        };
    }
}
```

## 5. Compilation and Testing

### 5.1. Compilation Steps

1. Ensure all neural fuel files are in the makefile (`algo.mk`)
2. Rebuild configuration generation if config variables were added
3. Compile firmware with neural system enabled
4. Verify no compilation errors

### 5.2. Initial Testing

```cpp
// Simple test in engine initialization
void testNeuralFuelSystem() {
    auto* neuralController = engine->getNeuralFuelController();
    
    // Test basic functionality
    neuralController->setEnabled(true);
    
    // Wait for buffer to fill (need 1 second of data)
    Timer testTimer;
    testTimer.reset();
    
    while (!testTimer.hasElapsedSec(2.0f)) {
        // Simulate engine running
        neuralController->onFastCallback();
        if (testTimer.hasElapsedSec(0.02f)) {
            neuralController->onSlowCallback();
            testTimer.reset();
        }
    }
    
    // Check if system is working
    if (neuralController->isActive()) {
        logf("Neural fuel system test: PASSED");
    } else {
        logf("Neural fuel system test: FAILED");
    }
}
```

## 6. Troubleshooting

### 6.1. Common Issues

1. **Neural system not activating**: Check sensor validity, especially lambda sensor
2. **Low confidence scores**: May need trained weights instead of default random values
3. **Compilation errors**: Ensure all includes and makefile entries are correct
4. **High CPU usage**: Check if neural inference frequency is appropriate

### 6.2. Debugging

```cpp
// Debug neural system state
void debugNeuralFuel() {
    auto* neuralController = engine->getNeuralFuelController();
    const auto& telemetry = neuralController->getTelemetry();
    
    logf("Neural Debug:");
    logf("  Enabled: %d, Active: %d, Fallback: %d", 
         telemetry.isEnabled, telemetry.isActive, telemetry.fallbackMode);
    logf("  Buffer Ready: %d, Consecutive Failures: %d", 
         telemetry.bufferReady, telemetry.consecutiveFailures);
    logf("  Confidence: %.3f, Success Rate: %.1f%%", 
         telemetry.currentConfidence, telemetry.successRate * 100.0f);
}
```

This integration example provides the foundation for incorporating the neural fuel system into the main RusEFI codebase. The system is designed to be safely disabled by default and gradually enabled as confidence in the implementation grows.