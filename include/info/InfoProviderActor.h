#pragma once

#include "al/Library/LiveActor/LiveActor.h"

#include "info/InfoProviderBase.h"

class InfoProviderActor : public InfoProviderBase {
public:
    InfoProviderActor(al::LiveActor* actor, bool hasVisuals = true) : mActor(actor), InfoProviderBase(hasVisuals) {}

    static void displayActor(al::LiveActor* actor);
    static void renderActor(al::LiveActor* actor);

    void displayName();
    void displayPose();
    void displayFlags();
    void displayNerves();
    void displaySensors();

    void renderPose();
    void renderNerves();
    void renderSensors();

protected:
    void displayImpl() override;
    void renderImpl() override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;

    const void* getKey() const override { return mActor; }
    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderActor(mActor); }

    al::LiveActor* mActor;
};
