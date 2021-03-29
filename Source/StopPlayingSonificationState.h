#pragma once

#include "SonificationState.h"

namespace falconSound {

class StopPlayingSonificationState : public SonificationState {
public:
    explicit StopPlayingSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handleSonificationAudioSourceChanged() override;

    std::string name() const override;
};

}