#pragma once

#include "al/Library/LiveActor/LiveActor.h"

#include "game/Player/PlayerFunction.h"

#include "custom/game/Player/HackCap/HackCapJointControlKeeper.h"

class PuppetCapActor : public al::LiveActor {
    friend class Ghost;

public:
    PuppetCapActor(const char* name);
    virtual void init(const al::ActorInitInfo&) override;
    virtual void initAfterPlacement() override;
    virtual void control(void) override;
    virtual void movement(void) override;
    virtual void appear() override;
    virtual void kill() override;

    const char* tryGetPuppetCapName();

    void startAction(const char* actName);
    void update();

private:
    HackCapJointControlKeeper* mJointKeeper;
    al::LiveActor* mModelCap = nullptr;
};
