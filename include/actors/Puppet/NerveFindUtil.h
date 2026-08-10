#pragma once

#include "sead/basis/seadTypes.h"

#include "al/Library/LiveActor/LiveActor.h"
#include "al/nerve/Nerve.h"
#include "al/util/NerveUtil.h"

namespace NrvFind {
const al::Nerve* getNerveAt(uintptr_t offset);
bool isNerveAt(al::LiveActor* actor, uintptr_t offset);
void setNerveAt(al::LiveActor* actor, uintptr_t offset);
}  // namespace NrvFind
