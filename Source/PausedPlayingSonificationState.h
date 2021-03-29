#pragma once

#include "SonificationState.h"

namespace falconSound {

class PausedPlayingSonificationState : public SonificationState {
public:
    explicit PausedPlayingSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handleStartClicked() override;
    void handleStopClicked() override;

    std::string name() const override;
};

}