#pragma once

#include "SonificationState.h"

namespace falconSound {

class ShutdownSonificationState : public SonificationState {
public:
    explicit ShutdownSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    std::string name() const override;
};

}