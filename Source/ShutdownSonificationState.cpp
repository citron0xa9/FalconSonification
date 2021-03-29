#include "ShutdownSonificationState.h"

#include "MainComponent.h"

namespace falconSound {

ShutdownSonificationState::ShutdownSonificationState(MainComponent& uiMainComponent)
    : SonificationState{ uiMainComponent, StateId::SHUTDOWN }
{
}

bool ShutdownSonificationState::canTransitionFromState(const SonificationState& otherState) const
{
    return true;
}

void ShutdownSonificationState::enter()
{
    mUiMainComponent.mTimeSlider->setEnabled(false);
    mUiMainComponent.mStartSonificationButton->setEnabled(false);
    mUiMainComponent.mPauseSonificationButton->setEnabled(false);
    mUiMainComponent.mStopSonificationButton->setEnabled(false);
    mUiMainComponent.mFlightLogFilenameComponent->setEnabled(false);
    mUiMainComponent.mDeviceSettingsButton->setEnabled(false);

    if (mUiMainComponent.mSonificationAudioTransportSource.isPlaying()) {
        mUiMainComponent.mSonificationAudioTransportSource.stop();
    }
}

std::string ShutdownSonificationState::name() const
{
    return "Shutdown";
}

}