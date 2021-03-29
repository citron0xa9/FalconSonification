#pragma once

#include <JuceHeader.h>
#include "FalconSimulation.h"
#include "MainSonificationProcessor.h"
#include "ParameterComponent.h"
#include "SonificationState.h"

namespace falconSound {

class FalconSonificationApplication;

class MainComponent : public juce::AudioAppComponent, public juce::FilenameComponentListener, public juce::ChangeListener
{
    friend class StartupSonificationState;
    friend class AudioReadySonificationState;
    friend class ReadySonificationState;
    friend class StartPlayingSonificationState;
    friend class PlayingSonificationState;
    friend class StopPlayingSonificationState;
    friend class PausePlayingSonificationState;
    friend class PausedPlayingSonificationState;
    friend class ShutdownSonificationState;
public:
    static const inline int RELAYOUT_COMMAND_ID = 1;

    explicit MainComponent(FalconSonificationApplication& application);
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void handleCommandMessage(int commandId) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void filenameComponentChanged(juce::FilenameComponent* componentThatChanged) override;

private:

    class UpdateComponentsTimer : public juce::Timer {
    public:
        UpdateComponentsTimer(MainComponent& component);
        void timerCallback() override;

    private:
        MainComponent& mMainComponent;
    };

    FalconSonificationApplication& mApplication;
    UpdateComponentsTimer mUpdateComponentsTimer;
    std::unique_ptr<ParameterComponent> mParameterComponent;
    std::unique_ptr<juce::FilenameComponent> mFlightLogFilenameComponent;
    std::unique_ptr<juce::TextButton> mStartSonificationButton;
    std::unique_ptr<juce::TextButton> mPauseSonificationButton;
    std::unique_ptr<juce::TextButton> mStopSonificationButton;
    std::unique_ptr<juce::Label> mStatusLabel;
    std::unique_ptr<juce::TextButton> mDeviceSettingsButton;
    std::unique_ptr<juce::Slider> mTimeSlider;
    std::unique_ptr<FalconSimulation> mSimulationPtr;
    MainSonificationProcessor mSonificationProcessor;
    juce::AudioTransportSource mSonificationAudioTransportSource;

    std::unique_ptr<SonificationState> mSonificationStatePtr;
    bool mIsSonificationRunning = false;
    double mSampleRate = 0.0;
    int mSamplesPerBlockExpected = 0;

    void createFlightlogFilenameComponent();
    void loadFlightLog();

    void changeSonificationState(std::unique_ptr<SonificationState>&& newState);

    void updateStatusLabel();
    void updateTimeSlider();
    void changeSimulationTime(float timeInSeconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

}