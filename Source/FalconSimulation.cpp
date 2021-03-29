#include "FalconSimulation.h"

#include <cassert>

namespace falconSound {

FalconSimulation::FalconSimulation(const juce::File& flightLog)
{
    parseFlightLog(flightLog);
}

float FalconSimulation::simulationLengthInSeconds() const noexcept
{
    return mSimulationLengthSeconds;
}

int32_t FalconSimulation::flightTimeInMilliseconds() const noexcept
{
    return mFlightTime;
}

FalconSimulation::TimepointData FalconSimulation::dataForTime(const float simulationTimeSeconds) const
{
    const float index = mEntriesPerSecond * simulationTimeSeconds;
    const size_t indexLow = static_cast<size_t>(index);
    const float interpolationFactor = index - indexLow;

    assert(mFlightLog.size() > indexLow + 1);

    return interpolateTimepointData(mFlightLog[indexLow], mFlightLog[indexLow + 1], interpolationFactor);
}

int32_t FalconSimulation::maxDynamicPressure() const noexcept
{
    return mMaxDynamicPressure;
}

float FalconSimulation::maxAltitude() const noexcept
{
    return mMaxAltitude;
}

float FalconSimulation::minAltitude() const noexcept
{
    return mMinAltitude;
}

float FalconSimulation::maxSpeed() const noexcept
{
    return mMaxSpeed;
}

float FalconSimulation::maxAcceleration() const noexcept
{
    return mMaxAcceleration;
}

int32_t FalconSimulation::minFuel() const noexcept
{
    return mMinFuel;
}

int32_t FalconSimulation::maxFuel() const noexcept
{
    return mMaxFuel;
}

float FalconSimulation::maxThrust() const noexcept
{
    return mMaxThrust;
}

float FalconSimulation::maxPropellantFlow() const noexcept
{
    return mMaxPropellantFlow;
}

float FalconSimulation::maxDownrage() const noexcept
{
    return mMaxDownrage;
}

int32_t FalconSimulation::maxDragForce() const noexcept
{
    return mMaxDragForce;
}

float FalconSimulation::maxPitch() const noexcept
{
    return mMaxPitch;
}

float FalconSimulation::minPitch() const noexcept
{
    return mMinPitch;
}

float FalconSimulation::maxYaw() const noexcept
{
    return mMaxYaw;
}

float FalconSimulation::minYaw() const noexcept
{
    return mMinYaw;
}

float FalconSimulation::maxRoll() const noexcept
{
    return mMaxRoll;
}

float FalconSimulation::minRoll() const noexcept
{
    return mMinRoll;
}

float FalconSimulation::maxAngularAcceleration() const noexcept
{
    return mMaxAngularAcceleration;
}

float FalconSimulation::maxAbsoluteNozzleAngle() const noexcept
{
    return mMaxAbsoluteNozzleAngle;
}

float FalconSimulation::maxMachNumber() const noexcept
{
    return mMaxMachNumber;
}


void FalconSimulation::parseFlightLog(const juce::File& flightLog)
{
    if (!flightLog.existsAsFile()) {
        throw std::invalid_argument("FalconSimulation::parseFlightLog: Invalid flight log file given");
    }

    juce::FileInputStream inputStream{ flightLog };
    if (!inputStream.openedOk()) {
        throw std::runtime_error("FalconSimulation::parseFlightLog: Failed to open input stream");
    }

    while (!inputStream.isExhausted()) {
        parseFlightLogLine(inputStream.readNextLine());
    }
}

void FalconSimulation::parseFlightLogLine(juce::String line)
{
    static constexpr float simulationTimeOffset = -3.0;

    juce::StringArray lineElements;
    lineElements.addTokens(line, "\t;,", "\"");

    if ((lineElements[0].getFloatValue() + simulationTimeOffset) < 0) {
        return;
    }
    
    mFlightLog.emplace_back();
    TimepointData& lineData = mFlightLog.back();
    lineData.mTimeSeconds = lineElements[0].getFloatValue() + simulationTimeOffset;
    lineData.mFlightTimeMilliseconds = static_cast<uint32_t>(lineElements[1].getIntValue());
    lineData.mFuelKg = static_cast<uint32_t>(lineElements[2].getIntValue());
    lineData.mThrottlePercent = lineElements[3].getFloatValue();
    lineData.mThrustKN = lineElements[4].getFloatValue();
    lineData.mPropellantFlowTonsPerSecond = lineElements[5].getFloatValue();
    lineData.mAccelerationMetersPerSecond2 = lineElements[6].getFloatValue();
    lineData.mSpeedKilometersPerHour = lineElements[7].getFloatValue();
    lineData.mAltitudeKilometers = lineElements[8].getFloatValue();
    lineData.mDownrageKilometers = lineElements[9].getFloatValue();
    lineData.mDynamicPressurePascal = static_cast<uint32_t>(lineElements[10].getIntValue());
    lineData.mDragForceNewton = static_cast<uint32_t>(lineElements[11].getIntValue());
    lineData.mLiftForceNewton = static_cast<uint32_t>(lineElements[12].getIntValue());
    lineData.mPitchDegrees = lineElements[13].getFloatValue();
    lineData.mYawDegrees = lineElements[14].getFloatValue();
    lineData.mRollDegrees = lineElements[15].getFloatValue();
    lineData.mAngularAccelerationMradPerS2 = lineElements[16].getFloatValue();
    lineData.mAbsoluteNozzleAngleDegrees = lineElements[17].getFloatValue();

    const float speedOfSound = calculateSpeedOfSound(lineData.mAltitudeKilometers * 1000.0f);
    lineData.mMachNumber = lineData.mSpeedKilometersPerHour / (3.6f * speedOfSound);

    updateGlobalData(lineData);
}

void FalconSimulation::updateGlobalData(const TimepointData& data)
{
    if (mEntriesPerSecond == 0 && data.mTimeSeconds > 0.0f) {
        mEntriesPerSecond = 1.0f / data.mTimeSeconds;
    }
    mSimulationLengthSeconds = data.mTimeSeconds;
    mFlightTime = data.mFlightTimeMilliseconds;

    mMaxDynamicPressure = std::max(mMaxDynamicPressure, data.mDynamicPressurePascal);
    mMinAltitude = std::min(mMinAltitude, data.mAltitudeKilometers);
    mMaxAltitude = std::max(mMaxAltitude, data.mAltitudeKilometers);
    mMaxAcceleration = std::max(mMaxAcceleration, data.mAccelerationMetersPerSecond2);
    mMaxSpeed = std::max(mMaxSpeed, data.mSpeedKilometersPerHour);
    mMinFuel = std::min(mMinFuel, data.mFuelKg);
    mMaxFuel = std::max(mMaxFuel, data.mFuelKg);
    mMaxThrust = std::max(mMaxThrust, data.mThrustKN);
    mMaxPropellantFlow = std::max(mMaxPropellantFlow, data.mPropellantFlowTonsPerSecond);
    mMaxDownrage = std::max(mMaxDownrage, data.mDownrageKilometers);
    mMaxDragForce = std::max(mMaxDragForce, data.mDragForceNewton);
    mMaxPitch = std::max(mMaxPitch, data.mPitchDegrees);
    mMinPitch = std::min(mMinPitch, data.mPitchDegrees);
    mMaxYaw = std::max(mMaxYaw, data.mYawDegrees);
    mMinYaw = std::min(mMinYaw, data.mYawDegrees);
    mMaxRoll = std::max(mMaxRoll, data.mRollDegrees);
    mMinRoll = std::min(mMinRoll, data.mRollDegrees);
    mMaxAngularAcceleration = std::max(mMaxAngularAcceleration, data.mAngularAccelerationMradPerS2);
    mMaxAbsoluteNozzleAngle = std::max(mMaxAbsoluteNozzleAngle, data.mAbsoluteNozzleAngleDegrees);
    mMaxMachNumber = std::max(mMaxMachNumber, data.mMachNumber);
}

FalconSimulation::TimepointData FalconSimulation::interpolateTimepointData(const TimepointData& first, const TimepointData& second, const float factor)
{
    TimepointData result;
    result.mTimeSeconds = interpolatePrimitive(first.mTimeSeconds, second.mTimeSeconds, factor);
    result.mFlightTimeMilliseconds = interpolatePrimitive(first.mFlightTimeMilliseconds, second.mFlightTimeMilliseconds, factor);
    result.mFuelKg = interpolatePrimitive(first.mFuelKg, second.mFuelKg, factor);
    result.mThrottlePercent = interpolatePrimitive(first.mThrottlePercent, second.mThrottlePercent, factor);
    result.mThrustKN = interpolatePrimitive(first.mThrustKN, second.mThrustKN, factor);
    result.mPropellantFlowTonsPerSecond = interpolatePrimitive(first.mPropellantFlowTonsPerSecond, second.mPropellantFlowTonsPerSecond, factor);
    result.mAccelerationMetersPerSecond2 = interpolatePrimitive(first.mAccelerationMetersPerSecond2, second.mAccelerationMetersPerSecond2, factor);
    result.mSpeedKilometersPerHour = interpolatePrimitive(first.mSpeedKilometersPerHour, second.mSpeedKilometersPerHour, factor);
    result.mAltitudeKilometers = interpolatePrimitive(first.mAltitudeKilometers, second.mAltitudeKilometers, factor);
    result.mDownrageKilometers = interpolatePrimitive(first.mDownrageKilometers, second.mDownrageKilometers, factor);
    result.mDynamicPressurePascal = interpolatePrimitive(first.mDynamicPressurePascal, second.mDynamicPressurePascal, factor);
    result.mDragForceNewton = interpolatePrimitive(first.mDragForceNewton, second.mDragForceNewton, factor);
    result.mLiftForceNewton = interpolatePrimitive(first.mLiftForceNewton, second.mLiftForceNewton, factor);
    result.mPitchDegrees = interpolatePrimitive(first.mPitchDegrees, second.mPitchDegrees, factor);
    result.mYawDegrees = interpolatePrimitive(first.mYawDegrees, second.mYawDegrees, factor);
    result.mRollDegrees = interpolatePrimitive(first.mRollDegrees, second.mRollDegrees, factor);
    result.mAngularAccelerationMradPerS2 = interpolatePrimitive(first.mAngularAccelerationMradPerS2, second.mAngularAccelerationMradPerS2, factor);
    result.mAbsoluteNozzleAngleDegrees = interpolatePrimitive(first.mAbsoluteNozzleAngleDegrees, second.mAbsoluteNozzleAngleDegrees, factor);
    result.mMachNumber = interpolatePrimitive(first.mMachNumber, second.mMachNumber, factor);

    return result;
}

template<typename T>
T FalconSimulation::interpolatePrimitive(const T first, const T second, const float factor)
{
    return first + (factor * (second - first));
}

float FalconSimulation::calculateSpeedOfSound(float altitudeInMeters) noexcept
{
    float temperatureCelsius;
    if (altitudeInMeters < 11019.13f) {
        temperatureCelsius = 15.0f - (0.001085088f * altitudeInMeters);
    } else if (altitudeInMeters < 25.098756f) {
        temperatureCelsius = -56.6667f;
    }
    else {
        temperatureCelsius = -131.694444f + (0.000499872f * altitudeInMeters);
    }
    const float temperatureKelvin = 273.15 + temperatureCelsius;
    const float speedOfSound = std::sqrt(SPECIFIC_HEAT_RATIO * SPECIFIC_GAS_CONSTANT * temperatureKelvin);
    return speedOfSound;
}

}
