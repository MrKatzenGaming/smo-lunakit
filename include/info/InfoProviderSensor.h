#pragma once

#include "al/Project/HitSensor/HitSensor.h"

#include "info/InfoProviderBase.h"

class InfoProviderSensor : public InfoProviderBase {
public:
    InfoProviderSensor(al::HitSensor* sensor, bool hasVisuals = true) : mSensor(sensor), InfoProviderBase(hasVisuals) {}

private:
    void displayImpl() override;
    void renderImpl() override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;

    const void* getKey() const override { return mSensor; }
    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderSensor(mSensor); }

    al::HitSensor* mSensor;
    bool mShowVisuals = true;
};
