#pragma once

#include "sead/math/seadMatrix.h"
#include "sead/math/seadVector.h"

#include "Library/LiveActor/LiveActor.h"
#include "al/sensor/SensorHitGroup.h"

#include "HitSensor.h"
#include "types.h"

namespace al {
class HitSensorKeeper {
public:
    HitSensorKeeper(int);
    bool addSensor(al::LiveActor* sensorHost, const char* sensorName, u32 typeEnum, float radius, ushort maxCount, const sead::Vector3f* position,
                   const sead::Matrix34f* matrix, const sead::Vector3f& scale);
    void update(void);
    int getSensorNum(void) const;
    al::HitSensor* getSensor(int) const;
    void clear(void);
    void validate(void);
    void invalidate(void);
    void validateBySystem(void);
    void invalidateBySystem(void);
    al::HitSensor* getSensor(const char* sensorName) const;

    int mSensorKeeperNum;
    int mSensorNum;
    al::HitSensor** mSensors;  // 0x8
};
};  // namespace al
