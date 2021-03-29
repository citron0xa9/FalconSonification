#pragma once

#include <JuceHeader.h>

#include "MainSonificationProcessor.h"

namespace falconSound {

class ParameterComponent : public juce::Component {
public:
    explicit ParameterComponent(MainSonificationProcessor& sonificationProcessor);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void onSonificationProcessorInitialized();
private:
    using expression_element_t = std::pair<juce::Label, juce::TextEditor>;
    using set_expression_func_t = void (MainSonificationProcessor::*)(const std::string&);
    using get_expression_func_t = const std::string& (MainSonificationProcessor::*)() const;

    struct Checkbox {
        using set_enable_func = void (MainSonificationProcessor::*)(bool);
        using get_is_enabled_func = bool (MainSonificationProcessor::*)() const;

        juce::Label mLabel;
        juce::ToggleButton mButton;

        set_enable_func mEnableFunc;
        get_is_enabled_func mIsEnabledFunc;
    };

    MainSonificationProcessor& mSonificationProcessor;

    expression_element_t mEngineCloseGain;
    expression_element_t mEngineDistantGain;
    expression_element_t mEngineModulatorFrequency;
    expression_element_t mEngineModulatorGain;
    expression_element_t mEngineAllGain;
    expression_element_t mAirodynamicPressureLowGain;
    expression_element_t mAirodynamicPressureHighGain;
    expression_element_t mLowpassCutoffFrequency;

    std::array<expression_element_t*, 8> mExpressionElementPtrs;

    Checkbox mEnableEngineSound;
    Checkbox mEnableEngineModulation;
    Checkbox mEnableSonicBoom;
    Checkbox mEnableLowpass;
    Checkbox mEnableAirodynamicPressureSound;

    std::array<Checkbox*, 5> mCheckboxPtrs;

    juce::Slider mMasterGainSlider;
    juce::Label mMasterGainLabel;

    void connectTextEditorChange(juce::TextEditor& editor, set_expression_func_t setExpressionFunc, get_expression_func_t getExpressionFunc);
};

}