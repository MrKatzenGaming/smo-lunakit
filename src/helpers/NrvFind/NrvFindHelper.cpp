#include "NrvFindHelper.h"

const al::Nerve* NrvFindHelper::getNerveAt(uintptr_t offset)
{
    return (const al::Nerve*)((((u64)malloc) - 0x00724b94) + offset);
}

bool NrvFindHelper::isNerveAt(al::IUseNerve* target, uintptr_t offset)
{
    const al::Nerve* nerve = getNerveAt(offset);
    return al::isNerve(target, nerve);
}

void NrvFindHelper::setNerveAt(al::IUseNerve* target, uintptr_t offset)
{
    const al::Nerve* nerve = getNerveAt(offset);
    al::setNerve(target, nerve);
}