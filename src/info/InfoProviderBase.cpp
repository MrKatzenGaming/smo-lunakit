#include "info/InfoProviderBase.h"

#include "sead/basis/seadTypes.h"
#include "sead/heap/seadDisposer.h"
#include "sead/prim/seadSafeString.h"

#include <cxxabi.h>

#include "devgui/DevGuiManager.h"
#include "devgui/categories/actor/CategoryActorInspectors.h"
#include "devgui/windows/WindowActorBrowse2.h"
#include "imgui.h"
#include "primitives/PrimitiveQueue.h"

void InfoProviderBase::display() {
    auto window = getWindow();
    auto inspectors = window->getInspectorsCategory();

    ImVec2 posMin = ImGui::GetCursorScreenPos();

    if (!inspectors->hasProvider(mKey)) {
        if (ImGui::Button("Add to Inspector List"))
            inspectors->addProvider(mKey, this);
    } else {
        if (ImGui::Button("Remove from Inspector List"))
            inspectors->removeProvider(mKey);
    }

    if (mHasVisuals) {
        ImGui::SameLine();
        ImGui::Checkbox("Always Show Visuals", &mIsAlwaysShowVisuals);
    }

    displayImpl();

    ImVec2 posMax = ImGui::GetCursorScreenPos();
    posMax.x += ImGui::GetContentRegionAvail().x;  // Span full width

    mIsHovered = ImGui::IsMouseHoveringRect(posMin, posMax);
}

void InfoProviderBase::render(RenderInfo info) {
    renderImpl(RenderInfo{
        .renderSelf = mHasVisuals && (info.forceRenderChildren || mIsAlwaysShowVisuals || (mIsHovered && mIsShowVisualsOnHover)),
        .forceRenderChildren = info.forceRenderChildren || (mIsAlwaysShowVisuals && mIsForceShowChildren),
    });
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
    return SEAD_INSTANCE(DevGuiManager)->getWindow<WindowActorBrowse2>(windowNameActorBrowse2);
}

PrimitiveQueue* InfoProviderBase::getPrimitiveQueue() {
    return SEAD_INSTANCE(DevGuiManager)->getPrimitiveQueue();
}

sead::Heap* InfoProviderBase::getHeap() {
    return SEAD_INSTANCE(DevGuiManager)->getHeap();
}
