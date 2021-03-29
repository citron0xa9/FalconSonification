#include "PlayingSonificationState.h"

#include "ReadySonificationState.h"
#include "PausePlayingSonificationState.h"
#include "StopPlayingSonificationState.h"
#include "MainComponent.h"

namespace falconSound {

PlayingSonificationState::PlayingSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::PLAYING }
{
}

bool PlayingSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
    case StateId::START_PLAYING:
        return true;
    default:
        return false;
    }
}

void PlayingSonificationState::enter()
{
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(true);
    mUiMainComponent.mStopSonificationButton->setEnabled(true);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);
}

void PlayingSonificationState::handlePauseClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<PausePlayingSonificationState>(mUiMainComponent));
}

void PlayingSonificationState::handleStopClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<StopPlayingSonificationState>(mUiMainComponent));
}

void PlayingSonificationState::handleSonificationAudioSourceChanged()
{
    if (!mUiMainComponent.mSonificationAudioTransportSource.isPlaying()) {
        mUiMainComponent.changeSonificationState(std::make_unique<ReadySonificationState>(mUiMainComponent));
    }
}

std::string PlayingSonificationState::name() const
{
    return "Playing";
}

}