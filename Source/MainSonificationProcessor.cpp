#include "MainSonificationProcessor.h"

namespace falconSound {

MainSonificationProcessor::MainSonificationProcessor()
    : mExpressionParserPtr{createExpressionParser()}
{
    auto& oscillatorPressureLow = mPressureLow.get<0>();
    oscillatorPressureLow.initialise([](float xValue) {
        return 0.6f * std::sin(xValue) + 0.2f * std::sin(2.0f*xValue) + 0.1f * std::sin(3.0f*xValue);
    });
    oscillatorPressureLow.setFrequency(30);
    auto& gainPressureLow = mPressureLow.get<1>();
    gainPressureLow.setGainLinear(0.0f);

    auto& oscillatorPressureHigh = mPressureHigh.get<0>();
    oscillatorPressureHigh.initialise([](float xValue) {
        return 0.3f * std::sin(4.0f*xValue) + 0.2f * std::sin(5.0f*xValue) + 0.1f * std::sin(6.0f*xValue) + 0.2f * std::sin(9.0f * xValue) + 0.1f * std::sin(10.0f * xValue);
    });
    oscillatorPressureHigh.setFrequency(30);

    auto& gainPressureHigh = mPressureHigh.get<1>();
    gainPressureHigh.setGainLinear(0.0f);

    mAudioFormatManager.registerBasicFormats();
    mEngineCloseSoundPtr = std::make_unique<SampledSound>("engine-close.ogg", true, mAudioFormatManager);
    mEngineDistantSoundPtr = std::make_unique<SampledSound>("engine-distant.ogg", true, mAudioFormatManager);
    mSonicBoomSoundPtr = std::make_unique<SampledSound>("sonic-boom.ogg", false, mAudioFormatManager);

    auto& engineSoundModulatorOscillator = mEngineSoundModulator.get<0>();
    engineSoundModulatorOscillator.initialise([](float xValue) {
        return 0.5f * (std::sin(xValue) + 1.0f);
    }, 128);
    engineSoundModulatorOscillator.setFrequency(1);
    mEngineSoundModulator.get<1>().setGainLinear(0.0f);

    mLowPass.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    mLowPass.setCutoffFrequency(100.0f);
}

void MainSonificationProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32_t>(samplesPerBlockExpected), 2 };
    mPressureLow.prepare(processSpec);
    mPressureHigh.prepare(processSpec);

    mEngineCloseSoundPtr->prepareToPlay(samplesPerBlockExpected, sampleRate);
    mEngineDistantSoundPtr->prepareToPlay(samplesPerBlockExpected, sampleRate);
    mSonicBoomSoundPtr->prepareToPlay(samplesPerBlockExpected, sampleRate);

    mEngineSoundModulator.prepare(processSpec);
    mLowPass.prepare(processSpec);

    mEngineCloseSoundPtr->start();
    mEngineDistantSoundPtr->start();

    mSampleRate = sampleRate;
    mSamplesPerBlockExpected = samplesPerBlockExpected;
    mAudioIsInitialized = true;
}
void MainSonificationProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& mainAudioSourceChannelInfo)
{
    mainAudioSourceChannelInfo.clearActiveBufferRegion();
    if (mCurrentSamplePosition >= mLengthInSamples) {
        setNextReadPosition(mCurrentSamplePosition + mainAudioSourceChannelInfo.numSamples);
        return;
    }

    if (mCurrentSamplePosition + mainAudioSourceChannelInfo.numSamples > mLengthInSamples) {
        juce::AudioSourceChannelInfo firstPartInfo{ mainAudioSourceChannelInfo };
        firstPartInfo.numSamples = mLengthInSamples - mCurrentSamplePosition;
        getNextAudioBlock(firstPartInfo);

        juce::AudioSourceChannelInfo secondPartInfo{ mainAudioSourceChannelInfo };
        secondPartInfo.startSample = mainAudioSourceChannelInfo.startSample + firstPartInfo.numSamples;
        secondPartInfo.numSamples = mainAudioSourceChannelInfo.numSamples - firstPartInfo.numSamples;
        getNextAudioBlock(secondPartInfo);
        return;
    }

    updateSonificationParameters(mCurrentSamplePosition / mSampleRate);
    float totalGain = 0.0f;

    mTemporaryAudioBuffer.setSize(mainAudioSourceChannelInfo.buffer->getNumChannels(), mainAudioSourceChannelInfo.buffer->getNumSamples());
    juce::AudioSourceChannelInfo partialAudioSourceChannelInfo(&mTemporaryAudioBuffer, 0, mainAudioSourceChannelInfo.numSamples);
    juce::dsp::AudioBlock<float> mainAudioBlock{ *mainAudioSourceChannelInfo.buffer, static_cast<size_t>(mainAudioSourceChannelInfo.startSample) };
    juce::dsp::ProcessContextReplacing<float> mainAudioContext{ mainAudioBlock };

    if (mEnableEngineSound && mEnableEngineModulation) {
        juce::dsp::AudioBlock<float> partialAudioBlock{ *partialAudioSourceChannelInfo.buffer, static_cast<size_t>(partialAudioSourceChannelInfo.startSample) };
        juce::dsp::ProcessContextReplacing<float> partialAudioContext{ partialAudioBlock };

        mEngineCloseSoundPtr->getNextAudioBlock(partialAudioSourceChannelInfo);
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);
        mEngineSoundModulator.process(partialAudioContext);
        modulateMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);
        totalGain += mEngineSoundModulator.get<1>().getGainLinear();
    }
    
    if (mEnableEngineSound) {
        mEngineCloseSoundPtr->getNextAudioBlock(partialAudioSourceChannelInfo);
        float engineCloseSoundGain = mEngineCloseSoundGainExpression.value();
        partialAudioSourceChannelInfo.buffer->applyGain(engineCloseSoundGain);
        totalGain += engineCloseSoundGain;
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);

        mEngineDistantSoundPtr->getNextAudioBlock(partialAudioSourceChannelInfo);
        float engineDistantSoundGain = mEngineDistantSoundGainExpression.value();
        totalGain += engineCloseSoundGain;
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);

        float engineAllSoundGain = mEngineAllSoundGainExpression.value();
        mainAudioSourceChannelInfo.buffer->applyGain(engineAllSoundGain);
    }
    
    if (mEnableLowpass) {
        mLowPass.process(mainAudioContext);
    }

    if (mEnableSonicBoom) {
        mSonicBoomSoundPtr->getNextAudioBlock(partialAudioSourceChannelInfo);
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);
        totalGain += 1.0f;
    }
    
    if (mEnableAirodynamicPressureSound) {
        partialAudioSourceChannelInfo.clearActiveBufferRegion();
        juce::dsp::AudioBlock<float> partialAudioBlock{ *partialAudioSourceChannelInfo.buffer, static_cast<size_t>(partialAudioSourceChannelInfo.startSample) };
        juce::dsp::ProcessContextReplacing<float> partialAudioContext{ partialAudioBlock };
        mPressureLow.process(juce::dsp::ProcessContextReplacing<float>{ partialAudioBlock });
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);
        totalGain += 1.0f;

        partialAudioSourceChannelInfo.clearActiveBufferRegion();
        juce::dsp::AudioBlock<float> partialAudioBlockHigh{ *partialAudioSourceChannelInfo.buffer, static_cast<size_t>(partialAudioSourceChannelInfo.startSample) };
        juce::dsp::ProcessContextReplacing<float> partialAudioContextHigh{ partialAudioBlockHigh };
        mPressureHigh.process(partialAudioContextHigh);
        addToMix(mainAudioSourceChannelInfo, partialAudioSourceChannelInfo);
        totalGain += 1.0f;
    }
    
    float finalGainAdjustment = 1.0f;
    if (totalGain > 0.0f) {
        //finalGainAdjustment = 1.0f / totalGain;
    }
    finalGainAdjustment *= mMasterGain;
    mainAudioSourceChannelInfo.buffer->applyGain(finalGainAdjustment);

    setNextReadPosition(mCurrentSamplePosition + mainAudioSourceChannelInfo.numSamples);
}

void MainSonificationProcessor::releaseResources()
{
    mEngineCloseSoundPtr->releaseResources();
    mEngineDistantSoundPtr->releaseResources();
    mSonicBoomSoundPtr->releaseResources();
}

void MainSonificationProcessor::setNextReadPosition(const juce::int64 newPosition)
{
    mCurrentSamplePosition = newPosition;
    if (newPosition >= mLengthInSamples && mIsLooping) {
        mCurrentSamplePosition = mLengthInSamples > 0 ? newPosition % mLengthInSamples : 0;
    }
}

juce::int64 MainSonificationProcessor::getNextReadPosition() const
{
    return mCurrentSamplePosition;
}

juce::int64 MainSonificationProcessor::getTotalLength() const
{
    return mLengthInSamples;
}

void MainSonificationProcessor::setLooping(const bool shouldLoop)
{
    mIsLooping = shouldLoop;
    if (mIsLooping && mCurrentSamplePosition >= mLengthInSamples) {
        setNextReadPosition(mCurrentSamplePosition);
    }
}

bool MainSonificationProcessor::isLooping() const
{
    return mIsLooping;
}

void MainSonificationProcessor::falconSimulation(FalconSimulation* simulationPtr)
{
    assert(mAudioIsInitialized);
    mSimulationPtr = simulationPtr;

    mExpressionSymbolTable.add_constants();
    mExpressionSymbolTable.add_constant("SimulationTimeTotal", mSimulationPtr->simulationLengthInSeconds());
    mExpressionSymbolTable.add_constant("FlightTimeTotal", mSimulationPtr->flightTimeInMilliseconds());
    mExpressionSymbolTable.add_constant("MaxDynamicPressure", mSimulationPtr->maxDynamicPressure());
    mExpressionSymbolTable.add_constant("MinAltitude", mSimulationPtr->minAltitude());
    mExpressionSymbolTable.add_constant("MaxAltitude", mSimulationPtr->maxAltitude());
    mExpressionSymbolTable.add_constant("MaxSpeed", mSimulationPtr->maxSpeed());
    mExpressionSymbolTable.add_constant("MaxAcceleration", mSimulationPtr->maxAcceleration());
    mExpressionSymbolTable.add_constant("MinFuel", mSimulationPtr->minFuel());
    mExpressionSymbolTable.add_constant("MaxFuel", mSimulationPtr->maxFuel());
    mExpressionSymbolTable.add_constant("MaxThrust", mSimulationPtr->maxThrust());
    mExpressionSymbolTable.add_constant("MaxPropellantFlow", mSimulationPtr->maxPropellantFlow());
    mExpressionSymbolTable.add_constant("MaxDownrage", mSimulationPtr->maxDownrage());
    mExpressionSymbolTable.add_constant("MaxDragForce", mSimulationPtr->maxDragForce());
    mExpressionSymbolTable.add_constant("MaxPitch", mSimulationPtr->maxPitch());
    mExpressionSymbolTable.add_constant("MinPitch", mSimulationPtr->minPitch());
    mExpressionSymbolTable.add_constant("MaxYaw", mSimulationPtr->maxYaw());
    mExpressionSymbolTable.add_constant("MinYaw", mSimulationPtr->minYaw());
    mExpressionSymbolTable.add_constant("MaxRoll", mSimulationPtr->maxRoll());
    mExpressionSymbolTable.add_constant("MinRoll", mSimulationPtr->minRoll());
    mExpressionSymbolTable.add_constant("MaxAngularAcceleration", mSimulationPtr->maxAngularAcceleration());
    mExpressionSymbolTable.add_constant("MaxAbsoluteNozzleAngle", mSimulationPtr->maxAbsoluteNozzleAngle());
    mExpressionSymbolTable.add_constant("MaxMachNumber", mSimulationPtr->maxMachNumber());

    mExpressionSymbolTable.create_variable("SimulationTime");
    mExpressionSymbolTable.create_variable("FlightTime");
    mExpressionSymbolTable.create_variable("DynamicPressure");
    mExpressionSymbolTable.create_variable("Altitude");
    mExpressionSymbolTable.create_variable("Speed");
    mExpressionSymbolTable.create_variable("Acceleration");
    mExpressionSymbolTable.create_variable("Fuel");
    mExpressionSymbolTable.create_variable("Throttle");
    mExpressionSymbolTable.create_variable("Thrust");
    mExpressionSymbolTable.create_variable("PropellantFlow");
    mExpressionSymbolTable.create_variable("Downrage");
    mExpressionSymbolTable.create_variable("DragForce");
    mExpressionSymbolTable.create_variable("Pitch");
    mExpressionSymbolTable.create_variable("Yaw");
    mExpressionSymbolTable.create_variable("Roll");
    mExpressionSymbolTable.create_variable("AngularAcceleration");
    mExpressionSymbolTable.create_variable("AbsoluteNozzleAngle");
    mExpressionSymbolTable.create_variable("MachNumber");

    mEngineCloseSoundGainExpression.register_symbol_table(mExpressionSymbolTable);
    mEngineDistantSoundGainExpression.register_symbol_table(mExpressionSymbolTable);
    mEngineAllSoundGainExpression.register_symbol_table(mExpressionSymbolTable);
    mEngineSoundModulatorFrequencyExpression.register_symbol_table(mExpressionSymbolTable);
    mEngineSoundModulatorGainExpression.register_symbol_table(mExpressionSymbolTable);
    mAirodynamicPressureLowGainExpression.register_symbol_table(mExpressionSymbolTable);
    mAirodynamicPressureHighGainExpression.register_symbol_table(mExpressionSymbolTable);
    mLowpassCutoffFrequencyExpression.register_symbol_table(mExpressionSymbolTable);

    engineCloseSoundGainExpression("0.15");
    engineDistantSoundGainExpression("0.15");
    engineAllSoundGainExpression("Throttle / 100.0");
    engineSoundModulatorFrequencyExpression("1.0 + (Speed / MaxSpeed) * 80");
    engineSoundModulatorGainExpression("0.09");
    airodynamicPressureLowGainExpression("(DynamicPressure / MaxDynamicPressure)^2");
    airodynamicPressureHighGainExpression("(DynamicPressure / MaxDynamicPressure)^4");
    lowpassCutoffFrequencyExpression("100 + ((Altitude / MaxAltitude) * 21000)");

    mLengthInSamples = mSimulationPtr->simulationLengthInSeconds() * mSampleRate;
    mLengthInSamples = std::ceil(mLengthInSamples / static_cast<float>(mSamplesPerBlockExpected)) * mSamplesPerBlockExpected; //round up to multiple of mSamplesPerBlockExpected
}

void MainSonificationProcessor::updateSonificationParameters(const float sonificationTimeSeconds)
{
    mSonificationTimeSeconds = sonificationTimeSeconds;
    const auto simulationData = mSimulationPtr->dataForTime(sonificationTimeSeconds);

    mExpressionSymbolTable.variable_ref("SimulationTime") = simulationData.mTimeSeconds;
    mExpressionSymbolTable.variable_ref("FlightTime") = simulationData.mFlightTimeMilliseconds;
    mExpressionSymbolTable.variable_ref("DynamicPressure") = simulationData.mDynamicPressurePascal;
    mExpressionSymbolTable.variable_ref("Altitude") = simulationData.mAltitudeKilometers;
    mExpressionSymbolTable.variable_ref("Speed") = simulationData.mSpeedKilometersPerHour;
    mExpressionSymbolTable.variable_ref("Acceleration") = simulationData.mAccelerationMetersPerSecond2;
    mExpressionSymbolTable.variable_ref("Fuel") = simulationData.mFuelKg;
    mExpressionSymbolTable.variable_ref("Throttle") = simulationData.mThrottlePercent;
    mExpressionSymbolTable.variable_ref("Thrust") = simulationData.mThrustKN;
    mExpressionSymbolTable.variable_ref("PropellantFlow") = simulationData.mPropellantFlowTonsPerSecond;
    mExpressionSymbolTable.variable_ref("Downrage") = simulationData.mDownrageKilometers;
    mExpressionSymbolTable.variable_ref("DragForce") = simulationData.mDragForceNewton;
    mExpressionSymbolTable.variable_ref("Pitch") = simulationData.mPitchDegrees;
    mExpressionSymbolTable.variable_ref("Yaw") = simulationData.mYawDegrees;
    mExpressionSymbolTable.variable_ref("Roll") = simulationData.mRollDegrees;
    mExpressionSymbolTable.variable_ref("AngularAcceleration") = simulationData.mAngularAccelerationMradPerS2;
    mExpressionSymbolTable.variable_ref("AbsoluteNozzleAngle") = simulationData.mAbsoluteNozzleAngleDegrees;
    mExpressionSymbolTable.variable_ref("MachNumber") = simulationData.mMachNumber;

    mEngineDistantSoundPtr->setGain(mEngineDistantSoundGainExpression.value());
    mEngineSoundModulator.get<0>().setFrequency(mEngineSoundModulatorFrequencyExpression.value());
    mEngineSoundModulator.get<1>().setGainLinear(mEngineSoundModulatorGainExpression.value());
    mPressureLow.get<1>().setGainLinear(mAirodynamicPressureLowGainExpression.value());
    mPressureHigh.get<1>().setGainLinear(mAirodynamicPressureHighGainExpression.value());
    mLowPass.setCutoffFrequency(mLowpassCutoffFrequencyExpression.value());

    const float currentMachNumber = simulationData.mMachNumber;
    if (currentMachNumber > 1.0f && mLastMachNumber <= 1.0f) {
        mSonicBoomSoundPtr->resetToStart();
        mSonicBoomSoundPtr->start();
    }
    mLastMachNumber = currentMachNumber;
}

void MainSonificationProcessor::engineCloseSoundGainExpression(const std::string& expressionString)
{
    compileExpression(mEngineCloseSoundGainExpression, expressionString);
    mEngineCloseSoundGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::engineCloseSoundGainExpression() const noexcept
{
    return mEngineCloseSoundGainExpressionStr;
}

void MainSonificationProcessor::engineDistantSoundGainExpression(const std::string& expressionString)
{
    compileExpression(mEngineDistantSoundGainExpression, expressionString);
    mEngineDistantSoundGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::engineDistantSoundGainExpression() const noexcept
{
    return mEngineDistantSoundGainExpressionStr;
}
void MainSonificationProcessor::engineAllSoundGainExpression(const std::string& expressionString)
{
    compileExpression(mEngineAllSoundGainExpression, expressionString);
    mEngineAllSoundGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::engineAllSoundGainExpression() const noexcept
{
    return mEngineAllSoundGainExpressionStr;
}

void MainSonificationProcessor::engineSoundModulatorFrequencyExpression(const std::string& expressionString)
{
    compileExpression(mEngineSoundModulatorFrequencyExpression, expressionString);
    mEngineSoundModulatorFrequencyExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::engineSoundModulatorFrequencyExpression() const noexcept
{
    return mEngineSoundModulatorFrequencyExpressionStr;
}
void MainSonificationProcessor::engineSoundModulatorGainExpression(const std::string& expressionString)
{
    compileExpression(mEngineSoundModulatorGainExpression, expressionString);
    mEngineSoundModulatorGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::engineSoundModulatorGainExpression() const noexcept
{
    return mEngineSoundModulatorGainExpressionStr;
}

void MainSonificationProcessor::airodynamicPressureLowGainExpression(const std::string& expressionString)
{
    compileExpression(mAirodynamicPressureLowGainExpression, expressionString);
    mAirodynamicPressureLowGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::airodynamicPressureLowGainExpression() const noexcept
{
    return mAirodynamicPressureLowGainExpressionStr;
}
void MainSonificationProcessor::airodynamicPressureHighGainExpression(const std::string& expressionString)
{
    compileExpression(mAirodynamicPressureHighGainExpression, expressionString);
    mAirodynamicPressureHighGainExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::airodynamicPressureHighGainExpression() const noexcept
{
    return mAirodynamicPressureHighGainExpressionStr;
}

void MainSonificationProcessor::lowpassCutoffFrequencyExpression(const std::string& expressionString)
{
    compileExpression(mLowpassCutoffFrequencyExpression, expressionString);
    mLowpassCutoffFrequencyExpressionStr = expressionString;
}
const std::string& MainSonificationProcessor::lowpassCutoffFrequencyExpression() const noexcept
{
    return mLowpassCutoffFrequencyExpressionStr;
}

void MainSonificationProcessor::enableEngineSound(bool shouldEnable)
{
    mEnableEngineSound = shouldEnable;
}
bool MainSonificationProcessor::isEngineSoundEnabled() const noexcept
{
    return mEnableEngineSound;
}
void MainSonificationProcessor::enableEngineModulation(bool shouldEnable)
{
    mEnableEngineModulation = shouldEnable;
}
bool MainSonificationProcessor::isEngineModulationEnabled() const noexcept
{
    return mEnableEngineModulation;
}
void MainSonificationProcessor::enableSonicBoom(bool shouldEnable)
{
    mEnableSonicBoom = shouldEnable;
}
bool MainSonificationProcessor::isSonicBoomEnabled() const noexcept
{
    return mEnableSonicBoom;
}
void MainSonificationProcessor::enableLowpass(bool shouldEnable)
{
    mEnableLowpass = shouldEnable;
}
bool MainSonificationProcessor::isLowpassEnabled() const noexcept
{
    return mEnableLowpass;
}
void MainSonificationProcessor::enableAirodynamicPressureSound(bool shouldEnable)
{
    mEnableAirodynamicPressureSound = shouldEnable;
}
bool MainSonificationProcessor::isAirodynamicPressureSoundEnabled() const noexcept
{
    return mEnableAirodynamicPressureSound;
}

void MainSonificationProcessor::masterGain(const float gain)
{
    assert(gain >= 0.0f);
    mMasterGain = gain;
}

float MainSonificationProcessor::masterGain() const noexcept
{
    return mMasterGain;
}

float MainSonificationProcessor::currentSonificationTimeInSeconds() const
{
    return (mCurrentSamplePosition / mSampleRate);
}

void MainSonificationProcessor::onPositionChangedExternally()
{
    if (mSonicBoomSoundPtr->isPlaying()) {
        mSonicBoomSoundPtr->pause();
    }
}

void MainSonificationProcessor::addToMix(const juce::AudioSourceChannelInfo& mixAudioInfo, const juce::AudioSourceChannelInfo& partialAudioInfo)
{
    for (int chan = 0; chan < mixAudioInfo.buffer->getNumChannels(); ++chan) {
        mixAudioInfo.buffer->addFrom(chan, mixAudioInfo.startSample, *partialAudioInfo.buffer, chan, 0, mixAudioInfo.numSamples);
    }
}

void MainSonificationProcessor::modulateMix(const juce::AudioSourceChannelInfo& mixAudioInfo, const juce::AudioSourceChannelInfo& modulateAudioInfo)
{
    if (mixAudioInfo.buffer->hasBeenCleared()) {
        return;
    }
    if (modulateAudioInfo.buffer->hasBeenCleared()) {
        mixAudioInfo.clearActiveBufferRegion();
        return;
    }

    for (int chan = 0; chan < mixAudioInfo.buffer->getNumChannels(); ++chan) {
        auto* destination = mixAudioInfo.buffer->getWritePointer(chan, mixAudioInfo.startSample);
        auto* source = modulateAudioInfo.buffer->getReadPointer(chan, mixAudioInfo.startSample);
        juce::FloatVectorOperations::multiply(destination, source, mixAudioInfo.numSamples);
    }
}

void MainSonificationProcessor::compileExpression(exprtk::expression<float>& expression, const std::string& expressionString)
{
    if (!mExpressionParserPtr->compile(expressionString, expression)) {
        throw std::invalid_argument("EXPRTK: Compiling expression failed: " + mExpressionParserPtr->error());
    }
}

std::unique_ptr<exprtk::parser<float>> MainSonificationProcessor::createExpressionParser()
{
    using settings_t = exprtk::parser<float>::settings_t;
    const auto compileOptions = settings_t::e_bracket_check + settings_t::e_sequence_check + settings_t::e_commutative_check + settings_t::e_strength_reduction;
    return std::make_unique<exprtk::parser<float>>(compileOptions);
}

}