#include "PausePlayingSonificationState.h"

#include "PausedPlayingSonificationState.h"
#include "MainComponent.h"

namespace falconSound {

PausePlayingSonificationState::PausePlayingSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::PAUSE_PLAYING }
{
}

bool PausePlayingSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
    case StateId::PLAYING:
        return true;
    default:
        return false;
    }
}

void PausePlayingSonificationState::enter()
{
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);

    mUiMainComponent.mSonificationAudioTransportSource.stop();
}

void PausePlayingSonificationState::handleSonificationAudioSourceChanged()
{
    if (!mUiMainComponent.mSonificationAudioTransportSource.isPlaying()) {
        mUiMainComponent.changeSonificationState(std::make_unique<PausedPlayingSonificationState>(mUiMainComponent));
    }
}

std::string PausePlayingSonificationState::name() const
{
    return "Pause Playing";
}

}