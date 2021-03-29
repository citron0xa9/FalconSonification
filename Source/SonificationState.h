#pragma once

#include <string>

namespace falconSound {

class MainComponent;

class SonificationState {
public:
    enum class StateId {
        STARTUP,
        AUDIO_READY,
        READY,
        START_PLAYING,
        PLAYING,
        STOP_PLAYING,
        PAUSE_PLAYING,
        PAUSED_PLAYING,
        SHUTDOWN
    };

    explicit SonificationState(MainComponent& uiMainComponent, StateId stateId);
    virtual ~SonificationState() = default;

    virtual bool canTransitionFromState(const SonificationState& otherState) const = 0;
    virtual void enter() = 0;

    virtual void handleStartClicked();
    virtual void handlePauseClicked();
    virtual void handleStopClicked();
    virtual void handleSonificationAudioSourceChanged();
    virtual void handleFlightlogFilenameComponentChanged();

    StateId stateId() const noexcept;

    virtual std::string name() const = 0;

protected:
    MainComponent& mUiMainComponent;

private:
    StateId mStateId;
};

}