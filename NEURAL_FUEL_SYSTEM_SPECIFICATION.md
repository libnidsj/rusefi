# Sistema de Rede Neural para Combustível - Especificação Completa

## 1. Visão Geral

O Sistema de Rede Neural para Combustível é um controlador inteligente que complementa os sistemas existentes de **Adaptive Wall Wetting** e **Long Term Fuel Trim (LTFT)** no RusEFI, adicionando capacidade de aprendizado temporal avançado através de uma arquitetura neural otimizada para MCU.

### 1.1. Objetivos Principais

- **Predição Temporal**: Antecipar necessidades de correção baseadas em padrões históricos
- **Integração Inteligente**: Combinar dados de AWW, LTFT e sensores para decisões otimizadas  
- **Adaptação Dinâmica**: Ajustar continuamente baseado em condições operacionais
- **Failsafe Robusto**: Operar em conjunto com sistemas clássicos com fallback seguro

### 1.2. Arquitetura do Sistema

```
[Sensores] → [Buffer Temporal] → [Rede Neural LSTM] → [Correções Preditivas] → [Integração com AWW/LTFT]
     ↓              ↓                    ↓                      ↓                        ↓
[Lambda,        [Histórico        [Processamento      [Correções Beta/Tau    [Sistema Final
 TPS, MAP,       T1-T5,            Quantizado,         Preditivas,              Híbrido
 RPM, etc.]      Slopes,           Hidden States]      Slope Analysis]          Clássico+Neural]
                 Estatísticas]
```

## 2. Especificações Técnicas

### 2.1. Inputs da Rede Neural (12 inputs total)

#### 2.1.1. Inputs Temporais de Lambda (5 inputs)
- `LAMBDA_ERROR_T1`: Erro lambda atual (t=0)
- `LAMBDA_ERROR_T2`: Erro lambda t-100ms  
- `LAMBDA_ERROR_T3`: Erro lambda t-200ms
- `LAMBDA_ERROR_T4`: Erro lambda t-500ms
- `LAMBDA_ERROR_T5`: Erro lambda t-1000ms

#### 2.1.2. Análise de Slope Temporal (2 inputs)
- `LAMBDA_SLOPE_SHORT`: Slope de 200ms (regressão linear)
- `LAMBDA_SLOPE_LONG`: Slope de 1000ms (regressão linear)

#### 2.1.3. Features Estatísticas (2 inputs)
- `LAMBDA_VARIANCE`: Variância do erro lambda (janela 1s)
- `LAMBDA_TREND`: Tendência temporal (média móvel)

#### 2.1.4. Contexto Operacional (3 inputs)
- `ENGINE_LOAD_NORMALIZED`: Carga normalizada [0-1]
- `RPM_NORMALIZED`: RPM normalizado [0-1] 
- `TRANSIENT_MAGNITUDE`: Magnitude do transiente detectado

### 2.2. Outputs da Rede Neural (4 outputs)

- `BETA_CORRECTION_PREDICTION`: Predição de correção Beta [-0.5, +0.5]
- `TAU_CORRECTION_PREDICTION`: Predição de correção Tau [-0.5, +0.5]
- `CONFIDENCE_SCORE`: Nível de confiança da predição [0-1]
- `LEARNING_RATE_ADAPTIVE`: Taxa de aprendizado adaptativa [0-1]

### 2.3. Arquitetura da Rede Neural

#### 2.3.1. Estrutura LSTM Simplificada
```cpp
// Camada de Input (12 → 8)
Input Layer: 12 neurons → 8 neurons (linear transformation)

// Camada LSTM simplificada (8 → 8)
LSTM Cell: 8 hidden states, 8 cell states
- Forget Gate: sigmoid(Wf * [h(t-1), x(t)] + bf)
- Input Gate: sigmoid(Wi * [h(t-1), x(t)] + bi)  
- Candidate: tanh(Wc * [h(t-1), x(t)] + bc)
- Output Gate: sigmoid(Wo * [h(t-1), x(t)] + bo)

// Camada de Output (8 → 4)
Output Layer: 8 neurons → 4 neurons (linear + activation)
```

#### 2.3.2. Quantização para MCU
- **Pesos**: int16_t com fator de escala 2^12 (4096)
- **Ativações**: int16_t com range [-32768, 32767]
- **Bias**: int32_t para maior precisão
- **Operações**: Apenas soma, multiplicação e shift (sem divisão/ponto flutuante)

#### 2.3.3. Memória e Performance
- **Pesos totais**: ~800 bytes (200 pesos x 4 bytes)
- **Estados LSTM**: 64 bytes (16 estados x 4 bytes)
- **Tempo de execução**: <50µs por inferência
- **Frequência**: 50Hz (integrado ao onSlowCallback)

## 3. Implementação de Software

### 3.1. Estruturas de Dados

```cpp
// Configuração da rede neural
struct neural_fuel_config_s {
    bool enabled = false;
    float learningRate = 0.001f;
    float confidenceThreshold = 0.7f;
    uint16_t updateIntervalMs = 20; // 50Hz
    
    // Quantization parameters
    int16_t weightScale = 4096;
    int16_t activationScale = 32767;
};

// Estados da rede LSTM
struct lstm_state_s {
    int16_t hiddenState[8];
    int16_t cellState[8];
    bool initialized = false;
};

// Buffer temporal para inputs
struct temporal_buffer_s {
    float lambdaErrors[50];    // 1s buffer at 50Hz
    float timestamps[50];
    uint8_t currentIndex = 0;
    bool bufferFull = false;
};

// Dados de telemetria
struct neural_fuel_telemetry_s {
    float currentConfidence = 0;
    float betaPrediction = 0;
    float tauPrediction = 0;
    float learningRate = 0;
    uint32_t totalInferences = 0;
    uint32_t successfulPredictions = 0;
    bool isActive = false;
    bool fallbackMode = false;
};
```

### 3.2. Classe Principal

```cpp
class NeuralFuelController : public EngineModule {
public:
    using interface_t = NeuralFuelController;
    
    void onSlowCallback() override;  // 50Hz para rede neural
    void onFastCallback() override; // 200Hz para coleta de dados
    void onIgnitionStateChanged(bool ignitionOn) override;
    
    // Interface pública
    bool isEnabled() const { return m_config.enabled; }
    float getConfidence() const { return m_telemetry.currentConfidence; }
    float getBetaPrediction() const { return m_telemetry.betaPrediction; }
    float getTauPrediction() const { return m_telemetry.tauPrediction; }
    
    // Integração com sistemas existentes
    void integrateWithAdaptiveWW(AdaptiveWallWettingController* aww);
    void integrateWithLTFT(LongTermFuelTrim* ltft);
    
private:
    neural_fuel_config_s m_config;
    lstm_state_s m_lstmState;
    temporal_buffer_s m_temporalBuffer;
    neural_fuel_telemetry_s m_telemetry;
    
    // Timers
    Timer m_updateTimer;
    Timer m_learningTimer;
    Timer m_ignitionOnTimer;
    
    // Integração com sistemas existentes
    AdaptiveWallWettingController* m_adaptiveWW = nullptr;
    LongTermFuelTrim* m_ltft = nullptr;
    
    // Métodos principais
    void collectTemporalData();
    void processNeuralInference();
    void updateLSTMStates();
    void computeSlopes();
    bool validateInputs();
    void applyPredictions();
    
    // Rede neural
    void quantizeInputs(const float* inputs, int16_t* quantized);
    void runLSTMForward(const int16_t* inputs, int16_t* outputs);
    void dequantizeOutputs(const int16_t* quantized, float* outputs);
    
    // Utilitários
    float calculateConfidence(const float* outputs);
    void resetNeuralState();
    bool checkOperationalConditions();
    void updateTelemetry();
};
```

## 4. Algoritmos de Processamento

### 4.1. Coleta de Dados Temporais

```cpp
void NeuralFuelController::collectTemporalData() {
    // Obter lambda error usando target correto
    auto lambda = Sensor::get(SensorType::Lambda1);
    if (!lambda.Valid) return;
    
    float targetLambda = engine->fuelComputer.targetLambda; // CORRETO!
    float lambdaError = lambda.Value - targetLambda;
    
    // Adicionar ao buffer circular
    m_temporalBuffer.lambdaErrors[m_temporalBuffer.currentIndex] = lambdaError;
    m_temporalBuffer.timestamps[m_temporalBuffer.currentIndex] = getTimeNowNt() * 1e-6f;
    
    m_temporalBuffer.currentIndex = (m_temporalBuffer.currentIndex + 1) % 50;
    if (m_temporalBuffer.currentIndex == 0) {
        m_temporalBuffer.bufferFull = true;
    }
}
```

### 4.2. Análise de Slope Temporal

```cpp
void NeuralFuelController::computeSlopes() {
    if (!m_temporalBuffer.bufferFull) return;
    
    // Slope de curto prazo (últimos 200ms ~ 10 amostras)
    float shortSlope = computeLinearRegressionSlope(
        &m_temporalBuffer.lambdaErrors[m_temporalBuffer.currentIndex - 10],
        &m_temporalBuffer.timestamps[m_temporalBuffer.currentIndex - 10],
        10
    );
    
    // Slope de longo prazo (últimos 1000ms ~ 50 amostras)
    float longSlope = computeLinearRegressionSlope(
        m_temporalBuffer.lambdaErrors,
        m_temporalBuffer.timestamps,
        50
    );
    
    // Armazenar para uso na rede neural
    m_currentInputs[5] = shortSlope;
    m_currentInputs[6] = longSlope;
}

float NeuralFuelController::computeLinearRegressionSlope(
    const float* y, const float* x, int n) {
    
    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    
    for (int i = 0; i < n; i++) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    float denominator = n * sumX2 - sumX * sumX;
    if (fabsf(denominator) < 1e-6f) return 0.0f;
    
    return (n * sumXY - sumX * sumY) / denominator;
}
```

### 4.3. Processamento LSTM Quantizado

```cpp
void NeuralFuelController::runLSTMForward(const int16_t* inputs, int16_t* outputs) {
    // Forget gate: f_t = sigmoid(W_f * [h_{t-1}, x_t] + b_f)
    int32_t forgetGate[8];
    for (int i = 0; i < 8; i++) {
        int32_t sum = 0;
        // Multiplicação matricial quantizada
        for (int j = 0; j < 8; j++) {
            sum += (int32_t)m_lstmState.hiddenState[j] * m_weights.forgetW[i][j];
        }
        for (int j = 0; j < 12; j++) {
            sum += (int32_t)inputs[j] * m_weights.forgetU[i][j];
        }
        sum += (int32_t)m_weights.forgetBias[i] << 12; // Scale bias
        
        // Sigmoid quantizado usando lookup table
        forgetGate[i] = quantizedSigmoid(sum >> 12);
    }
    
    // Input gate: i_t = sigmoid(W_i * [h_{t-1}, x_t] + b_i)
    int32_t inputGate[8];
    for (int i = 0; i < 8; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 8; j++) {
            sum += (int32_t)m_lstmState.hiddenState[j] * m_weights.inputW[i][j];
        }
        for (int j = 0; j < 12; j++) {
            sum += (int32_t)inputs[j] * m_weights.inputU[i][j];
        }
        sum += (int32_t)m_weights.inputBias[i] << 12;
        
        inputGate[i] = quantizedSigmoid(sum >> 12);
    }
    
    // Candidate values: C_tilde_t = tanh(W_C * [h_{t-1}, x_t] + b_C)
    int32_t candidateValues[8];
    for (int i = 0; i < 8; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 8; j++) {
            sum += (int32_t)m_lstmState.hiddenState[j] * m_weights.candidateW[i][j];
        }
        for (int j = 0; j < 12; j++) {
            sum += (int32_t)inputs[j] * m_weights.candidateU[i][j];
        }
        sum += (int32_t)m_weights.candidateBias[i] << 12;
        
        candidateValues[i] = quantizedTanh(sum >> 12);
    }
    
    // Update cell state: C_t = f_t * C_{t-1} + i_t * C_tilde_t
    for (int i = 0; i < 8; i++) {
        int32_t forgetTerm = ((int32_t)forgetGate[i] * m_lstmState.cellState[i]) >> 15;
        int32_t inputTerm = ((int32_t)inputGate[i] * candidateValues[i]) >> 15;
        m_lstmState.cellState[i] = (int16_t)clampI(-32768, forgetTerm + inputTerm, 32767);
    }
    
    // Output gate: o_t = sigmoid(W_o * [h_{t-1}, x_t] + b_o)
    int32_t outputGate[8];
    for (int i = 0; i < 8; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 8; j++) {
            sum += (int32_t)m_lstmState.hiddenState[j] * m_weights.outputW[i][j];
        }
        for (int j = 0; j < 12; j++) {
            sum += (int32_t)inputs[j] * m_weights.outputU[i][j];
        }
        sum += (int32_t)m_weights.outputBias[i] << 12;
        
        outputGate[i] = quantizedSigmoid(sum >> 12);
    }
    
    // Update hidden state: h_t = o_t * tanh(C_t)
    for (int i = 0; i < 8; i++) {
        int32_t cellTanh = quantizedTanh(m_lstmState.cellState[i]);
        int32_t hiddenNew = ((int32_t)outputGate[i] * cellTanh) >> 15;
        m_lstmState.hiddenState[i] = (int16_t)clampI(-32768, hiddenNew, 32767);
    }
    
    // Output layer: y = W_out * h_t + b_out
    for (int i = 0; i < 4; i++) {
        int32_t sum = 0;
        for (int j = 0; j < 8; j++) {
            sum += (int32_t)m_lstmState.hiddenState[j] * m_weights.outputLayerW[i][j];
        }
        sum += (int32_t)m_weights.outputLayerBias[i] << 12;
        outputs[i] = (int16_t)clampI(-32768, sum >> 12, 32767);
    }
}
```

## 5. Integração com Sistemas Existentes

### 5.1. Integração com Adaptive Wall Wetting

```cpp
void NeuralFuelController::integrateWithAdaptiveWW(AdaptiveWallWettingController* aww) {
    m_adaptiveWW = aww;
    
    // Se a rede neural está confiante, usa suas predições
    if (m_telemetry.currentConfidence > m_config.confidenceThreshold) {
        // Aplicar predições neurais às correções AWW
        float neuralBetaCorrection = m_telemetry.betaPrediction;
        float neuralTauCorrection = m_telemetry.tauPrediction;
        
        // Combinar com correções clássicas (weighted average)
        float weight = m_telemetry.currentConfidence;
        
        // AWW aplicará a combinação neural + clássica
        aww->applyNeuralAssist(neuralBetaCorrection, neuralTauCorrection, weight);
    }
    
    // Sempre usar dados do AWW para aprendizado neural
    if (aww->isActive()) {
        m_temporalBuffer.awwState = aww->getState();
        m_temporalBuffer.awwTransientMagnitude = aww->getTransientMagnitude();
    }
}
```

### 5.2. Integração com LTFT

```cpp
void NeuralFuelController::integrateWithLTFT(LongTermFuelTrim* ltft) {
    m_ltft = ltft;
    
    // Usar dados históricos do LTFT para treinar a rede neural
    float currentLTFT = ltft->getLtft(getFuelingLoad(), Sensor::getOrZero(SensorType::Rpm));
    
    // Se LTFT está convergindo muito lentamente, assistir com predições neurais
    if (m_telemetry.currentConfidence > 0.8f) {
        float ltftAssist = m_telemetry.betaPrediction * 0.1f; // Assistência suave
        ltft->applyNeuralAssist(ltftAssist);
    }
}
```

## 6. Telemetria e Diagnósticos

### 6.1. Live Data Structure

```cpp
struct neural_fuel_live_data_s {
    // Inputs atuais
    float lambdaErrorT1 = 0;
    float lambdaErrorT2 = 0;
    float lambdaErrorT3 = 0;
    float lambdaErrorT4 = 0;
    float lambdaErrorT5 = 0;
    float lambdaSlopeShort = 0;
    float lambdaSlopeLong = 0;
    float lambdaVariance = 0;
    float lambdaTrend = 0;
    float loadNormalized = 0;
    float rpmNormalized = 0;
    float transientMagnitude = 0;
    
    // Outputs atuais
    float betaPrediction = 0;
    float tauPrediction = 0;
    float confidenceScore = 0;
    float learningRate = 0;
    
    // Estados internos
    float hiddenState0 = 0;
    float hiddenState1 = 0;
    float hiddenState2 = 0;
    float hiddenState3 = 0;
    
    // Estatísticas
    uint32_t totalInferences = 0;
    uint32_t successfulPredictions = 0;
    float successRate = 0;
    
    // Flags de status
    bool isEnabled = false;
    bool isActive = false;
    bool fallbackMode = false;
    bool bufferReady = false;
};
```

## 7. Failsafe e Robustez

### 7.1. Condições de Fallback

```cpp
bool NeuralFuelController::checkOperationalConditions() {
    // Verificar se sensores estão funcionando
    if (!Sensor::get(SensorType::Lambda1).Valid) {
        m_telemetry.fallbackMode = true;
        return false;
    }
    
    // Verificar se a confiança está muito baixa
    if (m_telemetry.currentConfidence < 0.3f) {
        m_telemetry.fallbackMode = true;
        return false;
    }
    
    // Verificar se há dados suficientes no buffer
    if (!m_temporalBuffer.bufferFull) {
        return false;
    }
    
    // Verificar se os sistemas clássicos estão funcionando
    if (m_adaptiveWW && !m_adaptiveWW->isOperational()) {
        m_telemetry.fallbackMode = true;
        return false;
    }
    
    return true;
}
```

### 7.2. Estratégia de Recovery

```cpp
void NeuralFuelController::handleFailure() {
    // Reset do estado neural
    resetNeuralState();
    
    // Desabilitar temporariamente
    m_config.enabled = false;
    
    // Registrar falha para diagnóstico
    m_telemetry.fallbackMode = true;
    
    // Tentar recovery após 30 segundos
    m_recoveryTimer.reset();
}

void NeuralFuelController::attemptRecovery() {
    if (m_recoveryTimer.hasElapsedSec(30.0f)) {
        // Verificar se condições melhoraram
        if (checkBasicOperationalConditions()) {
            m_config.enabled = true;
            m_telemetry.fallbackMode = false;
            resetNeuralState();
        }
    }
}
```

## 8. Configuração e Calibração

### 8.1. Parâmetros TunerStudio

```cpp
// Configuração básica
bool neuralFuelEnabled = false;
float neuralFuelConfidenceThreshold = 0.7f;
float neuralFuelLearningRate = 0.001f;
uint16_t neuralFuelUpdateInterval = 20; // ms

// Pesos da rede (só para visualização/diagnóstico)
float neuralFuelWeights[200]; // Somente leitura
float neuralFuelBiases[20];   // Somente leitura

// Estatísticas
uint32_t neuralFuelInferenceCount = 0;
float neuralFuelSuccessRate = 0;
float neuralFuelCurrentConfidence = 0;
```

### 8.2. Procedimento de Calibração

1. **Treinamento Offline**: Usar dados coletados em dinamômetro
2. **Quantização**: Converter pesos float32 para int16 
3. **Validação**: Testar em simulador antes de deployment
4. **Ajuste de Ganhos**: Calibrar thresholds de confiança
5. **Integração Gradual**: Começar com baixo peso neural, aumentar gradualmente

## 9. Cronograma de Implementação

### Fase 1: Infraestrutura Base (3 dias)
- [ ] Criar estruturas de dados básicas
- [ ] Implementar buffer temporal
- [ ] Integrar com EngineModule pattern
- [ ] Adicionar telemetria básica

### Fase 2: Rede Neural Core (4 dias)
- [ ] Implementar LSTM quantizado
- [ ] Adicionar funções de ativação
- [ ] Implementar forward pass
- [ ] Testes unitários da rede

### Fase 3: Integração de Dados (2 dias)
- [ ] Implementar coleta temporal
- [ ] Adicionar análise de slope
- [ ] Integrar com AWW/LTFT
- [ ] Validar inputs/outputs

### Fase 4: Aplicação e Failsafe (3 dias)
- [ ] Implementar aplicação de predições
- [ ] Adicionar sistema de fallback
- [ ] Implementar recovery automático
- [ ] Testes de robustez

### Fase 5: Telemetria e TunerStudio (2 dias)
- [ ] Implementar live data completa
- [ ] Adicionar interface TunerStudio
- [ ] Criar telas de diagnóstico
- [ ] Documentação de uso

**Total: 14 dias de implementação**

## 10. Métricas de Sucesso

- **Precisão**: >85% de predições corretas em condições normais
- **Performance**: <50µs por inferência, <5% de CPU usage
- **Robustez**: <1% de falhas em 1000 horas de operação
- **Integração**: Melhoria mensurável em AFR stability vs. sistemas clássicos
- **Eficiência**: Redução de 20% no tempo de convergência LTFT/AWW

Esta especificação fornece a base completa para implementar um sistema de rede neural robusto e eficiente que complementa os sistemas existentes do RusEFI.