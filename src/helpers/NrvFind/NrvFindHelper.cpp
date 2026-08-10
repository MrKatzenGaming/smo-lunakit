#include "helpers/NrvFind/NrvFindHelper.h"

#include "hk/ro/RoUtil.h"

#include "al/Library/Nerve/IUseNerve.h"
#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveUtil.h"

namespace NrvFindHelper {

const al::Nerve* getNerveAt(uintptr_t offset) {
    return (al::Nerve*)(hk::ro::getMainModule()->range().start() + offset);
}

bool isNerveAt(al::IUseNerve* actor, uintptr_t offset) {
    const al::Nerve* nerve = getNerveAt(offset);
    return al::isNerve(actor, nerve);
}

void setNerveAt(al::IUseNerve* actor, uintptr_t offset) {
    const al::Nerve* nerve = getNerveAt(offset);
    al::setNerve(actor, nerve);
}

}  // namespace NrvFindHelper
