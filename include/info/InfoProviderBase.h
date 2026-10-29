#pragma once

#include "sead/prim/seadSafeString.h"

#include "devgui/windows/WindowActorBrowse2.h"
#include "imgui.h"
#include "primitives/PrimitiveQueue.h"

class InfoProviderBase {
public:
    struct RenderInfo {
        /// The info-provider itself should draw its visuals
        bool renderSelf = false;
        /// All child info-providers are forced to draw their visuals
        bool forceRenderChildren = false;
    };

    InfoProviderBase(void* key, bool hasVisuals) : mKey(key), mHasVisuals(hasVisuals) {}
    virtual ~InfoProviderBase() {}

    /// Displays data with ImGui
    /// Should **NOT** be used with a stack-allocated object
    void display();
    /// Renders in-world visualizations
    /// Should **NOT** be used with a stack-allocated object
    void render(RenderInfo info);

    /// Provides a human-readable name
    const char* getName() const;

    u32 getGeneration() const { return sead::Mathu::max(mDisplayGeneration, mRenderGeneration); }

    /// Displays data with ImGui
    template <class T, class... Args>
    static void display(T::Key* key, Args&&... args) {
        auto provider = reinterpret_cast<InfoProviderBase*>(getOrCreate<T>(key, std::forward<Args>(args)...));
        if (!provider) {
            ImGui::TextDisabled("<ERROR: Failed to create Info Provider>");
            return;
        }

        if (provider->mDisplayGeneration == getWindow()->getGeneration())
            return;  // Avoid duplicates
        provider->mDisplayGeneration = getWindow()->getGeneration();

        provider->display();
    }

    /// Renders in-world visualizations
    template <class T, class... Args>
    static void render(RenderInfo info, T::Key* key, Args&&... args) {
        auto provider = reinterpret_cast<InfoProviderBase*>(getOrCreate<T>(key, std::forward<Args>(args)...));
        if (!provider)
            return;

        if (provider->mRenderGeneration == getWindow()->getGeneration())
            return;  // Avoid duplicates
        provider->mRenderGeneration = getWindow()->getGeneration();

        provider->render(info);
    }

    /// Retrieves the requested instance of the provider storage, or creates a new one if it does not exist yet
    /// Returns `nullptr` if called multiple times per frame, or if the storage capacity is full
    template <class T, class... Args>
    static T* getOrCreate(T::Key* key, Args&&... args) {
        if (!key)
            return nullptr;

        auto win = getWindow();
        auto provider = win->getProvider(key);
        if (!provider) {
            provider = new (getHeap()) T(key, std::forward<Args>(args)...);
            if (!win->addProvider(key, provider)) {
                delete provider;
                return nullptr;
            }
        }

        return reinterpret_cast<T*>(provider);
    }

protected:
    static constexpr ImGuiTreeNodeFlags cFramedNode = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull;

    virtual void displayImpl() = 0;
    virtual void renderImpl(RenderInfo info) = 0;
    virtual void getNameImpl(sead::BufferedSafeString* buffer) const = 0;

    template <class T>
    T* getKey() {
        return static_cast<T*>(mKey);
    }
    template <class T>
    const T* getKey() const {
        return static_cast<T*>(mKey);
    }

    /// Helper function for demangling a C++ type name without worrying about deallocations.
    /// Returned string is invalidated with the next call to this function.
    static const char* demangleTypeName(const char* mangledName);

    static WindowActorBrowse2* getWindow();
    static PrimitiveQueue* getPrimitiveQueue();
    static sead::Heap* getHeap();

    bool mHasVisuals;
    bool mIsShowVisualsOnHover = true;
    bool mIsAlwaysShowVisuals = false;
    bool mIsForceShowChildren = true;
    bool mIsHovered = false;

private:
    void* mKey;

    u32 mDisplayGeneration = 0;
    u32 mRenderGeneration = 0;
};
