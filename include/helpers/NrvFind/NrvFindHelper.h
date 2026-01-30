#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/Nerve.h"

namespace NrvFindHelper {

const al::Nerve* getNerveAt(uintptr_t offset);
bool isNerveAt(al::LiveActor* actor, uintptr_t offset);
void setNerveAt(al::LiveActor* actor, uintptr_t offset);
}  // namespace NrvFindHelper
