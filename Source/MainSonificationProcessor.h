#pragma once

#include <JuceHeader.h>

#include "FalconSimulation.h"
#include "SampledSound.h"

#include "exprtk.h"

namespace falconSound {

class MainSonificationProcessor : public juce::PositionableAudioSource {
public:
    explicit MainSonificationProcessor();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void falconSimulation(FalconSimulation* simulationPtr);

    void engineCloseSoundGainExpression(const std::string& expressionString);
    const std::string& engineCloseSoundGainExpression() const noexcept;
    void engineDistantSoundGainExpression(const std::string& expressionString);
    const std::string& engineDistantSoundGainExpression() const noexcept;
    void engineAllSoundGainExpression(const std::string& expressionString);
    const std::string& engineAllSoundGainExpression() const noexcept;

    void engineSoundModulatorFrequencyExpression(const std::string& expressionString);
    const std::string& engineSoundModulatorFrequencyExpression() const noexcept;
    void engineSoundModulatorGainExpression(const std::string& expressionString);
    const std::string& engineSoundModulatorGainExpression() const noexcept;

    void airodynamicPressureLowGainExpression(const std::string& expressionString);
    const std::string& airodynamicPressureLowGainExpression() const noexcept;
    void airodynamicPressureHighGainExpression(const std::string& expressionString);
    const std::string& airodynamicPressureHighGainExpression() const noexcept;

    void lowpassCutoffFrequencyExpression(const std::string& expressionString);
    const std::string& lowpassCutoffFrequencyExpression() const noexcept;

    void enableEngineSound(bool shouldEnable);
    bool isEngineSoundEnabled() const noexcept;
    void enableEngineModulation(bool shouldEnable);
    bool isEngineModulationEnabled() const noexcept;
    void enableSonicBoom(bool shouldEnable);
    bool isSonicBoomEnabled() const noexcept;
    void enableLowpass(bool shouldEnable);
    bool isLowpassEnabled() const noexcept;
    void enableAirodynamicPressureSound(bool shouldEnable);
    bool isAirodynamicPressureSoundEnabled() const noexcept;

    void masterGain(float gain);
    float masterGain() const noexcept;

    float currentSonificationTimeInSeconds() const;

    void onPositionChangedExternally();

private:
    FalconSimulation* mSimulationPtr = nullptr;

    float mSonificationTimeSeconds = 0.0f;
    juce::int64 mCurrentSamplePosition = 0;
    juce::int64 mLengthInSamples = 0;
    bool mIsLooping = false;
    double mSampleRate;
    int mSamplesPerBlockExpected;
    bool mAudioIsInitialized = false;

    float mLastMachNumber;

    juce::AudioFormatManager mAudioFormatManager;

    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> mPressureLow;
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> mPressureHigh;
    juce::dsp::StateVariableTPTFilter<float> mLowPass;
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> mEngineSoundModulator;
    std::unique_ptr<SampledSound> mEngineCloseSoundPtr;
    std::unique_ptr<SampledSound> mEngineDistantSoundPtr;
    std::unique_ptr<SampledSound> mSonicBoomSoundPtr;

    juce::AudioBuffer<float> mTemporaryAudioBuffer;

    std::string mEngineCloseSoundGainExpressionStr;
    std::string mEngineDistantSoundGainExpressionStr;
    std::string mEngineAllSoundGainExpressionStr;
    std::string mEngineSoundModulatorFrequencyExpressionStr;
    std::string mEngineSoundModulatorGainExpressionStr;
    std::string mAirodynamicPressureLowGainExpressionStr;
    std::string mAirodynamicPressureHighGainExpressionStr;
    std::string mLowpassCutoffFrequencyExpressionStr;
    exprtk::expression<float> mEngineCloseSoundGainExpression;
    exprtk::expression<float> mEngineDistantSoundGainExpression;
    exprtk::expression<float> mEngineAllSoundGainExpression;
    exprtk::expression<float> mEngineSoundModulatorFrequencyExpression;
    exprtk::expression<float> mEngineSoundModulatorGainExpression;
    exprtk::expression<float> mAirodynamicPressureLowGainExpression;
    exprtk::expression<float> mAirodynamicPressureHighGainExpression;
    exprtk::expression<float> mLowpassCutoffFrequencyExpression;

    bool mEnableEngineSound = true;
    bool mEnableEngineModulation = true;
    bool mEnableSonicBoom = true;
    bool mEnableLowpass = true;
    bool mEnableAirodynamicPressureSound = true;

    float mMasterGain = 1.0f;

    exprtk::symbol_table<float> mExpressionSymbolTable;
    std::unique_ptr<exprtk::parser<float>> mExpressionParserPtr;

    void updateSonificationParameters(float sonificationTimeSeconds);
    
    static void addToMix(const juce::AudioSourceChannelInfo& mixAudioInfo, const juce::AudioSourceChannelInfo& partialAudioInfo);
    static void modulateMix(const juce::AudioSourceChannelInfo& mixAudioInfo, const juce::AudioSourceChannelInfo& modulateAudioInfo);

    void compileExpression(exprtk::expression<float>& expression, const std::string& expressionString);
    static std::unique_ptr<exprtk::parser<float>> createExpressionParser();
};

}