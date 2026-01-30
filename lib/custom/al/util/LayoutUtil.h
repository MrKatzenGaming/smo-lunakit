#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>
#include <sead/prim/seadSafeString.h>

#include "Library/Layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"

typedef unsigned short int ushort;

namespace nn::ui2d {
class TextureInfo;
}

namespace al {

class IUseLayout;
class IUseLayoutAction;

char16_t* getPaneStringBuffer(const IUseLayout* lyt, const char* paneName);

void startFreezeActionEnd(IUseLayoutAction*, const char*, const char*);

void startHitReaction(const LayoutActor*, const char*, const char*);

void hidePane(IUseLayout* lyt, const char* paneName);
void hidePaneNoRecursive(IUseLayout* lyt, const char* paneName);
void showPane(IUseLayout* lyt, const char* paneName);
void showPaneNoRecursive(IUseLayout* lyt, const char* paneName);

bool isHidePane(const IUseLayout* lyt, const char* paneName);
bool isActionPlaying(IUseLayoutAction*, const char* action, const char* group);
bool isActionEnd(const IUseLayoutAction*, const char*);
bool isExistPane(IUseLayout* lyt, const char* paneName);

void initLayoutActor(LayoutActor*, const LayoutInitInfo&, const char*, const char*);
void setActionFrameRate(IUseLayoutAction*, float, const char*);
void setPaneString(IUseLayout* layout, const char* paneName, const char16_t* paneValue, ushort);
void setPaneStringFormat(IUseLayout* layout, const char* paneName, const char* format, ...);
void setPaneTexture(IUseLayout*, const char*, const nn::ui2d::TextureInfo*);
void calcLayoutPosFromWorldPos(sead::Vector2f*, const al::IUseCamera*, const sead::Vector3f&);
void calcLayoutPosFromWorldPosSub(sead::Vector2f*, const al::IUseCamera*, const sead::Vector3f&);
void calcPaneTrans(sead::Vector2f*, const IUseLayout*, const char*);
void setLocalTrans(IUseLayout*, const sead::Vector2f&);
void setLocalScale(IUseLayout*, float);

}  // namespace al
