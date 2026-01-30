#pragma once

#include "al/Library/Effect/IUseEffectKeeper.h"

namespace al {
class ActionEffectCtrl {
public:
    void startAction(const char*);
    IUseEffectKeeper* mEffectKeeper;
};
}  // namespace al
