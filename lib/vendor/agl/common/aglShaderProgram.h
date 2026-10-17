#pragma once

#include <container/seadBuffer.h>
#include <prim/seadSafeString.h>

#include "common/aglDisplayList.h"
#include "common/aglResCommon.h"
#include "common/aglShader.h"
#include "common/aglShaderCompileInfo.h"
#include "common/aglShaderEnum.h"

namespace sead {
class Heap;
}

namespace agl {
class AttributeLocation;
class UniformLocation;
class UniformBlockLocation;
class ShaderStorageBlockLocation;
class SamplerLocation;
class ImageLocation;
class DrawContext;
class ResShaderProgram;
class ResBinaryShaderProgram;
class ResShaderVariation;
class ShaderStorageBlock;
class UniformSymbol;
class UniformBlockSymbol;
class ShaderStorageBlockSymbol;
class SamplerSymbol;
class AttributeSymbol;
class BufferVariableSymbol;

class ShaderProgram {
public:
    class VariationBuffer {
    public:
        VariationBuffer();
        virtual ~VariationBuffer();

        void initialize(s32, sead::Heap* heap);
        void createMacro(s32, const sead::SafeString&, const sead::SafeString&, s32, sead::Heap* heap);
        void setMacroValue(s32, s32, const sead::SafeString&);
        void create(sead::Heap* heap);
        s32 getMacroAndValueArray(s32, const char**, const char**) const;

        s32 searchShaderProgarmIndex(s32, const char* const*, const char* const*, s32) const;
        const char* searchMacroValue(s32, const char*) const;
        const sead::SafeString& searchMacroName(const sead::SafeString&) const;
        const sead::SafeString& searchMacroID(const sead::SafeString&) const;
        s32 searchMacroIndex(const sead::SafeString&) const;

    private:
        u8 _8[0x28];
    };

    struct Variation {
        sead::SafeString _0;
        VariationBuffer buffer;
        u8 _40[0x30];
    };

    static_assert(sizeof(Variation) == 0x70);

    ShaderProgram();
    virtual ~ShaderProgram();

    void cleanUp();
    void destroyLocationBuffers();

    void initializeVariation(const sead::SafeString&, s32, sead::Heap*);
    void createVariationMacro(s32, const sead::SafeString&, const sead::SafeString&, s32, sead::Heap*);
    void setVariationMacroValue(s32, s32, const sead::SafeString&);
    void createVariation(sead::Heap*);

    void initialize(ResShaderProgram, sead::Heap* heap);
    void initialize(ResBinaryShaderProgram, sead::Heap* heap);

    void createAttribute(s32, sead::Heap* heap);
    void setAttributeName(s32, const sead::SafeString&);
    void destroyAttribute();

    void createUniform(s32, sead::Heap* heap);
    void setUniformName(s32, const sead::SafeString&);
    void destroyUniform();

    void createSamplerLocation(s32, sead::Heap* heap);
    void setSamplerLocationName(s32, const sead::SafeString&);
    void destroySamplerLocation();

    void createImageLocation(s32, sead::Heap* heap);
    void setImageLocationName(s32, const sead::SafeString&);
    void destroyImageLocation();

    void createUniformBlock(s32, sead::Heap* heap);
    void setUniformBlockName(s32, const sead::SafeString&);
    void destroyUniformBlock();

    void createShaderStorageBlock(s32, sead::Heap* heap);
    void setShaderStorageBlockName(s32, const sead::SafeString&);
    void destroyShaderStorageBlock();

    const Shader& getShader(ShaderType type) const;

    void updateLocation() const;
    void updateUniformLocation() const;
    void updateUniformBlockLocation() const;
    void updateShaderStorageBlockLocation() const;
    void updateAttributeLocation() const;
    void updateSamplerLocation() const;
    void updateImageLocation() const;

    void activate(DrawContext* context, bool) const;

    void setUpAllVariation(bool);
    void reverseSetUpAllVariation();

    Shader* getShader(ShaderType type);

    void dispatchCompute(DrawContext* context, s32 groupsX, s32 groupsY, s32 groupsZ) const;
    void dispatchComputeIndirect(DrawContext* context, const ShaderStorageBlock&, s32, s64) const;

    u32 calcHash(const void*, u32);

    s32 getUniformSymbol(UniformSymbol*, s32, bool) const;
    s32 getUniformBlockSymbol(UniformBlockSymbol*, s32, bool) const;
    s32 getShaderStorageBlockSymbol(ShaderStorageBlockSymbol*, s32, bool) const;
    s32 getSamplerSymbol(SamplerSymbol*, s32, bool) const;
    s32 getAttributeSymbol(AttributeSymbol*, s32, bool) const;
    s32 getBufferVariableSymbol(BufferVariableSymbol*, s32, bool) const;

    bool calcCompileSource(ShaderType type, sead::BufferedSafeString*, ShaderCompileInfo::Target) const;
    bool calcCompileSourceNoVariation(ShaderType type, sead::BufferedSafeString*, ShaderCompileInfo::Target) const;

    void dump() const;

    bool hasStage(ShaderType type) const;

public:
    void initialize_(const sead::SafeString&, ResArray<ResShaderVariation>, sead::Heap* heap);

    void setUpForVariation_() const;
    void setShaderGX2_(DrawContext* context);
    void validate_() const;
    void forceValidate_(bool) const;
    void compileOffline_() const;

public:
    u64* _8;
    void* _10;
    DisplayList mShaderDisplayList;  // _18
    sead::Buffer<AttributeLocation> mAttributeLocations;
    sead::Buffer<UniformLocation> mUniformLocations;
    sead::Buffer<UniformBlockLocation> mUniformBlockLocations;
    sead::Buffer<ShaderStorageBlockLocation> mShaderStorageBlockLocations;
    sead::Buffer<SamplerLocation> mSamplerLocations;
    sead::Buffer<ImageLocation> mImageLocations;
    VertexShader mVertexShader;
    FragmentShader mFragmentShader;
    GeometryShader mGeometryShader;
    ComputeShader mComputeShader;
    u8 _358[0x418 - 0x358];  // todo; what is here?
    void* _418;
    u32 _420;
};

static_assert(sizeof(ShaderProgram) == 0x428);

}  // namespace agl
