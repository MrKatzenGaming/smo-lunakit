#pragma once

#include "sead/prim/seadSafeString.h"

#include "al/Library/LiveActor/LiveActor.h"

// TODO: Make this actor only created once per puppet, and use SubActorKeeper to create PartsModel actors for each capture

class PuppetHackActor : public al::LiveActor {
public:
    PuppetHackActor(const char* name);
    virtual void init(const al::ActorInitInfo&) override;
    virtual void initAfterPlacement() override;
    virtual void control(void) override;
    virtual void movement(void) override;

    void startAction(const char* actName);

    void startHackAnim(bool isOn);

private:
    sead::FixedSafeString<0x20> mHackType;
};
