#include "AudioReadySonificationState.h"

#include "ReadySonificationState.h"
#include "MainComponent.h"

namespace falconSound {

AudioReadySonificationState::AudioReadySonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::AUDIO_READY }
{
}

bool AudioReadySonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
        case StateId::STARTUP:
        case StateId::READY:
            return true;
        default:
            return false;
    }
}

void AudioReadySonificationState::enter()
{
    mUiMainComponent.mTimeSlider->setEnabled(false);
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mDeviceSettingsButton->setEnabled(true);

    mUiMainComponent.createFlightlogFilenameComponent();
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(true);
    mUiMainComponent.postCommandMessage(MainComponent::RELAYOUT_COMMAND_ID);
}

void AudioReadySonificationState::handleFlightlogFilenameComponentChanged()
{
    mUiMainComponent.loadFlightLog();
    mUiMainComponent.changeSonificationState(std::make_unique<ReadySonificationState>(mUiMainComponent));
}

std::string AudioReadySonificationState::name() const
{
    return "Audio Ready";
}

}