#pragma once

#include "sead/heap/seadHeap.h"

#include "al/Library/Nerve/NerveStateBase.h"

#include "info/InfoProviderBase.h"

class InfoProviderNerveState : public InfoProviderBase {
public:
    InfoProviderNerveState(al::NerveStateBase* state, bool hasVisuals = false) : mState(state), InfoProviderBase(hasVisuals) {}

    static void displayState(al::NerveStateBase* state);
    static void renderState(al::NerveStateBase* state);

protected:
    void displayImpl() override;
    void renderImpl() override;
    void getNameImpl(sead::BufferedSafeString* buffer) const override;

    const void* getKey() const override { return mState; }
    InfoProviderBase* clone(sead::Heap* heap) const override { return new (heap) InfoProviderNerveState(mState); }

    al::NerveStateBase* mState;
};
