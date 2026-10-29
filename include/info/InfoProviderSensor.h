#pragma once

#include "al/Project/HitSensor/HitSensor.h"

#include "info/InfoProviderBase.h"

class InfoProviderSensor : public InfoProviderBase {
public:
    using Key = al::HitSensor;

    InfoProviderSensor(Key* sensor, bool hasVisuals = true) : InfoProviderBase(sensor, hasVisuals) {}

private:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};
