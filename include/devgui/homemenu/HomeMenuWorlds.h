#pragma once

#include "game/System/GameDataHolderAccessor.h"

#include "custom/game/Scene/StageScene.h"

#include "devgui/homemenu/HomeMenuBase.h"

class HomeMenuWorlds : public HomeMenuBase {
public:
    HomeMenuWorlds(DevGuiManager* parent, const char* menuName, bool isDisplayInListByDefault);

    void updateMenuDisplay() override;

private:
    struct KingdomEnglishNameMain {
        const char* mInternal;
        const char* mShort;
        const int clearMainScenario;
        const int endingScenario;
        const int moonRockScenario;
        const size_t mSubNamesCount;
    };

    KingdomEnglishNameMain mainNames[17] = {
        {"CapWorldHomeStage", "Cap", 2, 3, 4, 5},
        {"WaterfallWorldHomeStage", "Waterfall", 7, 3, 4, 5},
        {"SandWorldHomeStage", "Sand", 3, 4, 5, 16},
        {"ForestWorldHomeStage", "Forest", 3, 4, 5, 14},
        {"LakeWorldHomeStage", "Lake", 2, 3, 4, 5},
        {"CloudWorldHomeStage", "Cloud", 2, 3, 4, 2},
        {"ClashWorldHomeStage", "Clash", 2, 3, 4, 3},
        {"CityWorldHomeStage", "City", 4, 5, 8, 17},
        {"SnowWorldHomeStage", "Snow", 2, 3, 4, 11},
        {"SeaWorldHomeStage", "Sea", 2, 3, 4, 10},
        {"LavaWorldHomeStage", "Lava", 3, 7, 8, 12},
        {"BossRaidWorldHomeStage", "Boss", 2, 3, 4, 3},
        {"SkyWorldHomeStage", "Sky", 2, 3, 4, 8},
        {"MoonWorldHomeStage", "Moon", 2, -1, 3, 7},
        {"PeachWorldHomeStage", "Peach", -1, 2, 9, 18},
        {"Special1WorldHomeStage", "Special1", 2, -1, 9, 10},
        {"Special2WorldHomeStage", "Special2", 2, -1, 9, 4},
    };

    inline void drawKingdomPicker(const char* worldName, GameDataHolderAccessor holder);
    inline void drawScenarioPicker(KingdomEnglishNameMain& entry, StageScene* scene);

    inline const char* getScenarioType(KingdomEnglishNameMain& entry, int scenario);

    void warpToStage(GameDataHolderAccessor data, const char* stageName, int scenario);

    int mScenarioPicker = -1;
};
