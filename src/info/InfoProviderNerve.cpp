#include "info/InfoProviderNerve.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateBase.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include <typeinfo>

void InfoProviderNerveKeeper::displayImpl() {
    auto nerveKeeper = getKey<Key>();
    auto currNerve = nerveKeeper->getCurrentNerve();
    if (!currNerve)
        return;

    s32 parentClassNameLen = strlen(demangleTypeName(typeid(*nerveKeeper->mParent).name()));
    const char* nrvName = demangleTypeName(typeid(*currNerve).name()) + strlen("(anonymous namespace)::") + parentClassNameLen + strlen("Nrv");
    ImGui::Text("Nerve: %s", nrvName);
    ImGui::Text("Step: %d", nerveKeeper->getCurrentStep());

    if (!nerveKeeper->mStateCtrl || nerveKeeper->mStateCtrl->mStateCount == 0)
        return;

    auto currState = nerveKeeper->mStateCtrl->mCurrentState;
    if (currState)
        ImGui::Text("State: %s", demangleTypeName(typeid(*currState->state).name()));

    ImGui::Separator();

    for (s32 idx = 0; idx < nerveKeeper->mStateCtrl->mStateCount; idx++) {
        auto state = &nerveKeeper->mStateCtrl->mStates[idx];
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
void InfoProviderNerveKeeper::renderImpl(RenderInfo info) {
    auto nerveKeeper = getKey<Key>();
    if (!nerveKeeper->mStateCtrl || nerveKeeper->mStateCtrl->mStateCount == 0)
        return;

    for (s32 idx = 0; idx < nerveKeeper->mStateCtrl->mStateCount; idx++) {
        auto state = &nerveKeeper->mStateCtrl->mStates[idx];
        InfoProviderNerveState::renderState(info, state->state);
    }
}
void InfoProviderNerveKeeper::getNameImpl(sead::BufferedSafeString* buffer) const {
    auto nerveKeeper = getKey<Key>();
    buffer->format("Nerve Keeper ('%s')", demangleTypeName(typeid(*nerveKeeper->mParent).name()));
}

void InfoProviderNerveState::displayImpl() {
    auto state = getKey<Key>();
    auto nerveKeeper = state->getNerveKeeper();
    if (nerveKeeper)
        display<InfoProviderNerveKeeper>(nerveKeeper);
}
void InfoProviderNerveState::renderImpl(RenderInfo info) {
    auto state = getKey<Key>();
    auto nerveKeeper = state->getNerveKeeper();
    if (nerveKeeper)
        render<InfoProviderNerveKeeper>(info, nerveKeeper);
}
void InfoProviderNerveState::getNameImpl(sead::BufferedSafeString* buffer) const {
    auto state = getKey<Key>();
    buffer->format("Nerve State ('%s' on '%s')", demangleTypeName(typeid(*state).name()), demangleTypeName(typeid(*state->mKeeper->mParent).name()));
}

void InfoProviderNerveState::displayState(al::NerveStateBase* state) {
    if (false) {
        // TODO: Implement custom state providers
    } else {
        display<InfoProviderNerveState>(state);
    }
}
void InfoProviderNerveState::renderState(RenderInfo info, al::NerveStateBase* state) {
    if (false) {
        // TODO: Implement custom state providers
    } else {
        render<InfoProviderNerveState>(info, state);
    }
}
