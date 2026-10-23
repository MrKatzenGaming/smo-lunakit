#include "hk/util/Context.h"

#include "al/Library/Nerve/IUseNerve.h"
#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveUtil.h"

namespace NrvHelper {

template <hk::util::TemplateString Symbol>
const al::Nerve* getNerveSym() {
    const ptr addr = hk::util::lookupSymbol<Symbol>();
    return reinterpret_cast<al::Nerve*>(addr);
}

template <hk::util::TemplateString Symbol>
bool isNerveSym(al::IUseNerve* actor) {
    return al::isNerve(actor, getNerveSym<Symbol>());
}

template <hk::util::TemplateString Symbol>
void setNerveSym(al::IUseNerve* actor) {
    al::setNerve(actor, getNerveSym<Symbol>());
}

}  // namespace NrvHelper
