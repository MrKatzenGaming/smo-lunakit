#pragma once

#include "sead/gfx/seadFrameBuffer.h"

#include "agl/common/aglDrawContext.h"

// seems to be a static class for managing agl::fctr::GPUStressChecker
namespace al {
class GpuPerf {
public:
    GpuPerf(void);
    void beginPerf(agl::DrawContext*);
    void endPerf(agl::DrawContext*);
    void update(void);
    void drawResult(agl::DrawContext*, const sead::FrameBuffer*) const;

    // this class has no members
};
}  // namespace al
