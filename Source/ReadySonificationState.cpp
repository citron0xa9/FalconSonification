#include "ReadySonificationState.h"

#include "StartPlayingSonificationState.h"
#include "MainComponent.h"

namespace falconSound {

ReadySonificationState::ReadySonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::READY }
{
}

bool ReadySonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
        case StateId::AUDIO_READY:
        case StateId::PAUSED_PLAYING:
        case StateId::PLAYING:
        case StateId::START_PLAYING:
        case StateId::STOP_PLAYING:
            return true;
        default:
            return false;
    }
}

void ReadySonificationState::enter()
{
    mUiMainComponent.mTimeSlider->setEnabled(true);
    mUiMainComponent.mStartSonificationButton->setEnabled(true);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(true);
    mUiMainComponent.mDeviceSettingsButton->setEnabled(true);

    mUiMainComponent.mSonificationAudioTransportSource.setNextReadPosition(0);
}

void ReadySonificationState::handleStartClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<StartPlayingSonificationState>(mUiMainComponent));
}

void ReadySonificationState::handleFlightlogFilenameComponentChanged()
{
    mUiMainComponent.loadFlightLog();
}

std::string ReadySonificationState::name() const
{
    return "Ready";
}

}