#include "program/SequenceUtil.h"

bool isInStageScene()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        auto gameSeq = (HakoniwaSequence*)curSequence;
        auto curScene = gameSeq->curScene;

        return curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene");
    }
}

bool isInStageScene(StageScene* curScene)
{
    return curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene");
}

al::Sequence* tryGetSequence()
{
    return GameSystemFunction::getGameSystem()->mCurSequence;
}

HakoniwaSequence* tryGetHakoniwaSequence()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        return (HakoniwaSequence*)curSequence;
    }

    return nullptr;
}

al::Scene* tryGetScene()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        auto gameSeq = (HakoniwaSequence*)curSequence;
        auto curScene = gameSeq->curScene;

        if (curScene && curScene->mIsAlive)
            return gameSeq->curScene;
    }

    return nullptr;
}

al::Scene* tryGetScene(HakoniwaSequence* curSequence)
{
    auto curScene = curSequence->curScene;

    if (curScene && curScene->mIsAlive)
        return curScene;

    return nullptr;
}

StageScene* tryGetStageScene()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        auto gameSeq = (HakoniwaSequence*)curSequence;
        auto curScene = gameSeq->curScene;

        if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene"))
            return (StageScene*)gameSeq->curScene;
    }

    return nullptr;
}


StageScene* tryGetStageScene(HakoniwaSequence* curSequence)
{
    auto curScene = curSequence->curScene;

    if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene"))
        return (StageScene*)curScene;

    return nullptr;
}

GameDataHolder* tryGetGameDataHolder()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        HakoniwaSequence* gameSequence = (HakoniwaSequence*)curSequence;
        return gameSequence->mGameDataHolder.mData;
    }

    return nullptr;
}

GameDataHolder* tryGetGameDataHolder(HakoniwaSequence* curSequence)
{
    return curSequence->mGameDataHolder.mData;
}

PlayerActorBase* tryGetPlayerActor()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        auto gameSeq = (HakoniwaSequence*)curSequence;
        auto curScene = gameSeq->curScene;

        if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene")) {
            StageScene* stageScene = (StageScene*)gameSeq->curScene;
            PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);
            return playerBase;
        }
    }

    return nullptr;
}

PlayerActorBase* tryGetPlayerActor(HakoniwaSequence* curSequence)
{
    auto curScene = curSequence->curScene;

    if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene")) {
        StageScene* stageScene = (StageScene*)curSequence->curScene;
        PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);
        return playerBase;
    }

    return nullptr;
}

PlayerActorBase* tryGetPlayerActor(StageScene* scene)
{
    PlayerActorBase *playerBase = rs::getPlayerActor(scene);
    return playerBase;
}

PlayerActorHakoniwa* tryGetPlayerActorHakoniwa()
{
    al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mCurSequence;
    if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
        auto gameSeq = (HakoniwaSequence*)curSequence;
        auto curScene = gameSeq->curScene;

        if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene")) {
            StageScene* stageScene = (StageScene*)gameSeq->curScene;
            PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);

            bool isYukimaru = !playerBase->getPlayerInfo();

            if(!isYukimaru)
                return (PlayerActorHakoniwa*)playerBase;
        }
    }

    return nullptr;
}

PlayerActorHakoniwa* tryGetPlayerActorHakoniwa(HakoniwaSequence* curSequence)
{
    auto curScene = curSequence->curScene;

    if (curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene")) {
        StageScene* stageScene = (StageScene*)curSequence->curScene;
        PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);

        bool isYukimaru = !playerBase->getPlayerInfo();

        if(!isYukimaru)
            return (PlayerActorHakoniwa*)playerBase;
    }

    return nullptr;
}

PlayerActorHakoniwa* tryGetPlayerActorHakoniwa(StageScene* scene)
{
    PlayerActorBase *playerBase = rs::getPlayerActor(scene);
    bool isYukimaru = !playerBase->getPlayerInfo();

    if(!isYukimaru)
        return (PlayerActorHakoniwa*)playerBase;

    return nullptr;
}