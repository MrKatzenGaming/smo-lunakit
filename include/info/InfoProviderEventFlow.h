#pragma once

#include "al/Library/Event/EventFlowExecutor.h"

#include "info/InfoProviderBase.h"

class InfoProviderEventFlow : public InfoProviderBase {
public:
    using Key = al::EventFlowExecutor;

    InfoProviderEventFlow(Key* eventFlow) : InfoProviderBase(eventFlow, true) {}

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};

class InfoProviderEventFlowNode : public InfoProviderBase {
public:
    using Key = al::EventFlowNode;

    InfoProviderEventFlowNode(Key* node, bool hasVisuals = false) : InfoProviderBase(node, hasVisuals) {}

    static void displayNode(Key* node);
    static void renderNode(RenderInfo info, Key* node);

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};

class InfoProviderEventFlowNodeMessageBalloon : public InfoProviderEventFlowNode {
public:
    static constexpr const char* cClassName = "27EventFlowNodeMessageBalloon";

    InfoProviderEventFlowNodeMessageBalloon(Key* node) : InfoProviderEventFlowNode(node, true) {}

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
};
