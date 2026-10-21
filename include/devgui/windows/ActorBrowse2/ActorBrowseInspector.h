#pragma once

#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/Scene/Scene.h"

#include <cxxabi.h>
#include <typeinfo>

#include "imgui.h"

class WindowActorBrowse2;

class ActorBrowseInspectorBase {
public:
    virtual void display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor);

protected:
    static constexpr ImGuiTreeNodeFlags cTreeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull;

    template <typename T>
    static const char* getClassName(const T* t) {
        static size_t demangleOutputLength = 0;
        static char* demangleOutputBuffer = nullptr;

        s32 demangleStatus;
        demangleOutputBuffer = abi::__cxa_demangle(typeid(*t).name(), demangleOutputBuffer, &demangleOutputLength, &demangleStatus);
        if (demangleStatus != 0)
            return nullptr;

        return demangleOutputBuffer;
    }

    virtual void displayName(al::LiveActor* actor);
    virtual void displayPose(al::ActorPoseKeeperBase* poseKeep);
    virtual void displayFlags(al::LiveActorFlag* flags);
    virtual void displayNerves(al::NerveKeeper* nrvKeep, s32 prefixLen);
    virtual void displaySensors(WindowActorBrowse2* window, al::HitSensorKeeper* sensorKeep);
    virtual void displaySensor(WindowActorBrowse2* window, al::HitSensor* sensor, al::HitSensor* collidingSensor);
};

class ActorBrowseInspectorGrowPlantSeed : public ActorBrowseInspectorBase {
public:
    void display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) override;
};

class ActorBrowseInspectorFukuwaraiFaceParts : public ActorBrowseInspectorBase {
public:
    void display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) override;
};

class ActorBrowseInspectorFukuwaraiWatcher : public ActorBrowseInspectorBase {
public:
    void display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) override;
};
