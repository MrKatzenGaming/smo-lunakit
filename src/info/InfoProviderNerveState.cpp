#include "info/InfoProviderNerveState.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateBase.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include <typeinfo>

void InfoProviderNerveState::displayImpl() {
    const char* stateName = demangleTypeName(typeid(*mState).name());

    auto nrvKeep = mState->getNerveKeeper();
    if (!nrvKeep)
        return;

    auto nrv = nrvKeep->getCurrentNerve();
    if (!nrv)
        return;

    s32 stateNameLen = strlen(stateName);
    const char* nrvName = demangleTypeName(typeid(*nrv).name()) + strlen("(anonymous namespace)::") + stateNameLen + strlen("Nrv");
    ImGui::Text("Nerve: %s", nrvName);
    ImGui::Text("Step: %d", nrvKeep->getCurrentStep());

    if (!nrvKeep->mStateCtrl || nrvKeep->mStateCtrl->mStateCount == 0)
        return;

    auto currState = nrvKeep->mStateCtrl->mCurrentState;
    if (currState)
        ImGui::Text("State: %s", demangleTypeName(typeid(*currState->state).name()));

    ImGui::Separator();

    for (s32 idx = 0; idx < nrvKeep->mStateCtrl->mStateCount; idx++) {
        auto state = &nrvKeep->mStateCtrl->mStates[idx];
        auto stateNrvKeep = state->state->getNerveKeeper();

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DrawLinesFull;
        if (!stateNrvKeep)
            treeFlags |= ImGuiTreeNodeFlags_Leaf;

        const char* stateName = demangleTypeName(typeid(*state->state).name());

        if (ImGui::TreeNodeEx(state, treeFlags, "%s (%s)", stateName, state->name)) {
            InfoProviderNerveState::displayState(state->state);
            ImGui::TreePop();
        }
    }
}
void InfoProviderNerveState::renderImpl() {
    auto nrvKeep = mState->getNerveKeeper();
    if (!nrvKeep)
        return;

    if (!nrvKeep->mStateCtrl || nrvKeep->mStateCtrl->mStateCount == 0)
        return;

    for (s32 idx = 0; idx < nrvKeep->mStateCtrl->mStateCount; idx++) {
        auto state = &nrvKeep->mStateCtrl->mStates[idx];
        InfoProviderNerveState::renderState(state->state);
    }
}

void InfoProviderNerveState::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("State ('%s')", demangleTypeName(typeid(*mState).name()));
}

void InfoProviderNerveState::displayState(al::NerveStateBase* state) {
    if (false) {
        // TODO: Implement custom state providers
    } else {
        InfoProviderNerveState(state).display();
    }
}
void InfoProviderNerveState::renderState(al::NerveStateBase* state) {
    if (false) {
        // TODO: Implement custom state providers
    } else {
        InfoProviderNerveState(state).render();
    }
}
