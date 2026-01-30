#include "ghost/Ghost.h"

#include "al/Library/LiveActor/ActorActionFunction.h"
#include "al/Library/LiveActor/ActorFlagFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/LiveActor/LiveActorFunction.h"
#include "al/Library/LiveActor/LiveActorKeeper.h"
#include "al/Library/Nerve/NerveSetupUtil.h"
#include "al/Library/Nerve/NerveUtil.h"

#include "actors/Puppet/PuppetActor.h"
#include "ghost/PlayerAnims.h"

namespace {
NERVE_IMPL(Ghost, Play);
NERVE_IMPL(Ghost, Wait);
NERVES_MAKE_STRUCT(Ghost, Play, Wait)

}  // namespace

Ghost::Ghost() : al::NerveExecutor("Ghost") {
    initNerve(&NrvGhost.Wait, 0);
}

Ghost::~Ghost() = default;

void Ghost::init(const al::ActorInitInfo& info) {
    mPuppet = new PuppetActor("Actor", this);
    mPuppet->init(info);
    mPuppet->mIs2DModel = false;
    if (isRunning())
        mPuppet->appear();
}

void Ghost::removePuppet() {
    mPuppet = nullptr;
}

void Ghost::startReplay(ReplayFrame* frames, s32 frameCount) {
    mFrames = frames;
    mFrameCount = frameCount;
    al::setNerve(this, &NrvGhost.Play);
    if (mPuppet)
        mPuppet->appear();
}

void Ghost::endReplay() {
    delete[] mFrames;
    mFrameCount = 0;
    al::setNerve(this, &NrvGhost.Wait);
    if (mPuppet) {
        mPuppet->kill();
        mPuppet->mPuppetCap->kill();
    }
}

bool Ghost::isRunning() {
    return al::isNerve(this, &NrvGhost.Play);
}

void Ghost::exeWait() {}

void Ghost::exePlay() {
    if (!mPuppet)
        return;
    int step = al::getNerveStep(this);
    if (step >= mFrameCount) {
        endReplay();
        return;
    }

    ReplayFrame& curFrame = mFrames[step];
    PuppetCapActor* cap = mPuppet->mPuppetCap;
    al::LiveActor* curModel = mPuppet->getCurrentModel();

    // cappy visibility updating
    if (curFrame.isCapVisible != al::isAlive(cap)) {
        if (curFrame.isCapVisible) {
            cap->makeActorAlive();  // start vis action?
        } else {
            cap->makeActorDead();
            al::LiveActor* headModel = al::getSubActor(curModel, "頭");
            if (headModel)
                al::startVisAnimForAction(headModel, "CapOn");
        }
    }

    if (mPuppet->mIs2DModel != curFrame.is2D) {
        mPuppet->changeModel(mPuppet->mIs2DModel ? "Normal" : "Normal2D");
        mPuppet->mIs2DModel = curFrame.is2D;
    }

    const char* animStr = PlayerAnims::FindStr(curFrame.playerAnim);
    if (!al::isActionPlaying(curModel, animStr) || al::isActionEnd(curModel))
        mPuppet->startAction(animStr);

    const char* capAnimStr = PlayerAnims::FindStr(curFrame.capAnim);
    if (capAnimStr && (!al::isActionPlaying(cap, capAnimStr) || al::isActionEnd(cap)))
        cap->startAction(capAnimStr);

    if (mPuppet->isNeedBlending())
        for (int i = 0; i < 6; i++)
            mPuppet->setBlendWeight(i, curFrame.blendWeights[i]);

    al::setTrans(mPuppet, curFrame.pTrans);
    al::setQuat(mPuppet, curFrame.pRotation);
    al::setTrans(cap, curFrame.cTrans);
    cap->mJointKeeper->mJointRot.x = curFrame.cJoint.x;
    cap->mJointKeeper->mJointRot.y = curFrame.cJoint.y;
    cap->mJointKeeper->mJointRot.z = curFrame.cJoint.z;
    cap->mJointKeeper->mSkew = curFrame.cSkew;
    al::setQuat(cap, curFrame.cRotation);
}
