#pragma once

#include "al/Library/Event/EventFlowExecutor.h"

#include "info/InfoProviderBase.h"

class InfoProviderEventFlow : public InfoProviderBase {
public:
    InfoProviderEventFlow(al::EventFlowExecutor* eventFlow) : mEventFlow(eventFlow), InfoProviderBase(true) {}

protected:
    void displayImpl() override;
    void renderImpl() override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;

    const void* getKey() const override { return mEventFlow; }

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderEventFlow(mEventFlow); }

private:
    al::EventFlowExecutor* mEventFlow;
};

class InfoProviderEventFlowNode : public InfoProviderBase {
public:
    InfoProviderEventFlowNode(al::EventFlowNode* node, bool hasVisuals = false) : mNode(node), InfoProviderBase(hasVisuals) {}

    static void displayNode(al::EventFlowNode* node);
    static void renderNode(al::EventFlowNode* node);

protected:
    void displayImpl() override;
    void renderImpl() override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;

    const void* getKey() const override { return mNode; }

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderEventFlowNode(mNode); }

    al::EventFlowNode* mNode;
};

class InfoProviderEventFlowNodeMessageBalloon : public InfoProviderEventFlowNode {
public:
    static constexpr const char* cClassName = "27EventFlowNodeMessageBalloon";

    InfoProviderEventFlowNodeMessageBalloon(al::EventFlowNode* node) : InfoProviderEventFlowNode(node, true) {}

protected:
    void displayImpl() override;
    void renderImpl() override;

    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderEventFlowNodeMessageBalloon(mNode); }
};
