#pragma once

#include "al/Library/Nerve/NerveStateBase.h"

#include "info/InfoProviderBase.h"

class InfoProviderNerveKeeper : public InfoProviderBase {
public:
    using Key = al::NerveKeeper;

    InfoProviderNerveKeeper(Key* nerveKeeper) : InfoProviderBase(nerveKeeper, true) {}

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};

class InfoProviderNerveState : public InfoProviderBase {
public:
    using Key = al::NerveStateBase;

    InfoProviderNerveState(Key* state, bool hasVisuals = false) : InfoProviderBase(state, hasVisuals) {}

    static void displayState(Key* state);
    static void renderState(RenderInfo info, Key* state);

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;
};
