
/*
    Looking for LunaKit code?
    Head to src/devgui/DevGuiManager.h to get started!
*/

#include "hk/gfx/ImGuiBackendNvn.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "hk/svc/api.h"
#include "hk/svc/types.h"

#include "nn/diag.h"
#include "nn/fs/fs_mount.h"
#include "nn/nifm.h"

#include "sead/basis/seadNew.h"
#include "sead/filedevice/nin/seadNinFileDeviceBaseNin.h"
#include "sead/filedevice/seadFileDeviceMgr.h"
#include "sead/filedevice/seadPath.h"
#include "sead/framework/nx/seadGameFrameworkNx.h"
#include "sead/gfx/seadPrimitiveRenderer.h"
#include "sead/heap/seadExpHeap.h"
#include "sead/prim/seadSafeString.h"
#include "sead/random/seadGlobalRandom.h"
#include "sead/random/seadRandom.h"
#include "sead/resource/seadArchiveRes.h"

#include "Project/Draw/GpuPerf.h"
#include "al/Library/LiveActor/ActorInitInfo.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/Memory/HeapUtil.h"
#include "al/Library/Nerve/NerveUtil.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/System/GameSystemInfo.h"
#include "al/Project/File/FileLoader.h"

#include "agl/common/aglDrawContext.h"

#include "game/Player/PlayerFunction.h"
#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/Application.h"
#include "game/System/GameDataFile.h"
#include "game/System/GameDataFunction.h"
#include "game/System/GameSystem.h"

#include "custom/game/Scene/StageScene.h"

#include <cstring>

#include "Imgui.hpp"
#include "Util/DemoUtil.h"
#include "devgui/DevGuiHooks.h"
#include "devgui/DevGuiManager.h"
#include "devgui/windows/StagePause/WindowStagePause.h"
#include "devgui/windows/input/WindowInput.h"
#include "ghost/GhostManager.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "logger/LoadLogger.hpp"
#include "logger/Logger.hpp"
#include "smo-tas/TAS.h"
#include "stage-pause/StageSceneStateStagePause.h"

void getSymbolName(char* buffer, uintptr_t address) {
    nn::diag::GetSymbolName(buffer, 0x100, address);
}

struct stack_frame {
    stack_frame* fp;
    size_t lr;
};

HkTrampoline<u32, sead::Random*> RandomGetU32 = hk::hook::trampoline([](sead::Random* random) -> u32 {
    if (random != sead::GlobalRandom::instance())
        return RandomGetU32.orig(random);
    register stack_frame* framePointer asm("x29");
    register uintptr_t startingLink asm("x30");
    stack_frame* fp = framePointer;
    uintptr_t lr = startingLink - hk::ro::getMainModule()->range().start();
    while (fp) {
        hk::svc::MemoryInfo memInfo;
        u32 pageInfo;
        if (hk::svc::QueryMemory(&memInfo, &pageInfo, (uintptr_t)fp).failed() || (memInfo.permission & hk::svc::MemoryPermission_Read) == 0)
            break;

        lr += fp->lr - hk::ro::getMainModule()->range().start();
        fp = fp->fp;
    }
    return RandomGetU32.orig(random);
});

void runTas(al::Scene* scene) {
    if (al::isNerve(scene, &StageSceneNrvStagePause::sInstance))
        return;

    auto* tas = TAS::instance();

    if (tas->isRunning() && tas->getSpeedUntilFrame() > 0 && !tas->hasSpeedUntilFrame()) {
        if (tas->getFrameIndex() == tas->getSpeedUntilFrame()) {
            (DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause))->tryTogglePause();
            tas->setHasSpeedUntilFrame(true);
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

HkTrampoline<void, HakoniwaSequence*> RunTasHook = hk::hook::trampoline([](HakoniwaSequence* seq) -> void {
    al::Scene* scene = tryGetScene(seq);
    if (!scene) {
        RunTasHook.orig(seq);
        return;
    }

    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    TAS* tas = TAS::instance();
    if (tas->isRunning() && !win->isPausing() && tas->getFrameIndex() != 0) {
        for (int i = 1; i < tas->getSpeed(); i++) {
            scene = tryGetScene(seq);
            if (scene)
                runTas(scene);

            RunTasHook.orig(seq);
        }
    }
    if (scene)
        runTas(scene);
    RunTasHook.orig(seq);
});

HkTrampoline<void, al::Scene*, const al::ActorInitInfo&> SceneEndInitHook =
    hk::hook::trampoline([](al::Scene* scene, const al::ActorInitInfo& info) -> void {
        GhostManager::instance()->init(info);
        SceneEndInitHook.orig(scene, info);
    });

HkTrampoline<void, sead::FileDeviceMgr*> CreateFileDeviceMgr = hk::hook::trampoline([](sead::FileDeviceMgr* thisPtr) -> void {
    CreateFileDeviceMgr.orig(thisPtr);
    thisPtr->mMountedSd = nn::fs::MountSdCardForDebug("sd");
    sead::NinFileDeviceBase* sdFileDevice = new sead::NinFileDeviceBase("sd", "sd");
    thisPtr->mount(sdFileDevice);
});

HkTrampoline<sead::FileDevice*, sead::FileDeviceMgr*, sead::SafeString&, sead::BufferedSafeString*> RedirectFileDevice =
    hk::hook::trampoline([](sead::FileDeviceMgr* thisPtr, sead::SafeString& path, sead::BufferedSafeString* pathNoDrive) -> sead::FileDevice* {
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
    });

HkTrampoline<sead::ArchiveRes*, al::FileLoader*, sead::SafeString&, const char*, sead::FileDevice*> FileLoaderLoadArc =
    hk::hook::trampoline([](al::FileLoader* thisPtr, sead::SafeString& path, const char* ext, sead::FileDevice* device) -> sead::ArchiveRes* {
        ResourceLoadLogger* log = ResourceLoadLogger::instance();

        if (log)
            log->pushTextToVector(path.cstr());

        sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

        if (sdFileDevice && sdFileDevice->isExistFile(path))
            device = sdFileDevice;

        return FileLoaderLoadArc.orig(thisPtr, path, ext, device);
    });

HkTrampoline<bool, al::FileLoader*, sead::SafeString&, sead::FileDevice*> FileLoaderIsExistFile =
    hk::hook::trampoline([](al::FileLoader* thisPtr, sead::SafeString& path, sead::FileDevice* device) -> bool {
        ResourceLoadLogger* log = ResourceLoadLogger::instance();

        if (log)
            log->pushTextToVector(path.cstr());

        sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

        if (sdFileDevice && sdFileDevice->isExistFile(path))
            device = sdFileDevice;

        return FileLoaderIsExistFile.orig(thisPtr, path, device);
    });

HkTrampoline<bool, al::FileLoader*, sead::SafeString&, sead::FileDevice*> FileLoaderIsExistArchive =
    hk::hook::trampoline([](al::FileLoader* thisPtr, sead::SafeString& path, sead::FileDevice* device) -> bool {
        sead::FileDevice* sdFileDevice = sead::FileDeviceMgr::instance()->findDevice("sd");

        if (sdFileDevice && sdFileDevice->isExistFile(path))
            device = sdFileDevice;

        return FileLoaderIsExistArchive.orig(thisPtr, path, device);
    });

// HkTrampolineVarArgs<void, const char*> ReplaceSeadPrint = hk::hook::trampoline([](const char* format, ...) -> void {
//     va_list args;
//     va_start(args, format);
//     Logger::log(format, args);
//     va_end(args);
// });

HkTrampoline<void> DisableSocketInit = hk::hook::trampoline([]() -> void {});
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

HkTrampoline<void, GameSystem*> GameSystemInit = hk::hook::trampoline([](GameSystem* thisPtr) -> void {
    nn::nifm::Initialize();

    // creates heap for LunaKit at 9MB directly off the Stationed heap
    lkHeap = sead::ExpHeap::create(9_MB, "LunaKitHeap", al::getStationedHeap(), 8, sead::Heap::HeapDirection::cHeapDirection_Forward, false);
    lkHeap->enableLock(true);

    imgui::setup(lkHeap);

    Logger::instance().init(lkHeap);
    DisableSocketInit.installAtSym<"_ZN2nn6socket10InitializeEPvmmi">();

    ResourceLoadLogger::createInstance(lkHeap);
    ResourceLoadLogger::instance()->init(lkHeap);

    DevGuiManager::createInstance(lkHeap);
    DevGuiManager::instance()->init(lkHeap);

    // create TAS instance on LunaKit heap
    TAS::createInstance(lkHeap);

    // create GhostManager instance on LunaKit heap
    GhostManager::createInstance(lkHeap);

    GameSystemInit.orig(thisPtr);

    InputHelper::initKBM();
});

HkTrampoline<void, HakoniwaSequence*> UpdateLunaKit = hk::hook::trampoline([](HakoniwaSequence* thisPtr) -> void {
    UpdateLunaKit.orig(thisPtr);
    DevGuiManager::instance()->update();
});

HkTrampoline<void, sead::GameFrameworkNx*> DrawMainHook = hk::hook::trampoline([](sead::GameFrameworkNx* system) -> void {
    DrawMainHook.orig(system);

    imgui::updateImGuiInput();
    InputHelper::updatePadState();
    draw();
});

extern "C" void hkMain() {
    GameSystemInit.installAtSym<"_ZN10GameSystem4initEv">();
    DrawMainHook.installAtSym<"_ZN10GameSystem8drawMainEv">();
    // ReplaceSeadPrint::InstallAtSymbol("_ZN4sead6system5PrintEPKcz");

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
    SceneEndInitHook.installAtSym<"_ZN2al5Scene7endInitERKNS_13ActorInitInfoE">();

    // Debug Text Writer Drawing
    UpdateLunaKit.installAtSym<"_ZNK16HakoniwaSequence8drawMainEv">();

    // ImGui Hooks

    hk::gfx::ImGuiBackendNvn::instance()->installHooks(false);
    // hk::gfx::DebugRenderer::instance()->installHooks();
}
