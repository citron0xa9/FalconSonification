#include "StopPlayingSonificationState.h"

#include "ReadySonificationState.h"
#include "MainComponent.h"

namespace falconSound {

StopPlayingSonificationState::StopPlayingSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::STOP_PLAYING }
{
}

bool StopPlayingSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
    case StateId::PLAYING:
        return true;
    default:
        return false;
    }
}

void StopPlayingSonificationState::enter()
{
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);

    mUiMainComponent.mSonificationAudioTransportSource.stop();
}

void StopPlayingSonificationState::handleSonificationAudioSourceChanged()
{
    if (!mUiMainComponent.mSonificationAudioTransportSource.isPlaying()) {
        mUiMainComponent.changeSonificationState(std::make_unique<ReadySonificationState>(mUiMainComponent));
    }
}

std::string StopPlayingSonificationState::name() const
{
    return "Stop Playing";
}

}