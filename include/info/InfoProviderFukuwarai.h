#pragma once

#include "info/InfoProviderActor.h"

class InfoProviderFukuwaraiFaceParts : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "18FukuwaraiFaceParts";

    InfoProviderFukuwaraiFaceParts(Key* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
};

class InfoProviderFukuwaraiNpc : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "12FukuwaraiNpc";

    InfoProviderFukuwaraiNpc(Key* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;
    void renderImpl(RenderInfo info) override;
};

class InfoProviderFukuwaraiWatcher : public InfoProviderActor {
public:
    static constexpr const char* cClassName = "16FukuwaraiWatcher";

    InfoProviderFukuwaraiWatcher(Key* actor) : InfoProviderActor(actor) {}

protected:
    void displayImpl() override;
};
