#include "devgui/windows/WindowHitboxes.h"

#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"
#include "al/Library/Sequence/Sequence.h"

#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameSystem.h"

#include "devgui/DevGuiManager.h"
#include "devgui/windows/Hitboxes/CollisionDrawer.h"
#include "imgui.h"

WindowHitboxes::WindowHitboxes(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    CollisionDrawer::createInstance(parent->getHeap());
}

static const al::Scene* getScene(GameSystem* system) {
    const auto* sequence = system->mSequence;
    if (!sequence || !sequence->mIsAlive)
        return nullptr;

    const auto* scene = sequence->mCurrentScene;
    if (!scene || !scene->mIsAlive)
        return nullptr;

    return scene;
}

bool WindowHitboxes::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    ImGui::Checkbox("Enable", &mEnabled);
    ImGui::Separator();

    ImGui::Checkbox("Draw Collision Geometry", &mDrawCollision);

    return true;
}
