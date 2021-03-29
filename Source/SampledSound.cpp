#include "SampledSound.h"

#include <cassert>

namespace falconSound {

SampledSound::SampledSound(juce::String soundSampleName, bool isLooping, juce::AudioFormatManager& audioFormatManager)
{
    auto sampleFile = juce::File::getCurrentWorkingDirectory().getChildFile("samples").getChildFile(soundSampleName);
    if (!sampleFile.existsAsFile()) {
        throw std::invalid_argument("SampledSound: Invalid audio sample file path supplied: " + sampleFile.getFullPathName().toStdString());
    }

    auto* audioReader = audioFormatManager.createReaderFor(sampleFile);
    if (!audioReader) {
        throw std::runtime_error("SampledSound: Failed opening audio sample file " + sampleFile.getFullPathName().toStdString());
    }

    mAudioReaderSourcePtr = std::make_unique<juce::AudioFormatReaderSource>(audioReader, true);
    mAudioReaderSourcePtr->setLooping(isLooping);
    mTransportAudioSource.setSource(mAudioReaderSourcePtr.get(), 0, nullptr, audioReader->sampleRate);
}

void SampledSound::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mTransportAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SampledSound::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mTransportAudioSource.getNextAudioBlock(bufferToFill);
}

void SampledSound::releaseResources()
{
    mTransportAudioSource.releaseResources();
}

void SampledSound::start()
{
    mTransportAudioSource.start();
}

void SampledSound::pause()
{
    mTransportAudioSource.stop();
}

void SampledSound::resetToStart()
{
    mTransportAudioSource.setNextReadPosition(0);
}

bool SampledSound::isPlaying() const
{
    return mTransportAudioSource.isPlaying();
}

void SampledSound::setGain(const float gain)
{
    mTransportAudioSource.setGain(gain);
}

}
