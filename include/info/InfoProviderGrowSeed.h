#pragma once

#include "info/InfoProviderActor.h"

class InfoProviderGrowPlantSeed : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "13GrowPlantSeed";

    InfoProviderGrowPlantSeed(Key* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;
};
