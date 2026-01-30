#pragma once

#include "agl/DrawContext.h"
#include "agl/util.h"

#include <gfx/seadProjection.h>
#include <hostio/seadHostIOEventListener.h>
#include <prim/seadSafeString.h>

#include "gfx/seadCamera.h"
#include "gfx/seadProjection.h"
#include "math/seadVector.h"
#include "types.h"

namespace sead {
struct DirectCamera;
}

namespace agl {
struct TextureSampler;
struct RenderBuffer;

namespace utl {
struct ParameterOfBool {
    char size[0x20];
};

struct ParameterOfInt {
    char size[0x20];
};

struct ParameterOfFloat {
    char size[0x20];
};

struct IParameterObj {
    char size[0x30];
};

struct IParameterIO {
    char size[0x1D0];
};

struct DebugTexturePage {
    char possibleSize[0x240];
};
}  // namespace utl
}  // namespace agl

void initialize_(agl::TextureType, agl::TextureFormat, uint, uint, uint, uint, agl::TextureAttribute, agl::MultiSampleType, bool);

// unk, 1, 33, 256, 256, 1, 1, 1, 0, 1

namespace al {
class CubeMapMgr {
public:
    struct HDREncodeType;
    struct IlluminantInfo;
    struct ReservedUnit;
    struct IlluminantInfoArg;
    struct UnitDebugInfoArg;
    struct Unit;

    struct CreateArg {
        int mReservedUnitCount = 6;
        int mPreviewTexSize = 128;
        agl::TextureFormat mTextureFormat = agl::TextureFormat::R32_G32_B32_A32;
        int field_C = 43;
        int field_10 = 43;
        int field_14;
        int field_18;
        bool field_1C;
    };
    struct UpdateAgent;

    CubeMapMgr();
    ~CubeMapMgr();
    void getCubeMapProjection(sead::PerspectiveProjection*, float, float);
    void getCubeMapCamera(sead::DirectCamera*, unsigned int, const sead::Vector3f&);
    void searchIlluminantEntity(const sead::SafeString&) const;
    void searchIlluminantTexture(const sead::SafeString&) const;
    void getNormalizeCoeff(const al::CubeMapMgr::Unit*, al::CubeMapMgr::HDREncodeType) const;
    void genMessageIlluminantInfo(sead::hostio::Context*, al::CubeMapMgr::IlluminantInfo*, const sead::SafeString&);
    void outputFile(agl::DrawContext*, al::CubeMapMgr::ReservedUnit*, bool) const;
    void pushUpdateRequest(al::CubeMapMgr::ReservedUnit*);
    void freeUnit(al::CubeMapMgr::ReservedUnit*);
    void normalizeName(al::CubeMapMgr::ReservedUnit*) const;
    void clearIlluminant(bool);
    void initialize(const al::CubeMapMgr::CreateArg&, sead::Heap*);
    void allocate();
    void allocate(al::CubeMapMgr::ReservedUnit*);
    void free(al::CubeMapMgr::ReservedUnit*);
    void updateValidateInfo(bool);
    void freeAll();
    void isEnableUnit(al::CubeMapMgr::ReservedUnit*) const;
    void getCubeMap(al::CubeMapMgr::ReservedUnit*) const;
    void getCubeMap(const sead::SafeString&) const;
    void getUnitIndex(al::CubeMapMgr::ReservedUnit*) const;
    void getUnitName(al::CubeMapMgr::ReservedUnit*) const;
    void setUnitName(al::CubeMapMgr::ReservedUnit*, const sead::SafeString&);
    void searchUnitIndex(const sead::SafeString&) const;
    void searchUnit(const sead::SafeString&) const;
    void setUnitPosition(al::CubeMapMgr::ReservedUnit*, const sead::Vector3f&);
    void getUnitPosition(const al::CubeMapMgr::ReservedUnit*) const;
    void setUnitNearFar(al::CubeMapMgr::ReservedUnit*, float, float);
    void getUnitNear(const al::CubeMapMgr::ReservedUnit*) const;
    void getUnitFar(const al::CubeMapMgr::ReservedUnit*) const;
    void setUnitRenderingNumber(al::CubeMapMgr::ReservedUnit*, unsigned int, bool);
    void getUnitRenderingNumber(al::CubeMapMgr::ReservedUnit*) const;
    void setUnitBufferClear(al::CubeMapMgr::ReservedUnit*, bool);
    void setUnitAdhereCamera(al::CubeMapMgr::ReservedUnit*, bool);
    void isUnitRenderingFinished(const al::CubeMapMgr::ReservedUnit*) const;
    void setUnitIlluminantInfo(al::CubeMapMgr::ReservedUnit*, unsigned int, const al::CubeMapMgr::IlluminantInfoArg&);
    void getUnitIlluminantInfo(al::CubeMapMgr::IlluminantInfoArg*, const al::CubeMapMgr::ReservedUnit*, unsigned int) const;
    void setUnitDebugInfo(al::CubeMapMgr::ReservedUnit*, const al::CubeMapMgr::UnitDebugInfoArg&);
    void outputArrayFile(agl::DrawContext*, bool) const;
    void setUnitFileName(al::CubeMapMgr::ReservedUnit*, const sead::SafeString&) const;
    void outputFileUnit(agl::DrawContext*, al::CubeMapMgr::ReservedUnit*) const;
    void outputFileUnit(agl::DrawContext*, al::CubeMapMgr::ReservedUnit*, const sead::SafeString&) const;
    void pushUpdateRequestAll();
    void eraseUpdateRequest(al::CubeMapMgr::ReservedUnit*);
    void eraseUpdateRequestAll();
    void hasUpdateRequest(al::CubeMapMgr::ReservedUnit*);
    void popUpdateRequest(al::CubeMapMgr::UpdateAgent*, bool, unsigned int);
    void rebindIlluminant();
    void addIlluminantEntity(const sead::SafeString&, const sead::Vector3f*);
    void addIlluminantTexture(const sead::SafeString&, const agl::TextureSampler*);
    void drawIlluminant(agl::DrawContext*, const al::CubeMapMgr::ReservedUnit*, const sead::Projection*, const sead::Camera*) const;
    void drawDebug(agl::DrawContext*, const agl::RenderBuffer*, const sead::Projection*, const sead::Camera*, bool) const;
    void postRead_();
    void allocateUnit();
    void genMessage(sead::hostio::Context*);
    void listenPropertyEvent(const sead::hostio::PropertyEvent*);

    void* field_0;
    void* field_8;
    int field_10;
    char gap_14[8];
    char field_1C;
    char gap_1D[443];
    agl::utl::IParameterObj field_1D8;
    int field_208;
    void* field_210;
    sead::ListImpl field_218;
    void* field_22C;
    char* field_238;
    int field_240;
    int field_244;
    void* field_248;
    void* field_250;
    void* field_258;
    int field_260;
    void* field_268;
    void* field_270;
    void* field_278;
    int field_280;
    void* field_288;
    void* field_290;
    int field_298;
    int field_29C;
    void* field_2A0;
    int field_2A8;
    int field_2AC;
    int field_2B0;
    int field_2B4;
    int field_2B8;
    int field_2BC;
    int field_2C0;
    void* field_2C8;
    int field_2D0;
    void* field_2D8;
    int field_2E0;
    int field_2E8;
    void* field_2F0;
    int field_2F8;
    void* field_300;
    void* field_308;
    char* field_310;
    int field_318;
    void* field_31C;
    char gap_324[252];
    void* field_420;
    int field_428;
    int field_42C;
    void* field_430;
    void* field_438;
    int field_440;
    void* field_448;
    void* field_450;
    int field_458;
    void* field_460;
    char gap_468[472];
    agl::utl::DebugTexturePage mDbgTxtPage;
    sead::CriticalSection mCriticalSection1;
    sead::CriticalSection mCriticalSection2;
};
}  // namespace al
