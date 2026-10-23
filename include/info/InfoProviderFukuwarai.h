#pragma once

#include "info/InfoProviderActor.h"

class InfoProviderFukuwaraiFaceParts : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "18FukuwaraiFaceParts";

    InfoProviderFukuwaraiFaceParts(al::LiveActor* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;
    void renderImpl() override;

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderFukuwaraiFaceParts(mActor); }
};

class InfoProviderFukuwaraiWatcher : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "16FukuwaraiWatcher";

    InfoProviderFukuwaraiWatcher(al::LiveActor* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderFukuwaraiWatcher(mActor); }
};
