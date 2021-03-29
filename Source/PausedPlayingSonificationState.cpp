#include "PausedPlayingSonificationState.h"

#include "StartPlayingSonificationState.h"
#include "ReadySonificationState.h"
#include "MainComponent.h"

namespace falconSound {

PausedPlayingSonificationState::PausedPlayingSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::PAUSED_PLAYING }
{
}

bool PausedPlayingSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    switch (otherState.stateId()) {
    case StateId::PAUSE_PLAYING:
        return true;
    default:
        return false;
    }
}

void PausedPlayingSonificationState::enter()
{
    mUiMainComponent.mStartSonificationButton->setEnabled(true);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(true);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);
}

void PausedPlayingSonificationState::handleStartClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<StartPlayingSonificationState>(mUiMainComponent));
}

void PausedPlayingSonificationState::handleStopClicked()
{
    mUiMainComponent.changeSonificationState(std::make_unique<ReadySonificationState>(mUiMainComponent));
}

std::string PausedPlayingSonificationState::name() const
{
    return "Paused Playing";
}

}