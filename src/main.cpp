
/*
    Looking for LunaKit code?
    Head to src/devgui/DevGuiManager.h to get started!
*/

#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/Replace.h"
#include "hk/hook/Trampoline.h"

#include "nn/fs/fs_mount.h"
#include "nn/nifm.h"

#include "sead/basis/seadNew.h"
#include "sead/filedevice/nin/seadNinFileDeviceBaseNin.h"
#include "sead/filedevice/seadFileDeviceMgr.h"
#include "sead/filedevice/seadPath.h"
#include "sead/gfx/seadPrimitiveRenderer.h"
#include "sead/heap/seadExpHeap.h"
#include "sead/prim/seadSafeString.h"
#include "sead/resource/seadArchiveRes.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/LiveActor/ActorInitInfo.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/Memory/HeapUtil.h"
#include "al/Library/Nerve/NerveUtil.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Sequence/Sequence.h"
#include "al/Library/System/GameSystemInfo.h"
#include "al/Project/Draw/GpuPerf.h"
#include "al/Project/File/FileLoader.h"

#include "agl/common/aglDrawContext.h"

#include "game/Player/PlayerFunction.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/Application.h"
#include "game/System/GameDataFile.h"
#include "game/System/GameDataFunction.h"
#include "game/System/GameSystem.h"
#include "game/Util/DemoUtil.h"

#include "custom/game/Scene/StageScene.h"

#include <cstring>
#include <cxxabi.h>

#include "Imgui.hpp"
#include "devgui/DevGuiHooks.h"
#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowInput.h"
#include "devgui/windows/WindowStagePause.h"
#include "ghost/GhostManager.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "smo-tas/STAS.h"

void runTas(al::Scene* scene) {
    auto* tas = STAS::instance();
    if (!tas || al::isNerve(scene, &StageSceneNrvStagePause::sInstance))
        return;

    if (tas->isRunning() && tas->mSpeedupFrame > 0 && !tas->mIsSpeedupDone) {
        if (tas->getCurFrame() == tas->mSpeedupFrame) {
            WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
            if (win)
                win->tryTogglePause();
            tas->mIsSpeedupDone = true;
            return;
        }
    }

    tas->setScene(scene);
    tas->updateNerve();
    auto* ghostManager = GhostManager::instance();
    ghostManager->setScene(scene);
    ghostManager->updateNerve();
    ghostManager->updateGhostNerve();
}

HkTrampoline RunTasHookScene = [](TrampolineStatic(), al::Scene* scene) -> void {
    int status;
    al::NerveKeeper* sceneNerveKeeper = scene->getNerveKeeper();
    if (!sceneNerveKeeper)
        return;

    const al::Nerve* sceneNerve = sceneNerveKeeper->getCurrentNerve();
    char* sceneNerveName = abi::__cxa_demangle(typeid(*sceneNerve).name(), nullptr, nullptr, &status);
    auto prefixLen = sceneNerveName[0] == '(' ? strlen("(anonymous namespace)::") : 0;
    char* nrvName = sceneNerveName + prefixLen;

    if (!al::isEqualString(nrvName, "TitleMenuSceneNrvLoadWait"))
        runTas(scene);

    free(sceneNerveName);
    orig(scene);
};

HkTrampoline SceneEndInitHook = [](TrampolineStatic(), al::Scene* scene, const al::ActorInitInfo& info) -> void {
    GhostManager::instance()->init(info);
    orig(scene, info);
};

HkTrampoline CreateFileDeviceMgr = [](TrampolineStatic(), sead::FileDeviceMgr* thisPtr) -> void {
    orig(thisPtr);
    thisPtr->mMountedSd = nn::fs::MountSdCardForDebug("sd");
    sead::NinFileDeviceBase* sdFileDevice = new sead::NinFileDeviceBase("sd", "sd");
    thisPtr->mount(sdFileDevice);
};

HkTrampoline RedirectFileDevice = [](TrampolineStatic(), sead::FileDeviceMgr* thisPtr, sead::SafeString& path,
                                     sead::BufferedSafeString* pathNoDrive) -> sead::FileDevice* {
    sead::FixedSafeString<32> driveName;
    sead::FileDevice* device;
    if (!sead::Path::getDriveName(&driveName, path)) {
        device = thisPtr->findDevice("sd");
        if (!(device && device->isExistFile(path))) {
            device = thisPtr->getDefaultFileDevice();
            if (!device)
                return nullptr;
        } else {
        }
    } else
        device = thisPtr->findDevice(driveName);

    if (!device)
        return nullptr;

    if (pathNoDrive != nullptr)
        sead::Path::getPathExceptDrive(pathNoDrive, path);

    return device;
};

HkTrampoline FileLoaderLoadArc = [](TrampolineStatic(), al::FileLoader* thisPtr, sead::SafeString& path, const char* ext,
                                    sead::FileDevice* device) -> sead::ArchiveRes* {
    sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

    if (sdFileDevice && sdFileDevice->isExistFile(path))
        device = sdFileDevice;

    return orig(thisPtr, path, ext, device);
};

HkTrampoline FileLoaderIsExistFile = [](TrampolineStatic(), al::FileLoader* thisPtr, sead::SafeString& path, sead::FileDevice* device) -> bool {
    sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

    if (sdFileDevice && sdFileDevice->isExistFile(path))
        device = sdFileDevice;

    return orig(thisPtr, path, device);
};

HkTrampoline FileLoaderIsExistArchive = [](TrampolineStatic(), al::FileLoader* thisPtr, sead::SafeString& path, sead::FileDevice* device) -> bool {
    sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

    if (sdFileDevice && sdFileDevice->isExistFile(path))
        device = sdFileDevice;

    return orig(thisPtr, path, device);
};

void seadPrint(const char* format, ...) {
    va_list args;
    va_start(args, format);
    Logger::log(format, args);
    va_end(args);
}

HkReplaceVarArgs ReplaceSeadPrint = seadPrint;

HkTrampoline DisableSocketInit = [](TrampolineStatic()) -> void {};
static sead::Heap* lkHeap;

void draw() {
    agl::DrawContext* drawContext = Application::instance()->mDrawSystemInfo->drawContext;

    ImGui::NewFrame();
    WindowInput* inp = DevGuiManager::instance()->getWindow<WindowInput>(windowNameInput);
    if (inp) {
        inp->drawInputDisplay();
        inp->drawInputDisplayP2();
    }

    DevGuiManager::instance()->updateDisplay();

    ImGui::Render();
    hk::gfx::ImGuiBackendNvn::instance()->draw(ImGui::GetDrawData(), drawContext->getCommandBuffer()->ToData()->pNvnCommandBuffer);
};

HkTrampoline GameSystemInit = [](TrampolineStatic(), GameSystem* thisPtr) -> void {
    nn::nifm::Initialize();

    // creates heap for LunaKit at 10MB directly off the Stationed heap
    lkHeap = sead::ExpHeap::create(10_MB, "LunaKitHeap", al::getStationedHeap(), 8, sead::Heap::HeapDirection::cHeapDirection_Forward, false);
    lkHeap->enableLock(true);

    Logger::instance().init(lkHeap);
    DisableSocketInit.installAtSym<"_ZN2nn6socket10InitializeEPvmmi">();

    imgui::setup(lkHeap);

    DevGuiManager::createInstance(lkHeap);
    DevGuiManager::instance()->init(lkHeap);

    // create TAS instance on LunaKit heap
    STAS::createInstance(lkHeap);

    // create GhostManager instance on LunaKit heap
    GhostManager::createInstance(lkHeap);

    orig(thisPtr);

    InputHelper::initKBM();
};

HkTrampoline UpdateLunaKit = [](TrampolineStatic(), HakoniwaSequence* thisPtr) -> void {
    orig(thisPtr);
    DevGuiManager::instance()->update();
};

HkTrampoline DrawMainHook = [](TrampolineStatic(), GameSystem* system) -> void {
    orig(system);

    imgui::updateImGuiInput();
    InputHelper::updatePadState();
    draw();
};

HkTrampoline RunTasHook = [](TrampolineStatic(), HakoniwaSequence* seq) -> void {
    al::Scene* scene = tryGetSuperScene(seq);
    if (!scene) {
        orig(seq);
        return;
    }

    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    STAS* tas = STAS::instance();
    if (tas && win && tas->isRunning() && !win->isPausing() && !al::isLessEqualStep(seq, 2) && !tas->mIsSpeedupDone) {
        for (int i = 1; i < tas->mSpeed; i++) {
            if (tas->mIsSpeedupDone)
                break;
            orig(seq);
            if (tryGetScene(seq) && al::isFirstStep(tryGetScene(seq)))
                // seq->drawMain();
                DrawMainHook.orig(GameSystemFunction::getGameSystem());
        }
    }

    orig(seq);
};

extern "C" void hkMain() {
    GameSystemInit.installAtSym<"_ZN10GameSystem4initEv">();
    DrawMainHook.installAtSym<"_ZN10GameSystem8drawMainEv">();
    ReplaceSeadPrint.installAtSym<"_ZN4sead6system5PrintEPKcz">();

    // RandomGetU32.installAtSym<"_ZN4sead6Random6getU32Ev">();

    // DevGui cheats
    DevGuiHooks::exlInstallDevGuiHooks();  // Located in devgui/DevGuiHooks.cpp

    // SD File Redirection
    RedirectFileDevice.installAtSym<"_ZNK4sead13FileDeviceMgr18findDeviceFromPathERKNS_14SafeStringBaseIcEEPNS_22BufferedSafeStringBaseIcEE">();
    FileLoaderLoadArc.installAtSym<"_ZN2al10FileLoader16loadArchiveLocalERKN4sead14SafeStringBaseIcEEPKcPNS1_10FileDeviceE">();
    CreateFileDeviceMgr.installAtSym<"_ZN4sead13FileDeviceMgrC2Ev">();
    FileLoaderIsExistFile.installAtSym<"_ZNK2al10FileLoader11isExistFileERKN4sead14SafeStringBaseIcEEPNS1_10FileDeviceE">();
    FileLoaderIsExistArchive.installAtSym<"_ZNK2al10FileLoader14isExistArchiveERKN4sead14SafeStringBaseIcEEPNS1_10FileDeviceE">();

    // TAS
    RunTasHook.installAtSym<"_ZN16HakoniwaSequence6updateEv">();
    RunTasHookScene.installAtSym<"_ZN2al5Scene8movementEv">();
    SceneEndInitHook.installAtSym<"_ZN2al5Scene7endInitERKNS_13ActorInitInfoE">();
    STAS::installHooks();

    // Debug Text Writer Drawing
    UpdateLunaKit.installAtSym<"_ZNK16HakoniwaSequence8drawMainEv">();

    hk::gfx::ImGuiBackendNvn::instance()->installHooks(false);
    // hk::gfx::DebugRenderer::instance()->installHooks();
}
