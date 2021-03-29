#include "FalconSonificationApplication.h"

namespace falconSound {

const juce::String FalconSonificationApplication::getApplicationName()
{
    return "Falcon Sonification App";
}

const juce::String FalconSonificationApplication::getApplicationVersion()
{
    return "1.0.0";
}

bool FalconSonificationApplication::moreThanOneInstanceAllowed()
{
    return true;
}

void FalconSonificationApplication::initialise(const juce::String& commandLine)
{
    mMainWindow = std::make_unique<MainWindow>(*this, getApplicationName());
}

void FalconSonificationApplication::shutdown()
{
    mMainWindow.reset();
}

void FalconSonificationApplication::systemRequestedQuit()
{
    quit();
}

void FalconSonificationApplication::anotherInstanceStarted(const juce::String& commandLine)
{
}

}
