
/*
    - Primitive Queue -

    This system is a simplification of sead's Primitive Renderer class, allowing pushing various objects to
    the list of objects to rendered that frame using primitive shapes.

    For more information about this class, read the docs:
    https://github.com/Amethyst-szs/smo-lunakit/wiki/Code-Documentation#primitives
*/

#pragma once

#include "sead/container/seadPtrArray.h"
#include "sead/heap/seadHeap.h"

#include "primitives/PrimitiveTypes.h"

class PrimitiveQueue {
public:
    PrimitiveQueue(sead::Heap* heap);

    void render();

    void pushPoint(sead::Vector3f trans, float size, sead::Color4f col) {
        auto* entry = new (mHeap) PrimitiveTypePoint(trans, size, col);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushLine(sead::Vector3f point1, sead::Vector3f point2, sead::Color4f col) {
        auto* entry = new (mHeap) PrimitiveTypeLine(point1, point2, col, 0.f, 0.f);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushLine(sead::Vector3f point1, sead::Vector3f point2, sead::Color4f col, float size) {
        auto* entry = new (mHeap) PrimitiveTypeLine(point1, point2, col, size, size);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushLine(sead::Vector3f point1, sead::Vector3f point2, sead::Color4f col, float size1, float size2) {
        auto* entry = new (mHeap) PrimitiveTypeLine(point1, point2, col, size1, size2);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushAxis(sead::Vector3f trans, float size) {
        auto* entry = new (mHeap) PrimitiveTypeAxis(trans, size);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushBox(sead::Vector3f pos, float size, sead::Color4f frame, sead::Color4f fill) {
        auto* entry = new (mHeap) PrimitiveTypeBox(pos, {size, size, size}, frame, fill);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushArea(const char* gName, sead::Color4f frame, sead::Color4f fill) {
        auto* entry = new (mHeap) PrimitiveTypeArea(gName, frame, fill);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushTriangle(al::Triangle tri, sead::Color4f color) {
        auto* entry = new (mHeap) PrimitiveTypeTriangle(tri, color);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushHitSensor(al::LiveActor* actor, HitSensorRenderTypes sensorTypes, float opacity) {
        auto* entry = new (mHeap) PrimitiveTypeHitSensor(actor, sensorTypes, opacity, -1);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushHitSensor(al::LiveActor* actor, HitSensorRenderTypes sensorTypes, float opacity, int sensorIdx) {
        auto* entry = new (mHeap) PrimitiveTypeHitSensor(actor, sensorTypes, opacity, sensorIdx);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushBezierCurve(al::BezierCurve* curve, u32 percision, sead::Color4f color) {
        auto* entry = new (mHeap) PrimitiveTypeBezierCurve(curve, percision, color);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

    void pushRail(const al::Rail* rail, u32 percision, sead::Color4f color) {
        auto* entry = new (mHeap) PrimitiveTypeRail(const_cast<al::Rail*>(rail), percision, color);
        if (!mRenderQueue.pushBack((PrimitiveTypeBase*)entry))
            delete entry;
    }

private:
    void emptyQueue();

    sead::Heap* mHeap = nullptr;
    sead::PtrArray<PrimitiveTypeBase> mRenderQueue;

    const int mMaxQueueSize = 0x400;
};
