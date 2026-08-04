#pragma once

#include "Library/Scene/Scene.h"
#include "Library/Sequence/Sequence.h"

#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameDataHolder.h"

#include "custom/game/Scene/StageScene.h"

// Is checks
bool isInScene();
bool isInScene(al::Scene* scene);
bool isInStageScene();
bool isInStageScene(al::Scene* scene);
bool tryReloadStage();

// Safe getters
al::Sequence* tryGetSequence();
HakoniwaSequence* tryGetHakoniwaSequence();
al::Scene* tryGetSuperScene(HakoniwaSequence* seq);

al::Scene* tryGetScene();
al::Scene* tryGetScene(al::Sequence* curSequence);
al::Scene* tryGetScene(HakoniwaSequence* curSequence);
StageScene* tryGetStageScene();
StageScene* tryGetStageScene(HakoniwaSequence* curSequence);

GameDataHolder* tryGetGameDataHolder();
GameDataHolder* tryGetGameDataHolder(HakoniwaSequence* curSequence);
GameDataHolder* tryGetGameDataHolder(StageScene* scene);
GameDataHolderAccessor* tryGetGameDataHolderAccess();
GameDataHolderAccessor* tryGetGameDataHolderAccess(HakoniwaSequence* curSequence);
// GameDataHolderAccessor* tryGetGameDataHolderAccess(al::Scene* scene);

const sead::FixedSafeString<128> getStageNameFromHakoniwa(HakoniwaSequence* seq);
s32 getScenarioFromHakoniwa(HakoniwaSequence* seq);

PlayerActorBase* tryGetPlayerActor();
PlayerActorBase* tryGetPlayerActor(HakoniwaSequence* curSequence);
PlayerActorBase* tryGetPlayerActor(al::Scene* scene);
PlayerActorHakoniwa* tryGetPlayerActorHakoniwa();
PlayerActorHakoniwa* tryGetPlayerActorHakoniwa(HakoniwaSequence* curSequence);
PlayerActorHakoniwa* tryGetPlayerActorHakoniwa(al::Scene* scene);
