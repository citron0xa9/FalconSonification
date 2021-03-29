#include "StartupSonificationState.h"

#include "MainComponent.h"

namespace falconSound {

StartupSonificationState::StartupSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::STARTUP}
{
}

bool StartupSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    return false;
}

void StartupSonificationState::enter()
{
    mUiMainComponent.mTimeSlider->setEnabled(false);
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);
    mUiMainComponent.mDeviceSettingsButton->setEnabled(false);
}

std::string StartupSonificationState::name() const
{
    return "Startup";
}

}