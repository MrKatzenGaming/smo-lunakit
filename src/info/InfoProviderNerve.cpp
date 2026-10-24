#include "info/InfoProviderNerve.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateBase.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include <typeinfo>

void InfoProviderNerveKeeper::displayImpl() {
    auto currNerve = mNerveKeeper->getCurrentNerve();
    if (!currNerve)
        return;

    s32 parentClassNameLen = strlen(demangleTypeName(typeid(*mNerveKeeper->mParent).name()));
    const char* nrvName = demangleTypeName(typeid(*currNerve).name()) + strlen("(anonymous namespace)::") + parentClassNameLen + strlen("Nrv");
    ImGui::Text("Nerve: %s", nrvName);
    ImGui::Text("Step: %d", mNerveKeeper->getCurrentStep());

    if (!mNerveKeeper->mStateCtrl || mNerveKeeper->mStateCtrl->mStateCount == 0)
        return;

    auto currState = mNerveKeeper->mStateCtrl->mCurrentState;
    if (currState)
        ImGui::Text("State: %s", demangleTypeName(typeid(*currState->state).name()));

    ImGui::Separator();

    for (s32 idx = 0; idx < mNerveKeeper->mStateCtrl->mStateCount; idx++) {
        auto state = &mNerveKeeper->mStateCtrl->mStates[idx];
        auto stateNerveKeeper = state->state->getNerveKeeper();

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DrawLinesFull;
        if (!stateNerveKeeper)
            treeFlags |= ImGuiTreeNodeFlags_Leaf;

        const char* stateName = demangleTypeName(typeid(*state->state).name());

        if (ImGui::TreeNodeEx(state, treeFlags, "%s (%s)", stateName, state->name)) {
            InfoProviderNerveState::displayState(state->state);
            ImGui::TreePop();
        }
    }
}
void InfoProviderNerveKeeper::renderImpl() {
    if (!mNerveKeeper->mStateCtrl || mNerveKeeper->mStateCtrl->mStateCount == 0)
        return;

    for (s32 idx = 0; idx < mNerveKeeper->mStateCtrl->mStateCount; idx++) {
        auto state = &mNerveKeeper->mStateCtrl->mStates[idx];
        InfoProviderNerveState::renderState(state->state);
    }
}
void InfoProviderNerveKeeper::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Nerve Keeper ('%s')", demangleTypeName(typeid(*mNerveKeeper->mParent).name()));
}

void InfoProviderNerveState::displayImpl() {
    auto nerveKeeper = mState->getNerveKeeper();
    if (nerveKeeper)
        InfoProviderNerveKeeper(nerveKeeper).display();
}
void InfoProviderNerveState::renderImpl() {
    auto nerveKeeper = mState->getNerveKeeper();
    if (nerveKeeper)
        InfoProviderNerveKeeper(nerveKeeper).render();
}
void InfoProviderNerveState::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Nerve State ('%s' on '%s')", demangleTypeName(typeid(*mState).name()),
                   demangleTypeName(typeid(*mState->mKeeper->mParent).name()));
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
