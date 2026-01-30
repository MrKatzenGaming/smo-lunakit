#pragma once

#include "game/System/GameDataHolder.h"

namespace SaveDataAccessFunction {
void startSaveDataInit(GameDataHolder*);
void startSaveDataInitSync(GameDataHolder*);
void startSaveDataLoadFile(GameDataHolder*);
void startSaveDataReadSync(GameDataHolder*);
void startSaveDataReadAll(GameDataHolder*);
void startSaveDataWrite(GameDataHolder*);
void startSaveDataWriteWithWindow(GameDataHolder*);
void startSaveDataCopyWithWindow(GameDataHolder*, int, int);
void startSaveDataDeleteWithWindow(GameDataHolder*, int);
void startSaveDataWriteSync(GameDataHolder*);
bool updateSaveDataAccess(GameDataHolder*, bool);
bool isEnableSave(const GameDataHolder*);
bool isDoneSave(GameDataHolder*);
}  // namespace SaveDataAccessFunction
