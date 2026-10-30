
#include "Library/Nerve/NerveStateBase.h"

#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class CollisionPartsConnector;
class FlashingCtrl;
class HitSensor;
}  // namespace al

class GrowSeedStateWait : public al::ActorStateBase {
public:
    struct Param {
        f32 gravity;
        sead::Vector2f groupVelocityScale;
        sead::Vector2f wallVelocityScale;
    };
    static_assert(sizeof(Param) == 0x14);

    GrowSeedStateWait(const char* name, al::LiveActor* actor, const GrowSeedStateWait::Param* param, sead::Quatf* quat, al::HitSensor* sensor);

    void resetFlashingTimer();

    void appear() override;
    void control() override;

    void exeConnecting();

    void setNerveNoConnect();
    void exeNoConnect();

    bool isConnecting() const;

    void tryStartMove();

public:
    al::CollisionPartsConnector* mConnector;
    sead::Vector3f mHomeTrans;
    al::FlashingCtrl* mFlashingCtrl;

    Param* mParam;
    sead::Quatf* mQuat;
    s32 _50;
    bool _54;

    al::HitSensor* mSensor;
    bool _60;
    bool _61;

    sead::Vector3f mVelocity;
};

static_assert(sizeof(GrowSeedStateWait) == 0x70);
