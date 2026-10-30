#include "devgui/categories/actor/CategoryActorInspectors.h"

#include "devgui/categories/CategoryBase.h"
#include "imgui.h"
#include "info/InfoProviderBase.h"

CategoryActorInspectors::CategoryActorInspectors(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryActorInspectors::updateCat() {
    if (mPendingDelete) {
        mProviders.erase(mPendingDelete);
        mPendingDelete = nullptr;
    }

    mProviders.forEach([](auto keyImpl, InfoProviderBase* provider) {
        provider->render(InfoProviderBase::RenderInfo{.renderSelf = true, .forceRenderChildren = false});
    });
}
void CategoryActorInspectors::updateCatDisplay() {
    if (mProviders.mSize == 0)
        ImGui::TextDisabled("No Inspectors added yet!");

    mProviders.forEach([](auto keyImpl, InfoProviderBase* provider) {
        if (ImGui::TreeNodeEx(keyImpl.key, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull, "%s", provider->getName())) {
            provider->display();
            ImGui::TreePop();
        }
    });
}

void CategoryActorInspectors::resetScene() {
    mProviders.clear();
}

void CategoryActorInspectors::addProvider(const void* key, InfoProviderBase* provider) {
    mProviders.insert(key, provider);
}
void CategoryActorInspectors::removeProvider(const void* key) {
    mPendingDelete = key;
}
bool CategoryActorInspectors::hasProvider(const void* key) const {
    return mProviders.find(key) != nullptr;
}
