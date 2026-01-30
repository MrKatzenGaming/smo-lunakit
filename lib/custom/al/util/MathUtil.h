#pragma once

#include <sead/math/seadVector.h>

namespace al {

class LiveActor;

void normalize(sead::Vector3f*);
float normalize(float, float, float);
float normalize(signed int, signed int, signed int);

float easeIn(float);
float easeOut(float);
float easeInOut(float);

float modf(float, float);
int modi(int, int);

float sign(float);
int sign(int);

float squareIn(float);
float squareOut(float);

float powerIn(float, float);
float powerOut(float, float);

float lerpValue(float, float, float);

bool isNearZero(float, float);

bool isNearZero(const sead::Vector3f&, float);

template <typename T>
inline T clamp(T value, T min, T max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

float calcSpringDumperForce(float unk1, float unk2, float unk3, float unk4);

void separateVectorHV(sead::Vector3f*, sead::Vector3f*, const sead::Vector3f&, const sead::Vector3f&);

bool tryNormalizeOrDirZ(sead::Vector3f*);
bool tryNormalizeOrDirZ(sead::Vector3f*, const sead::Vector3f&);
bool tryNormalizeOrZero(sead::Vector3f*, const sead::Vector3f&);
void rotateVectorDegree(sead::Vector3f*, const sead::Vector3f&, const sead::Vector3f&, float);
void rotateVectorDegreeX(sead::Vector3f*, float);
void rotateVectorDegreeY(sead::Vector3f*, float);
void rotateVectorDegreeZ(sead::Vector3f*, float);

float calcAngleToTargetH(const LiveActor*, const sead::Vector3f&);
float calcAngleToTargetV(const LiveActor*, const sead::Vector3f&);
float calcAngleRadian(const sead::Vector3f&, const sead::Vector3f&);
float calcAngleDegree(const sead::Vector3f&, const sead::Vector3f&);
float calcAngleDegree(const sead::Vector2f&, const sead::Vector2f&);
float calcAngleOnPlaneRadian(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);
float calcAngleOnPlaneDegree(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);
float calcAngleOnPlaneDegreeOrZero(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);
float calcAngleSignOnPlane(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);
bool tryCalcAngleDegree(float*, const sead::Vector3f&, const sead::Vector3f&);
bool tryCalcAngleOnPlaneDegree(float*, const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&);

};  // namespace al
