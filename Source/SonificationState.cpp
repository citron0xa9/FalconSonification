#include "SonificationState.h"

namespace falconSound {

SonificationState::SonificationState(MainComponent& uiMainComponent, const StateId stateId)
    : mUiMainComponent{ uiMainComponent }, mStateId{ stateId }
{
}

SonificationState::StateId SonificationState::stateId() const noexcept
{
    return mStateId;
}

void SonificationState::handleStartClicked()
{
}

void SonificationState::handlePauseClicked()
{
}

void SonificationState::handleStopClicked()
{
}

void SonificationState::handleSonificationAudioSourceChanged()
{
}

void SonificationState::handleFlightlogFilenameComponentChanged()
{
}

}