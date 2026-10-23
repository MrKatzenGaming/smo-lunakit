#pragma once

#include "devgui/windows/WindowActorBrowse2.h"
#include "imgui.h"
#include "prim/seadSafeString.h"
#include "primitives/PrimitiveQueue.h"

class InfoProviderBase {
public:
    InfoProviderBase(bool hasVisuals) : mHasVisuals(hasVisuals) {}
    virtual ~InfoProviderBase() {}

    /// Displays data with ImGui.
    void display();
    /// Provides in-world visualizations, if applicable.
    void render();

    const char* getName() const;

protected:
    static constexpr ImGuiTreeNodeFlags cFramedNode = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull;

    virtual void displayImpl() = 0;
    virtual void renderImpl() = 0;
    virtual void getNameImpl(sead::BufferedSafeString* buffer) const = 0;

    virtual const void* getKey() const = 0;
    virtual InfoProviderBase* clone(sead::Heap* heap) const = 0;

    /// Helper function for demangling a C++ type name without worrying about deallocations.
    /// Returned string is invalidated with the next call to this function.
    static const char* demangleTypeName(const char* mangledName);

    static WindowActorBrowse2* getWindow();
    static PrimitiveQueue* getPrimitiveQueue();
    static sead::Heap* getHeap();

private:
    bool mHasVisuals;
    bool mAlwaysShowVisuals = true;
    bool mIsHovered = false;
    bool mCanSaveData = false;
};
