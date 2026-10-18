#include "devgui/windows/WindowActorBrowse2.h"

#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/LiveActor/LiveActorInfo.h"
#include "al/Library/LiveActor/LiveActorKeeper.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"
#include "al/Library/Sequence/Sequence.h"

#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameSystem.h"

#include <cxxabi.h>
#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "devgui/popups/PopupKeyboard.h"
#include "helpers/GetHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"

WindowActorBrowse2::WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault) {
    mFilterActorGroup = new (mHeap) al::LiveActorGroup("ActorBrowseFilter[LunaKit]", 8192);
}

void WindowActorBrowse2::updateWin() {
    WindowBase::updateWin();
}

bool WindowActorBrowse2::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    al::Scene* scene = tryGetScene();
    if (!scene) {
        ImGui::TextDisabled("No scene!");
        return true;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        ImGui::TextDisabled("No LiveActorKit!");
        return true;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        ImGui::TextDisabled("No LiveActorGroup!");
        return true;
    }

    drawActorList(group);

    return true;
}

static const char* getClassName(const al::LiveActor* actor) {
    static size_t demangleOutputLength = 0;
    static char* demangleOutputBuffer = nullptr;

    s32 demangleStatus;
    demangleOutputBuffer = abi::__cxa_demangle(typeid(*actor).name(), demangleOutputBuffer, &demangleOutputLength, &demangleStatus);
    if (demangleStatus != 0)
        return nullptr;

    return demangleOutputBuffer;
}

void WindowActorBrowse2::drawActor(al::LiveActor* actor, ImGuiListClipper& clipper) {
    auto model = actor->getModelKeeper();
    auto subActors = actor->getSubActorKeeper();

    // Advance the clipper until we have data again or it's done
    while (clipper.UserIndex >= clipper.DisplayEnd)
        if (!clipper.Step())
            break;

    ImGuiID uid = (ImGuiID)(uintptr_t)actor;

    bool drawSubActors = false;
    if (clipper.UserIndex >= clipper.DisplayStart && clipper.UserIndex < clipper.DisplayEnd) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_None;
        treeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        treeFlags |= ImGuiTreeNodeFlags_NavLeftJumpsToParent;
        treeFlags |= ImGuiTreeNodeFlags_SpanFullWidth;
        treeFlags |= ImGuiTreeNodeFlags_DrawLinesToNodes;

        const char* className = getClassName(actor);
        const char* actorName = actor->getName();
        const char* modelName = model ? model->getName() : nullptr;

        if (!className)
            return;

        if (actor == mSelectedActor)
            treeFlags |= ImGuiTreeNodeFlags_Selected;
        if (!subActors || subActors->getCurActorCount() == 0)
            treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::SetNextItemStorageID(uid);

        if (modelName && actorName)
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s (%s) [%s]", className, actorName, modelName);
        else if (actor->getName())
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s (%s)", className, actorName);
        else
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s", className);

        if (!subActors || subActors->getCurActorCount() == 0)
            drawSubActors = false;

        if (ImGui::IsItemFocused())
            mSelectedActor = actor;
    } else {
        drawSubActors = subActors && subActors->getCurActorCount() > 0 && ImGui::TreeNodeGetOpen(uid);
        if (drawSubActors)
            ImGui::TreePush(actor);
    }

    clipper.UserIndex++;

    if (drawSubActors) {
        for (s32 idx = 0; idx < subActors->getCurActorCount(); idx++)
            drawActor(subActors->getActorInfo(idx)->subActor, clipper);

        ImGui::TreePop();
    }
}

void WindowActorBrowse2::drawActorList(al::LiveActorGroup* group) {
    if (ImGui::BeginChild("##List", ImVec2(500, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened)) {
        if (ImGui::InputTextWithHint("##Filter", "Search...", mSearchFilter.InputBuf, sizeof(mSearchFilter.InputBuf),
                                     ImGuiInputTextFlags_EscapeClearsAll)) {
            mSearchText = mSearchFilter.InputBuf;
            mSearchFilter.Build();

            regenerateFilerActorGroup(group);
        }

        ImGui::SameLine();
        if (mSearchFilter.IsActive()) {
            if (ImGui::Button("Clear")) {
                mSearchText = nullptr;
                mSearchFilter.Clear();
            }
        } else if (ImGui::Button("Open Keyboard")) {
            mParent->tryOpenKeyboard(sizeof(mSearchFilter.InputBuf), PopupKeyboardType::KEYTYPE_QWERTY, &mSearchText, &mKeyboardOpen);
        }

        if (mKeyboardOpen && mSearchText != nullptr) {
            strncpy(mSearchFilter.InputBuf, mSearchText, sizeof(mSearchFilter.InputBuf));
            mSearchFilter.Build();

            regenerateFilerActorGroup(group);
        }

        auto shownGroup = mSearchFilter.IsActive() ? mFilterActorGroup : group;

        if (ImGui::BeginTable("##Table", 1, ImGuiTableFlags_RowBg)) {
            ImGuiListClipper clipper;
            clipper.Begin(INT_MAX);

            for (s32 idx = 0; idx < shownGroup->getActorCount(); idx++)
                drawActor(shownGroup->getActor(idx), clipper);

            clipper.SeekCursorForItem(clipper.UserIndex);

            ImGui::EndTable();
        }
    }

    ImGui::EndChild();
}

void WindowActorBrowse2::regenerateFilerActorGroup(const al::LiveActorGroup* group) {
    mFilterActorGroup->removeActorAll();

    for (s32 idx = 0; idx < group->getActorCount(); idx++) {
        auto actor = group->getActor(idx);
        if (checkActorMatchesFilter(actor))
            mFilterActorGroup->registerActor(actor);
    }
}

bool WindowActorBrowse2::checkActorMatchesFilter(const al::LiveActor* actor) const {
    auto model = actor->getModelKeeper();
    auto subActors = actor->getSubActorKeeper();

    const char* className = getClassName(actor);
    const char* actorName = actor->getName();
    const char* modelName = model ? model->getName() : nullptr;

    if (mSearchFilter.PassFilter(className) || mSearchFilter.PassFilter(actorName) || mSearchFilter.PassFilter(modelName))
        return true;

    if (subActors) {
        for (s32 idx = 0; idx < subActors->getCurActorCount(); idx++)
            if (checkActorMatchesFilter(subActors->getActorInfo(idx)->subActor))
                return true;
    }

    return false;
}
