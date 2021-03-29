#pragma once

#include "SonificationState.h"

namespace falconSound {

class AudioReadySonificationState : public SonificationState {
public:
    explicit AudioReadySonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handleFlightlogFilenameComponentChanged() override;

    std::string name() const override;
};

}