#pragma once

#include "SonificationState.h"

namespace falconSound {

class ReadySonificationState : public SonificationState {
public:
    explicit ReadySonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handleStartClicked() override;
    void handleFlightlogFilenameComponentChanged() override;

    std::string name() const override;
};

}