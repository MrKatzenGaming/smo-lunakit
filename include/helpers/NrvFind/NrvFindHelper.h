#pragma once

#include "al/Library/Nerve/IUseNerve.h"
#include "al/Library/Nerve/Nerve.h"

#include <cstdint>

namespace NrvFindHelper {

const al::Nerve* getNerveAt(uintptr_t offset);
bool isNerveAt(al::IUseNerve* actor, uintptr_t offset);
void setNerveAt(al::IUseNerve* actor, uintptr_t offset);
}  // namespace NrvFindHelper
