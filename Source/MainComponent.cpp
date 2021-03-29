#include "MainComponent.h"

#include "FalconSonificationApplication.h"
#include "StartupSonificationState.h"
#include "ShutdownSonificationState.h"
#include "AudioReadySonificationState.h"

namespace falconSound {

MainComponent::MainComponent(FalconSonificationApplication& application)
    : mApplication{application},
    mUpdateComponentsTimer{*this}
{
    mSonificationAudioTransportSource.setSource(&mSonificationProcessor);
    mSonificationAudioTransportSource.addChangeListener(this);

    mParameterComponent = std::make_unique<ParameterComponent>(mSonificationProcessor);
    addAndMakeVisible(*mParameterComponent);
    createFlightlogFilenameComponent();

    mStartSonificationButton = std::make_unique<juce::TextButton>("Start", "Start Sonification");
    mStartSonificationButton->onClick = [this]() {
        mSonificationStatePtr->handleStartClicked();
    };
    addAndMakeVisible(*mStartSonificationButton);

    mPauseSonificationButton = std::make_unique<juce::TextButton>("Pause", "Pause Sonification");
    mPauseSonificationButton->onClick = [this]() {
        mSonificationStatePtr->handlePauseClicked();
    };
    addAndMakeVisible(*mPauseSonificationButton);

    mStopSonificationButton = std::make_unique<juce::TextButton>("Stop", "Stop Sonification");
    mStopSonificationButton->onClick = [this]() {
        mSonificationStatePtr->handleStopClicked();
    };
    addAndMakeVisible(*mStopSonificationButton);

    mTimeSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextBoxAbove);
    mTimeSlider->onValueChange = [this]() {
        changeSimulationTime(mTimeSlider->getValue());
    };
    mTimeSlider->setRange(0, 60, 0.1);
    mTimeSlider->setTextValueSuffix(" seconds");
    addAndMakeVisible(*mTimeSlider);

    mStatusLabel = std::make_unique<juce::Label>("StatusLabel");
    mStatusLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*mStatusLabel);

    mDeviceSettingsButton = std::make_unique<juce::TextButton>("Device Settings", "Open Device Settings");
    mDeviceSettingsButton->onClick = [this]() {
        auto deviceSelectorComponent = new juce::AudioDeviceSelectorComponent{ deviceManager, 0, 0, 1, 2, false, false, true, false };
        deviceSelectorComponent->setSize(800, 600);
        juce::DialogWindow::LaunchOptions dialogOptions;
        dialogOptions.dialogTitle = "Device Settings";
        dialogOptions.content = juce::OptionalScopedPointer<juce::Component>{ deviceSelectorComponent, true };
        dialogOptions.componentToCentreAround = this;
        dialogOptions.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
        dialogOptions.escapeKeyTriggersCloseButton = false;
        dialogOptions.resizable = true;
        dialogOptions.useBottomRightCornerResizer = false;
        dialogOptions.launchAsync();
    };
    addAndMakeVisible(*mDeviceSettingsButton);

    changeSonificationState(std::make_unique<StartupSonificationState>(*this));
    updateStatusLabel();

    mUpdateComponentsTimer.startTimer(30);

    setSize(800, 600);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    mUpdateComponentsTimer.stopTimer();
    changeSonificationState(std::make_unique<ShutdownSonificationState>(*this));
    updateStatusLabel();
    shutdownAudio();
}


void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mSampleRate = sampleRate;
    mSamplesPerBlockExpected = samplesPerBlockExpected;
    mSonificationAudioTransportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    changeSonificationState(std::make_unique<AudioReadySonificationState>(*this));
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mSonificationAudioTransportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mSonificationAudioTransportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    juce::Grid gridLayout;
    gridLayout.templateRows = { Track{ Fr{ 9 } }, Track{Fr{3}}, Track{Fr{3}}, Track{ Fr{ 2 } }, Track{Fr{1}} };
    gridLayout.templateColumns = { Track{Fr{1}}, Track{Fr{1}} , Track{Fr{1}} };

    juce::GridItem parametersItem{ *mParameterComponent };
    parametersItem.column.start = 1;
    parametersItem.column.end = juce::GridItem::Span{ 3 };
    parametersItem.row.start = 1;

    juce::GridItem timeSliderItem{ *mTimeSlider };
    timeSliderItem.column.start = 1;
    timeSliderItem.column.end = juce::GridItem::Span{ 3 };
    timeSliderItem.row.start = 3;
    timeSliderItem.margin.top = 4;

    juce::GridItem flightLogItem{ *mFlightLogFilenameComponent };
    flightLogItem.column.start = 1;
    flightLogItem.column.end = juce::GridItem::Span{ 3 };
    flightLogItem.row.start = 4;

    juce::GridItem statusLabelItem{ *mStatusLabel };
    statusLabelItem.column.start = 1;
    statusLabelItem.column.end = juce::GridItem::Span{ 2 };
    statusLabelItem.row.start = 5;

    juce::GridItem deviceSettingsItem{ *mDeviceSettingsButton };
    deviceSettingsItem.column.start = 3;
    deviceSettingsItem.row.start = 5;

    gridLayout.items = { parametersItem, juce::GridItem{*mStartSonificationButton}, juce::GridItem{*mPauseSonificationButton}, juce::GridItem{*mStopSonificationButton},
        timeSliderItem, flightLogItem, statusLabelItem, deviceSettingsItem };
    gridLayout.performLayout(getLocalBounds());
}

void MainComponent::handleCommandMessage(const int commandId) {
    if (commandId == RELAYOUT_COMMAND_ID) {
        resized();
        repaint();
    }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source != &mSonificationAudioTransportSource) {
        return;
    }

    mSonificationStatePtr->handleSonificationAudioSourceChanged();
}

void MainComponent::filenameComponentChanged(juce::FilenameComponent* componentThatChanged)
{
    mSonificationStatePtr->handleFlightlogFilenameComponentChanged();
    updateStatusLabel();
}

void MainComponent::createFlightlogFilenameComponent()
{
    mFlightLogFilenameComponent = std::make_unique<juce::FilenameComponent>(
        "flightLogFilename",
        juce::File{},
        false,
        false,
        false,
        "*.csv",
        juce::String{},
        "Select flight log");
    mFlightLogFilenameComponent->addListener(this);
    addAndMakeVisible(*mFlightLogFilenameComponent);
}

void MainComponent::loadFlightLog()
{
    try {
        mSimulationPtr = std::make_unique<FalconSimulation>(mFlightLogFilenameComponent->getCurrentFile());
        mSonificationProcessor.falconSimulation(mSimulationPtr.get());
        mParameterComponent->onSonificationProcessorInitialized();
        mTimeSlider->setRange(0, mSonificationAudioTransportSource.getLengthInSeconds(), 0.1);
    } catch (const std::exception& ex) {
        juce::NativeMessageBox::showMessageBoxAsync(juce::AlertWindow::AlertIconType::WarningIcon, "Error", juce::String{ "Loading flightlog failed: " } +ex.what(), this);
    }
}

void MainComponent::changeSonificationState(std::unique_ptr<SonificationState>&& newState)
{
    if (mSonificationStatePtr && mSonificationStatePtr->stateId() == newState->stateId()) {
        return;
    }
    assert((!mSonificationStatePtr && newState->stateId() == SonificationState::StateId::STARTUP) || newState->canTransitionFromState(*mSonificationStatePtr));
    mSonificationStatePtr = std::move(newState);
    mSonificationStatePtr->enter();
}

void MainComponent::updateStatusLabel()
{
    juce::String status{"Status: "};
    status = status + mSonificationStatePtr->name();
    if (!mSimulationPtr) {
        status += " No flightlog loaded";
    }
    else {
        status += " Flightlog loaded";
    }

    status += " Sonification Time: " + juce::String{ mSonificationAudioTransportSource.getCurrentPosition() }
        + " --- Sample Rate: " + juce::String{ mSampleRate } + " Samples Per Block Expected: " + juce::String{ mSamplesPerBlockExpected };

    mStatusLabel->setText(status, juce::NotificationType::dontSendNotification);
}

void MainComponent::updateTimeSlider()
{
    switch (mSonificationStatePtr->stateId()) {
        case SonificationState::StateId::STARTUP:
        case SonificationState::StateId::AUDIO_READY:
        case SonificationState::StateId::SHUTDOWN:
            break;
        default:
            mTimeSlider->setValue(mSonificationAudioTransportSource.getCurrentPosition(), juce::NotificationType::dontSendNotification);;
            break;
    }
}

void MainComponent::changeSimulationTime(const float timeInSeconds)
{
    switch (mSonificationStatePtr->stateId()) {
        case SonificationState::StateId::STARTUP:
        case SonificationState::StateId::AUDIO_READY:
        case SonificationState::StateId::SHUTDOWN:
            return;
        default:
            mSonificationAudioTransportSource.setPosition(timeInSeconds);
            mSonificationProcessor.onPositionChangedExternally();
    }
}

MainComponent::UpdateComponentsTimer::UpdateComponentsTimer(MainComponent& component)
    : mMainComponent{ component }
{
}

void MainComponent::UpdateComponentsTimer::timerCallback()
{
    mMainComponent.updateStatusLabel();
    mMainComponent.updateTimeSlider();
}

}