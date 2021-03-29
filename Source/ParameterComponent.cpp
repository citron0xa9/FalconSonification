#include "ParameterComponent.h"

namespace falconSound {

ParameterComponent::ParameterComponent(MainSonificationProcessor& sonificationProcessor)
    : mSonificationProcessor{ sonificationProcessor },
    mExpressionElementPtrs{
        &mEngineCloseGain,
        &mEngineDistantGain,
        &mEngineModulatorFrequency,
        &mEngineModulatorGain,
        &mEngineAllGain,
        &mAirodynamicPressureLowGain,
        &mAirodynamicPressureHighGain,
        &mLowpassCutoffFrequency},
    mCheckboxPtrs{
        &mEnableEngineSound,
        &mEnableEngineModulation,
        &mEnableSonicBoom,
        &mEnableLowpass,
        &mEnableAirodynamicPressureSound}
{
    mEngineCloseGain.first.setText("Engine Sound Close Gain", juce::NotificationType::dontSendNotification);
    mEngineDistantGain.first.setText("Engine Sound Distant Gain", juce::NotificationType::dontSendNotification);
    mEngineModulatorFrequency.first.setText("Engine Modulator Frequency", juce::NotificationType::dontSendNotification);;
    mEngineModulatorGain.first.setText("Engine Modulator Gain", juce::NotificationType::dontSendNotification);
    mEngineAllGain.first.setText("Engine Sound All Gain", juce::NotificationType::dontSendNotification);
    mAirodynamicPressureLowGain.first.setText("Airodynamic Pressure Low Frequencies Gain", juce::NotificationType::dontSendNotification);
    mAirodynamicPressureHighGain.first.setText("Airodynamic Pressure High Frequencies Gain", juce::NotificationType::dontSendNotification);
    mLowpassCutoffFrequency.first.setText("Engine Sound Lowpass Frequency", juce::NotificationType::dontSendNotification);

    connectTextEditorChange(mEngineCloseGain.second, &MainSonificationProcessor::engineCloseSoundGainExpression, &MainSonificationProcessor::engineCloseSoundGainExpression);
    connectTextEditorChange(mEngineDistantGain.second, &MainSonificationProcessor::engineDistantSoundGainExpression, &MainSonificationProcessor::engineDistantSoundGainExpression);
    connectTextEditorChange(mEngineModulatorFrequency.second, &MainSonificationProcessor::engineSoundModulatorFrequencyExpression, &MainSonificationProcessor::engineSoundModulatorFrequencyExpression);
    connectTextEditorChange(mEngineModulatorGain.second, &MainSonificationProcessor::engineSoundModulatorGainExpression, &MainSonificationProcessor::engineSoundModulatorGainExpression);
    connectTextEditorChange(mEngineAllGain.second, &MainSonificationProcessor::engineAllSoundGainExpression, &MainSonificationProcessor::engineAllSoundGainExpression);
    connectTextEditorChange(mAirodynamicPressureLowGain.second, &MainSonificationProcessor::airodynamicPressureLowGainExpression, &MainSonificationProcessor::airodynamicPressureLowGainExpression);
    connectTextEditorChange(mAirodynamicPressureHighGain.second, &MainSonificationProcessor::airodynamicPressureHighGainExpression, &MainSonificationProcessor::airodynamicPressureHighGainExpression);
    connectTextEditorChange(mLowpassCutoffFrequency.second, &MainSonificationProcessor::lowpassCutoffFrequencyExpression, &MainSonificationProcessor::lowpassCutoffFrequencyExpression);

    for (auto* expressionElementPtr : mExpressionElementPtrs) {
        expressionElementPtr->second.setReadOnly(true);
        expressionElementPtr->second.setMultiLine(false);
        addAndMakeVisible(expressionElementPtr->first);
        addAndMakeVisible(expressionElementPtr->second);
    }

    mEnableEngineSound.mLabel.setText("Enable Engine Sound", juce::NotificationType::dontSendNotification);
    mEnableEngineSound.mEnableFunc = &MainSonificationProcessor::enableEngineSound;
    mEnableEngineSound.mIsEnabledFunc = &MainSonificationProcessor::isEngineSoundEnabled;

    mEnableEngineModulation.mLabel.setText("Enable Engine Sound Modulation", juce::NotificationType::dontSendNotification);
    mEnableEngineModulation.mEnableFunc = &MainSonificationProcessor::enableEngineModulation;
    mEnableEngineModulation.mIsEnabledFunc = &MainSonificationProcessor::isEngineModulationEnabled;

    mEnableSonicBoom.mLabel.setText("Enable Sonic Boom", juce::NotificationType::dontSendNotification);
    mEnableSonicBoom.mEnableFunc = &MainSonificationProcessor::enableSonicBoom;
    mEnableSonicBoom.mIsEnabledFunc = &MainSonificationProcessor::isSonicBoomEnabled;

    mEnableLowpass.mLabel.setText("Enable Engine Sound Lowpass", juce::NotificationType::dontSendNotification);
    mEnableLowpass.mEnableFunc = &MainSonificationProcessor::enableLowpass;
    mEnableLowpass.mIsEnabledFunc = &MainSonificationProcessor::isLowpassEnabled;

    mEnableAirodynamicPressureSound.mLabel.setText("Enable Airodynamic Pressure Sound", juce::NotificationType::dontSendNotification);
    mEnableAirodynamicPressureSound.mEnableFunc = &MainSonificationProcessor::enableAirodynamicPressureSound;
    mEnableAirodynamicPressureSound.mIsEnabledFunc = &MainSonificationProcessor::isAirodynamicPressureSoundEnabled;

    for (auto* checkboxPtr : mCheckboxPtrs) {
        checkboxPtr->mButton.setClickingTogglesState(true);
        checkboxPtr->mButton.setToggleState((mSonificationProcessor.*checkboxPtr->mIsEnabledFunc)(), juce::NotificationType::dontSendNotification);
        checkboxPtr->mButton.onClick = [checkboxPtr, this]() {
            (mSonificationProcessor.*checkboxPtr->mEnableFunc)(checkboxPtr->mButton.getToggleState());
        };
        addAndMakeVisible(checkboxPtr->mLabel);
        addAndMakeVisible(checkboxPtr->mButton);
    }

    mMasterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mMasterGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, mMasterGainSlider.getTextBoxWidth(), mMasterGainSlider.getTextBoxHeight());
    mMasterGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    mMasterGainSlider.setRange(0.0, 3.0);
    mMasterGainSlider.setValue(mSonificationProcessor.masterGain());
    mMasterGainSlider.onValueChange = [this]() {
        mSonificationProcessor.masterGain(mMasterGainSlider.getValue());
    };
    addAndMakeVisible(mMasterGainSlider);
    mMasterGainLabel.setText("Master Gain", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(mMasterGainLabel);
}

void ParameterComponent::onSonificationProcessorInitialized()
{
    for (auto* expressionElementPtr : mExpressionElementPtrs) {
        expressionElementPtr->second.setReadOnly(false);
    }

    mEngineCloseGain.second.setText(mSonificationProcessor.engineCloseSoundGainExpression(), false);
    mEngineDistantGain.second.setText(mSonificationProcessor.engineDistantSoundGainExpression(), false);
    mEngineModulatorFrequency.second.setText(mSonificationProcessor.engineSoundModulatorFrequencyExpression(), false);
    mEngineModulatorGain.second.setText(mSonificationProcessor.engineSoundModulatorGainExpression(), false);
    mEngineAllGain.second.setText(mSonificationProcessor.engineAllSoundGainExpression(), false);
    mAirodynamicPressureLowGain.second.setText(mSonificationProcessor.airodynamicPressureLowGainExpression(), false);
    mAirodynamicPressureHighGain.second.setText(mSonificationProcessor.airodynamicPressureHighGainExpression(), false);
    mLowpassCutoffFrequency.second.setText(mSonificationProcessor.lowpassCutoffFrequencyExpression(), false);
}

void ParameterComponent::paint(juce::Graphics& g)
{
}

void ParameterComponent::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    juce::Grid gridLayout;
    gridLayout.templateRows = {
        //Expressions
        Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}}, Track{Fr{2}},
        //Seperation
        Track{ Fr{ 1 } },
        //Checkboxes
        Track{ Fr{ 2 } }, Track{ Fr{ 2 } }, Track{ Fr{ 2 } }, Track{ Fr{ 2 } }, Track{ Fr{ 2 } },
        //Seperation
        Track{ Fr{ 1 } },
        //Master Gain
        Track{Fr{1}},
        //Seperation
        Track{Fr{1}} };

    gridLayout.templateColumns = { Track{Fr{1}}, Track{Fr{1}} };

    for (auto* expressionElementPtr : mExpressionElementPtrs) {
        gridLayout.items.add(juce::GridItem{ expressionElementPtr->first });
        gridLayout.items.add(juce::GridItem{ expressionElementPtr->second });
    }

    int currentRow = 10;
    for (auto* checkboxPtr : mCheckboxPtrs) {
        juce::GridItem checkboxLabelItem{ checkboxPtr->mLabel };
        checkboxLabelItem.row.start = currentRow;
        checkboxLabelItem.column.start = 1;
        gridLayout.items.add(checkboxLabelItem);

        juce::GridItem checkboxButtonItem{ checkboxPtr->mButton };
        checkboxButtonItem.row.start = currentRow;
        checkboxButtonItem.column.start = 2;
        gridLayout.items.add(checkboxButtonItem);

        ++currentRow;
    }

    juce::GridItem sliderItem{ mMasterGainSlider };
    sliderItem.row.start = 16;
    sliderItem.column.start = 1;
    sliderItem.column.end = juce::GridItem::Span{ 2 };
    gridLayout.items.add(sliderItem);
    gridLayout.performLayout(getLocalBounds());

    auto sliderBounds = mMasterGainSlider.getBounds();
    int sliderLabelWidth = std::ceil(mMasterGainLabel.getLookAndFeel().getLabelFont(mMasterGainLabel).getStringWidthFloat(mMasterGainLabel.getText()));
    sliderLabelWidth += mMasterGainLabel.getBorderSize().getLeftAndRight();

    mMasterGainLabel.setBounds(sliderBounds.removeFromLeft(sliderLabelWidth));
    mMasterGainSlider.setBounds(sliderBounds);
}

void ParameterComponent::connectTextEditorChange(juce::TextEditor& editor, set_expression_func_t setExpressionFunc, get_expression_func_t getExpressionFunc)
{
    editor.onReturnKey = [this, &editor, setExpressionFunc, getExpressionFunc]() {
        try {
            (mSonificationProcessor.*setExpressionFunc)(editor.getText().toStdString());
        }
        catch (const std::exception& ex) {
            juce::NativeMessageBox::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "Error", std::string{ "Failed changing expression:\n" } + ex.what());
            editor.setText((mSonificationProcessor.*getExpressionFunc)(), false);
        }
    };
    editor.onFocusLost = editor.onEscapeKey = [this, &editor, getExpressionFunc]() {
        editor.setText((mSonificationProcessor.*getExpressionFunc)(), false);
    };
}

}