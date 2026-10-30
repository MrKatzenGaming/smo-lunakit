#pragma once

#include "al/Library/LiveActor/LiveActor.h"

#include "info/InfoProviderBase.h"

class InfoProviderActor : public InfoProviderBase {
public:
    using Key = al::LiveActor;

    InfoProviderActor(Key* actor) : InfoProviderBase(actor, true) {
        mIsAlwaysShowVisuals = true;
        mIsShowVisualsOnHover = false;
    }

    static void displayActor(al::LiveActor* actor);
    static void renderActor(RenderInfo info, al::LiveActor* actor);

    void displayName();
    void displayPose();
    void displayFlags();
    void displayNerves();
    void displaySensors();

    void renderPose();
    void renderNerves();
    void renderSensors(RenderInfo info);

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};
