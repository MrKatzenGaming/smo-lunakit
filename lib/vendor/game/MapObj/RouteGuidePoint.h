#pragma once

#include "Library/HostIO/IUseName.h"

#include <math/seadVector.h>

namespace al {
class LiveActor;
};  // namespace al

class RouteGuidePoint : virtual public al::IUseName {
public:
    RouteGuidePoint();

    virtual const sead::Vector3f& getGuideTrans() = 0;

    virtual void onHorizontalGuide();
    virtual void offHorizontalGuide();
    virtual bool isHorizontalGuide() const { return mIsHorizontal; }

    virtual void validateGuide();
    virtual void invalidateGuide();
    virtual bool isValidateGuide() const { return mIsValidate; }

private:
    bool mIsValidate;
    bool mIsHorizontal;
};

class RouteGuidePointTrans : public RouteGuidePoint {
public:
    RouteGuidePointTrans(const sead::Vector3f& trans);

    const sead::Vector3f& getGuideTrans() override;

private:
    sead::Vector3f mTrans;
};

class RouteGuidePointTransPtr : public RouteGuidePoint {
public:
    RouteGuidePointTransPtr(const sead::Vector3f* transPtr);

    const sead::Vector3f& getGuideTrans() override;

private:
    const sead::Vector3f* mTransPtr;
};

class RouteGuidePointActor : public RouteGuidePoint {
public:
    RouteGuidePointActor(al::LiveActor* actor);

    const sead::Vector3f& getGuideTrans() override;
    const char* getName() const override;

private:
    const sead::Vector3f* mTransPtr;
};
