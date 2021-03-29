#pragma once

#include "SonificationState.h"

namespace falconSound {

class PausePlayingSonificationState : public SonificationState {
public:
    explicit PausePlayingSonificationState(MainComponent& uiMainComponent);

    bool canTransitionFromState(const SonificationState& otherState) const override;
    void enter() override;

    void handleSonificationAudioSourceChanged() override;

    std::string name() const override;
};

}