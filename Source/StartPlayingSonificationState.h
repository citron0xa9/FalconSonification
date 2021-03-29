#pragma once

#include "SonificationState.h"

namespace falconSound {

class StartPlayingSonificationState : public SonificationState {
public:
    explicit StartPlayingSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handlePauseClicked() override;
    void handleStopClicked() override;
    void handleSonificationAudioSourceChanged() override;

    std::string name() const override;
};

}