#include "info/InfoProviderBase.h"

#include "sead/basis/seadTypes.h"

#include <cxxabi.h>

#include "devgui/DevGuiManager.h"
#include "devgui/categories/actor/CategoryActorInspectors.h"
#include "devgui/windows/WindowActorBrowse2.h"
#include "heap/seadDisposer.h"
#include "imgui.h"
#include "prim/seadSafeString.h"
#include "primitives/PrimitiveQueue.h"

void InfoProviderBase::display() {
    auto key = getKey();

    auto window = getWindow();
    auto inspectors = window->getInspectorsCategory();

    ImVec2 posMin = ImGui::GetCursorScreenPos();

    auto provider = inspectors->getProvider(key);
    if (provider) {
        if (ImGui::Button("Remove from Inspector List")) {
            provider = this;
            inspectors->removeProvider(key);
        }
    } else {
        if (ImGui::Button("Add to Inspector List")) {
            provider = clone(getHeap());
            provider->mCanSaveData = true;
            inspectors->addProvider(key, provider);
        } else {
            provider = this;
        }
    }

    if (provider->mHasVisuals && provider->mCanSaveData) {
        ImGui::SameLine();
        ImGui::Checkbox("Always Show Visuals", &provider->mAlwaysShowVisuals);
    }

    provider->displayImpl();

    ImVec2 posMax = ImGui::GetCursorScreenPos();
    posMax.x += ImGui::GetContentRegionAvail().x;  // Span full width

    provider->mIsHovered = ImGui::IsMouseHoveringRect(posMin, posMax);
}
void InfoProviderBase::render() {
    auto key = getKey();

    auto window = getWindow();
    auto inspectors = window->getInspectorsCategory();

    auto provider = inspectors->getProvider(key);
    if (!provider)
        provider = this;

    if (!provider->mHasVisuals || !(provider->mAlwaysShowVisuals || provider->mIsHovered))
        return;

    provider->renderImpl();
}

const char* InfoProviderBase::getName() const {
    static sead::FixedSafeString<128> buffer;
    getNameImpl(&buffer);
    return buffer.cstr();
}

const char* InfoProviderBase::demangleTypeName(const char* mangledName) {
    static size_t length = 0;
    static char* buffer = nullptr;

    s32 demangleStatus;
    buffer = abi::__cxa_demangle(mangledName, buffer, &length, &demangleStatus);
    if (demangleStatus != 0)
        return nullptr;

    return buffer;
}

WindowActorBrowse2* InfoProviderBase::getWindow() {
    return SEAD_INSTANCE(DevGuiManager)->getWindow<WindowActorBrowse2>(windowNameactorBrowse2);
}

PrimitiveQueue* InfoProviderBase::getPrimitiveQueue() {
    return SEAD_INSTANCE(DevGuiManager)->getPrimitiveQueue();
}

sead::Heap* InfoProviderBase::getHeap() {
    return SEAD_INSTANCE(DevGuiManager)->getHeap();
}
