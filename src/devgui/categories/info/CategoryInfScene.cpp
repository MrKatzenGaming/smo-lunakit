#include "devgui/categories/info/CategoryInfScene.h"

#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateCtrl.h"
#include "al/Library/Sequence/Sequence.h"

#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameDataFile.h"

#include <cxxabi.h>
#include <typeinfo>

#include "helpers/GetHelper.h"
#include "imgui.h"

CategoryInfScene::CategoryInfScene(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryInfScene::updateCatDisplay() {
    HakoniwaSequence* seq = tryGetHakoniwaSequence();
    if (!seq) {
        ImGui::Text("Hakoniwa Sequence does not exist!");
        return;
    }

    al::Scene* scene = tryGetSuperScene(seq);

    if (!scene) {
        ImGui::Text("Scene does not exist!");
        return;
    }

    const sead::FixedSafeString<128> stageName = getStageNameFromHakoniwa(seq);
    s32 nextScenarioNum = getScenarioFromHakoniwa(seq);
    GameDataHolder* holder = tryGetGameDataHolder(seq);

    ImGui::Text("Name: %s", stageName.cstr());
    ImGui::Text("Scenario: %u", nextScenarioNum);
    ImGui::Text("Entrance: %s", holder->getGameDataFile()->getPlayerStartId());

    int status;
    al::NerveKeeper* sceneNerveKeeper = scene->getNerveKeeper();
    if (!sceneNerveKeeper)
        return;

    char* sceneName = abi::__cxa_demangle(typeid(*scene).name(), nullptr, nullptr, nullptr);
    if (sceneName) {
        ImGui::Text("Type: %s", sceneName);

        const al::Nerve* sceneNerve = sceneNerveKeeper->getCurrentNerve();
        char* sceneNerveName = abi::__cxa_demangle(typeid(*sceneNerve).name(), nullptr, nullptr, nullptr);
        if (sceneNerveName) {
            auto prefixLen = sceneNerveName[0] == '(' ? strlen("(anonymous namespace)::") : 0;
            ImGui::Text("Nrv: %s", sceneNerveName + prefixLen + strlen(sceneName) + strlen("nrv"));
            ImGui::SameLine();
            ImGui::Text("\tStep: %d", sceneNerveKeeper->mStep);

            if (scene->getNerveKeeper()->mStateCtrl) {
                al::NerveStateCtrl::State* state = scene->getNerveKeeper()->mStateCtrl->findStateInfo(sceneNerve);
                if (state) {
                    const al::Nerve* stateNerve = state->state->getNerveKeeper()->getCurrentNerve();
                    char* stateName = abi::__cxa_demangle(typeid(*state->state).name(), nullptr, nullptr, nullptr);
                    if (stateName) {
                        char* stateNrvName = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, nullptr);
                        if (stateNrvName) {
                            ImGui::Text("State: %s", stateName);
                            ImGui::Text("State Nrv: %s", stateNrvName + strlen("(anonymous namespace)::") + strlen(stateName) + strlen("nrv"));
                            ImGui::SameLine();
                            ImGui::Text("\tStep: %d", state->state->getNerveKeeper()->mStep);
                            free(stateNrvName);
                        }
                        free(stateName);
                    }
                }
            }

            free(sceneNerveName);
        }
        free(sceneName);
    }
}
