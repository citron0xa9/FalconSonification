#include "StartPlayingSonificationState.h"

#include "ReadySonificationState.h"
#include "PausePlayingSonificationState.h"
#include "StopPlayingSonificationState.h"
#include "PlayingSonificationState.h"
#include "MainComponent.h"

namespace falconSound {

StartPlayingSonificationState::StartPlayingSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::START_PLAYING }
{
}

bool StartPlayingSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
    case StateId::READY:
    case StateId::PAUSED_PLAYING:
    case StateId::PAUSE_PLAYING:
        return true;
    default:
        return false;
    }
}

void StartPlayingSonificationState::enter()
{
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);
    mUiMainComponent.mDeviceSettingsButton->setEnabled(false);

    mUiMainComponent.mSonificationAudioTransportSource.start();
}

void StartPlayingSonificationState::handlePauseClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<PausePlayingSonificationState>(mUiMainComponent));
}

void StartPlayingSonificationState::handleStopClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<StopPlayingSonificationState>(mUiMainComponent));
}

void StartPlayingSonificationState::handleSonificationAudioSourceChanged()
{
    if (mUiMainComponent.mSonificationAudioTransportSource.isPlaying()) {
        mUiMainComponent.changeSonificationState(std::make_unique<PlayingSonificationState>(mUiMainComponent));
    }
    else {
        mUiMainComponent.changeSonificationState(std::make_unique<ReadySonificationState>(mUiMainComponent));
    }
}

std::string StartPlayingSonificationState::name() const
{
    return "Start Playing";
}

}