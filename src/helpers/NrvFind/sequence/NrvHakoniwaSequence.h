#pragma once

#include "helpers/NrvFind/NrvFindHelper.h"

const uintptr_t nrvHakoniwaSequenceBootLoadData             = 0x01d977e0; // Quickly returns to startup screen, crashes due to heaps
const uintptr_t nrvHakoniwaSequenceChangeLangauge           = 0x01d977d0; // No effect
const uintptr_t nrvHakoniwaSequenceDemoEnding               = 0x01d977f0; // Crash
const uintptr_t nrvHakoniwaSequenceDemoLava                 = 0x01d97800; // Crash
const uintptr_t nrvHakoniwaSequenceDemoOpening              = 0x01d977e8; // Restarts game, crashes due to heaps
const uintptr_t nrvHakoniwaSequenceDemoWorldWarp            = 0x01d977f8; // Crash
const uintptr_t nrvHakoniwaSequenceDestroy                  = 0x01d97808; // Destroys all heaps, reloads, and then reloads previous stage
const uintptr_t nrvHakoniwaSequenceDestroyToChangeLanguage  = 0x01d97840; // Restarts the game, changes lang to JP
const uintptr_t nrvHakoniwaSequenceDestroyToDebugLoad       = 0x01d97848; // Destroys all heaps, reloads, and then reloads previous stage
const uintptr_t nrvHakoniwaSequenceDestroyToDemoLava        = 0x01d97818; // Loads luncheon cutscene, then returns to previous stage
const uintptr_t nrvHakoniwaSequenceDestroyToEnding          = 0x01d97810; // Loads an extremely long time, plays ending cutscene
const uintptr_t nrvHakoniwaSequenceDestroyToMiss            = 0x01d97820; // Cut to black, play coin loss animation, reload stage
const uintptr_t nrvHakoniwaSequenceDestroyToModeChange      = 0x01d97830; // Crash
const uintptr_t nrvHakoniwaSequenceDestroyToNewGame         = 0x01d97838; // Cut to black, restarts game, then crashes on memory exception
const uintptr_t nrvHakoniwaSequenceDestroyToReboot          = 0x01d97828; // Crash
const uintptr_t nrvHakoniwaSequenceFadeToNewGame            = 0x01d97880; // Fades to black, restarts game, then crashes on memory exception
const uintptr_t nrvHakoniwaSequenceLoadStage                = 0x01d97850; // Crash
const uintptr_t nrvHakoniwaSequenceLoadStageAfterOpening    = 0x01d97858; // Crash
const uintptr_t nrvHakoniwaSequenceLoadWorldResource        = 0x00510dd4; // Crash
const uintptr_t nrvHakoniwaSequenceMiss                     = 0x01d97868; // Displays white line on screen, crashes to heaps
const uintptr_t nrvHakoniwaSequenceMissCoinSub              = 0x01d97698; // No effect?
const uintptr_t nrvHakoniwaSequenceMissEnd                  = 0x01d976d8; // No effect?
const uintptr_t nrvHakoniwaSequencePlayStage                = 0x01d977d0; // No effect?
const uintptr_t nrvHakoniwaSequenceWaitLoadData             = 0x01d97718; // No effect?
const uintptr_t nrvHakoniwaSequenceWaitLoadDataModeChange   = 0x01d97758; // No effect?
const uintptr_t nrvHakoniwaSequenceWaitWriteData            = 0x01d97870; // Crash
const uintptr_t nrvHakoniwaSequenceWaitWriteDataModeChange  = 0x01d97878; // Crash