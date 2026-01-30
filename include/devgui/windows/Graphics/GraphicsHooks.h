#pragma once

#include "Library/LiveActor/LiveActor.h"

#include "custom/agl/DevTools.h"

#include "gfx/seadViewport.h"

void exlSetupGraphicsHooks();

void ViewportApplyHook(sead::Viewport* viewport, agl::DrawContext* ctx, agl::RenderBuffer* buffer);
void SkyInitHook(al::LiveActor* actor, const al::ActorInitInfo& info, const sead::SafeStringBase<char>& preset, const char* unk);
