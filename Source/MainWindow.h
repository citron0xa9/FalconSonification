#pragma once

#include <JuceHeader.h>

namespace falconSound {

class FalconSonificationApplication;

class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow(FalconSonificationApplication& application, juce::String name);
    void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

}