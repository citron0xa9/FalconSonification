#pragma once

#include <JuceHeader.h>

namespace falconSound {

class FalconSimulation {
public:
    explicit FalconSimulation(const juce::File& flightLog);

    struct TimepointData {
        float mTimeSeconds;
        int32_t mFlightTimeMilliseconds;
        int32_t mFuelKg;
        float mThrottlePercent;
        float mThrustKN;
        float mPropellantFlowTonsPerSecond;
        float mAccelerationMetersPerSecond2;
        float mSpeedKilometersPerHour;
        float mAltitudeKilometers;
        float mDownrageKilometers;
        int32_t mDynamicPressurePascal;
        int32_t mDragForceNewton;
        int32_t mLiftForceNewton;
        float mPitchDegrees;
        float mYawDegrees;
        float mRollDegrees;
        float mAngularAccelerationMradPerS2;
        float mAbsoluteNozzleAngleDegrees;
        float mMachNumber;
    };

    float simulationLengthInSeconds() const noexcept;
    int32_t flightTimeInMilliseconds() const noexcept;
    TimepointData dataForTime(float simulationTimeSeconds) const;

    int32_t maxDynamicPressure() const noexcept;
    float minAltitude() const noexcept;
    float maxAltitude() const noexcept;
    float maxSpeed() const noexcept;
    float maxAcceleration() const noexcept;
    int32_t minFuel() const noexcept;
    int32_t maxFuel() const noexcept;
    float maxThrust() const noexcept;
    float maxPropellantFlow() const noexcept;
    float maxDownrage() const noexcept;
    int32_t maxDragForce() const noexcept;
    float maxPitch() const noexcept;
    float minPitch() const noexcept;
    float maxYaw() const noexcept;
    float minYaw() const noexcept;
    float maxRoll() const noexcept;
    float minRoll() const noexcept;
    float maxAngularAcceleration() const noexcept;
    float maxAbsoluteNozzleAngle() const noexcept;
    float maxMachNumber() const noexcept;

private:
    static constexpr float SPECIFIC_HEAT_RATIO = 1.4f;
    static constexpr float SPECIFIC_GAS_CONSTANT = 286.0f;
    std::vector<TimepointData> mFlightLog;

    uint16_t mEntriesPerSecond = 0;
    float mSimulationLengthSeconds;
    int32_t mFlightTime;
    int32_t mMaxDynamicPressure = 0;
    float mMinAltitude = std::numeric_limits<float>::max();
    float mMaxAltitude = std::numeric_limits<float>::lowest();
    float mMaxSpeed = std::numeric_limits<float>::lowest();
    float mMaxAcceleration = std::numeric_limits<float>::lowest();
    int32_t mMinFuel = std::numeric_limits<int32_t>::max();
    int32_t mMaxFuel = std::numeric_limits<int32_t>::lowest();
    float mMaxThrust = std::numeric_limits<float>::lowest();
    float mMaxPropellantFlow = std::numeric_limits<float>::lowest();
    float mMaxDownrage = std::numeric_limits<float>::lowest();
    int32_t mMaxDragForce = 0;
    float mMaxPitch = std::numeric_limits<float>::lowest();
    float mMinPitch = std::numeric_limits<float>::max();
    float mMaxYaw = std::numeric_limits<float>::lowest();
    float mMinYaw = std::numeric_limits<float>::max();
    float mMaxRoll = std::numeric_limits<float>::lowest();
    float mMinRoll = std::numeric_limits<float>::max();
    float mMaxAngularAcceleration = std::numeric_limits<float>::lowest();
    float mMaxAbsoluteNozzleAngle = std::numeric_limits<float>::lowest();
    float mMaxMachNumber = std::numeric_limits<float>::lowest();


    void parseFlightLog(const juce::File& flightLog);
    void parseFlightLogLine(juce::String line);
    void updateGlobalData(const TimepointData& data);

    static TimepointData interpolateTimepointData(const TimepointData& first, const TimepointData& second, float factor);
    
    template<typename T>
    static T interpolatePrimitive(T first, T second, float factor);

    static float calculateSpeedOfSound(float altitudeInMeters) noexcept;
};

}