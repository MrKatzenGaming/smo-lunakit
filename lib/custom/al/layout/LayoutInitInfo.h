#pragma once

#include "Library/Audio/AudioDirector.h"
#include "Library/Camera/CameraDirector.h"
#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Execute/ExecuteDirector.h"
#include "Library/Layout/LayoutSceneInfo.h"
#include "Library/Message/MessageSystem.h"
#include "Library/Scene/Scene.h"
#include "al/layout/LayoutKit.h"
#include "al/rumble/PadRumbleDirector.h"

namespace al {
class LayoutSystem;

class LayoutInitInfo : public LayoutSceneInfo {
public:
    void init(al::ExecuteDirector*, const al::EffectSystemInfo*, al::SceneObjHolder*, const al::AudioDirector*, al::CameraDirector*,
              const al::LayoutSystem*, const al::MessageSystem*, const al::GamePadSystem*, al::PadRumbleDirector*);

    al::MessageSystem* getMessageSystem(void) const;

    void* qword30;
    void* qword38;
    void* qword40;
    al::ExecuteDirector* mExecuteDirector;
    al::EffectSystemInfo* mEffectSysInfo;
    al::AudioDirector* mAudioDirector;
    al::LayoutSystem* mLayoutSystem;
};

void initLayoutInitInfo(al::LayoutInitInfo*, const al::Scene*, const al::SceneInitInfo&);
void initLayoutInitInfo(al::LayoutInitInfo*, const al::LayoutKit*, al::SceneObjHolder*, const al::AudioDirector*, const al::LayoutSystem*,
                        const al::MessageSystem*, const al::GamePadSystem*);
}  // namespace al
