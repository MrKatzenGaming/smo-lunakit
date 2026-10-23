#pragma once

#include "Library/LiveActor/LiveActor.h"

#include "info/InfoProviderActor.h"

class InfoProviderGrowPlantSeed : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "13GrowPlantSeed";

    InfoProviderGrowPlantSeed(al::LiveActor* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderGrowPlantSeed(mActor); }
};
