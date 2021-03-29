#pragma once

#include "SonificationState.h"

namespace falconSound {

class StartupSonificationState : public SonificationState {
public:
    explicit StartupSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    std::string name() const override;
};

}