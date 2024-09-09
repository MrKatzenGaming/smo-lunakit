#include "NrvFindHelper.h"

namespace NrvFindHelper {

const al::Nerve* getNerveAt(uintptr_t offset) {
    return (const al::Nerve*)((((u64)malloc) - 0x00724b94) + offset);
}

bool isNerveAt(al::LiveActor* actor, uintptr_t offset) {
    const al::Nerve* nerve = getNerveAt(offset);
    return al::isNerve(actor, nerve);
}

void setNerveAt(al::LiveActor* actor, uintptr_t offset) {
    const al::Nerve* nerve = getNerveAt(offset);
    al::setNerve(actor, nerve);
}

} // namespace NrvFindHelper