#pragma once

#include "sead/prim/seadSafeString.h"

#include "al/Library/Audio/IUseAudioKeeper.h"

namespace al {

bool checkIsPlayingSe(const al::IUseAudioKeeper*, const sead::SafeString&, const char*);

bool isPlayingBgm(const al::IUseAudioKeeper*);

bool isPlayingBgm(const al::IUseAudioKeeper*, const char*);

void stopAllBgm(const al::IUseAudioKeeper*, int);

bool tryStopAllBgm(const al::IUseAudioKeeper*, int);

}  // namespace al
