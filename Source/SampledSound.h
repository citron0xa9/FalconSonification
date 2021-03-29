#pragma once

#include <JuceHeader.h>

namespace falconSound {

class SampledSound : public juce::AudioSource {
public:
    explicit SampledSound(juce::String soundSampleName, bool isLooping, juce::AudioFormatManager& audioFormatManager);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void pause();
    void resetToStart();

    bool isPlaying() const;

    void setGain(float gain);

private:
    std::unique_ptr<juce::AudioFormatReaderSource> mAudioReaderSourcePtr;
    juce::AudioTransportSource mTransportAudioSource;
};

}
