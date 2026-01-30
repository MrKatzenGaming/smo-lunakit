#pragma once

#ifndef SEAD_GRAPHICS_CONTEXT_H_
#define SEAD_GRAPHICS_CONTEXT_H_

#include "sead/gfx/seadColor.h"
#include "sead/gfx/seadDrawContext.h"

// #include "custom/sead/gfx/seadGraphics.h"
#include "nn/gfx/gfx_Enum.h"
#include "nvn/nvn_Cpp.h"

enum CullingMode { cCullingMode_Front = 0, cCullingMode_Back = 1, cCullingMode_None = 2, cCullingMode_All = 3 };

namespace sead {

class GraphicsContext {
public:
    GraphicsContext();
    virtual ~GraphicsContext() {};
    void apply(sead::DrawContext*) const;
    void applyAlphaTest(sead::DrawContext*) const;
    void applyDepthAndStencilTest(sead::DrawContext*) const;
    void applyColorMask(sead::DrawContext*) const;
    void applyBlendAndFastZ(sead::DrawContext*) const;
    void applyBlendConstantColor(sead::DrawContext*) const;
    void applyCullingAndPolygonModeAndPolygonOffset(sead::DrawContext*) const;

    void setDepthEnable(bool test_enable, bool write_enable) {
        setDepthTestEnable(test_enable);
        setDepthWriteEnable(write_enable);
    }

    void setDepthTestEnable(bool test_enable) { mDepthTestEnable = test_enable; }

    void setDepthWriteEnable(bool write_enable) { mDepthWriteEnable = write_enable; }

    void setDepthFunc(nvn::DepthFunc func) { mDepthFunc = func; }

    void setCullingMode(CullingMode mode) { mCullingMode = mode; }

    void setBlendEnable(bool blend) { mBlendEnable = blend; }

    void setBlendFactor(nn::gfx::BlendFactor src_factor, nn::gfx::BlendFactor dst_factor) {
        setBlendFactorSrc(src_factor);
        setBlendFactorDst(dst_factor);
    }

    void setBlendFactorSeparate(nn::gfx::BlendFactor src_factor_rgb, nn::gfx::BlendFactor dst_factor_rgb, nn::gfx::BlendFactor src_factor_a,
                                nn::gfx::BlendFactor dst_factor_a) {
        setBlendFactorSrcRGB(src_factor_rgb);
        setBlendFactorDstRGB(dst_factor_rgb);
        setBlendFactorSrcAlpha(src_factor_a);
        setBlendFactorDstAlpha(dst_factor_a);
    }

    void setBlendFactorSrc(nn::gfx::BlendFactor factor) {
        setBlendFactorSrcRGB(factor);
        setBlendFactorSrcAlpha(factor);
    }

    void setBlendFactorDst(nn::gfx::BlendFactor factor) {
        setBlendFactorDstRGB(factor);
        setBlendFactorDstAlpha(factor);
    }

    void setBlendFactorSrcRGB(nn::gfx::BlendFactor factor) { mBlendFactorSrcRGB = factor; }

    void setBlendFactorSrcAlpha(nn::gfx::BlendFactor factor) { mBlendFactorSrcA = factor; }

    void setBlendFactorDstRGB(nn::gfx::BlendFactor factor) { mBlendFactorDstRGB = factor; }

    void setBlendFactorDstAlpha(nn::gfx::BlendFactor factor) { mBlendFactorDstA = factor; }

    void setBlendEquation(nvn::BlendEquation equation) {
        setBlendEquationRGB(equation);
        setBlendEquationAlpha(equation);
    }

    void setBlendEquationSeparate(nvn::BlendEquation equation_rgb, nvn::BlendEquation equation_a) {
        setBlendEquationRGB(equation_rgb);
        setBlendEquationAlpha(equation_a);
    }

    void setBlendEquationRGB(nvn::BlendEquation equation) { mBlendEquationRGB = equation; }

    void setBlendEquationAlpha(nvn::BlendEquation equation) { mBlendEquationA = equation; }

    void setBlendConstantColor(const Color4f& color) { mBlendConstantColor = color; }

    void setAlphaTestEnable(bool enable) { mAlphaTestEnable = enable; }

    void setAlphaTestFunc(nvn::AlphaFunc func, f32 ref) {
        mAlphaTestFunc = func;
        mAlphaTestRef = ref;
    }

    void setColorMask(bool r, bool g, bool b, bool a) {
        mColorMaskR = r;
        mColorMaskG = g;
        mColorMaskB = b;
        mColorMaskA = a;
    }

    void setStencilTestEnable(bool enable) { mStencilTestEnable = enable; }

    void setStencilTestFunc(nvn::StencilFunc func, s32 ref, u32 mask) {
        mStencilTestFunc = func;
        mStencilTestRef = ref;
        mStencilTestMask = mask;
    }

    void setStencilTestOp(nvn::StencilOp fail, nvn::StencilOp zfail, nvn::StencilOp zpass) {
        mStencilOpFail = fail;
        mStencilOpZFail = zfail;
        mStencilOpZPass = zpass;
    }

    void setPolygonMode(nvn::PolygonMode front, nvn::PolygonMode back) {
#ifdef cafe
        mPolygonModeFront = front;
        mPolygonModeBack = back;
#endif  // cafe
    }

    void setPolygonOffsetEnable(bool fill_front_enable, bool fill_back_enable, bool point_line_enable) {
#ifdef cafe
        mPolygonOffsetFrontEnable = fill_front_enable;
        mPolygonOffsetBackEnable = fill_back_enable;
        mPolygonOffsetPointLineEnable = point_line_enable;
#endif  // cafe
    }

    bool getDepthTestEnable() const { return mDepthTestEnable; }

    bool getDepthWriteEnable() const { return mDepthWriteEnable; }

    nvn::DepthFunc getDepthFunc() const { return mDepthFunc; }

    CullingMode getCullingMode() const { return mCullingMode; }

    bool getBlendEnable() const { return mBlendEnable; }

    nn::gfx::BlendFactor getBlendFactorSrcRGB() const { return mBlendFactorSrcRGB; }

    nn::gfx::BlendFactor getBlendFactorSrcAlpha() const { return mBlendFactorSrcA; }

    nn::gfx::BlendFactor getBlendFactorDstRGB() const { return mBlendFactorDstRGB; }

    nn::gfx::BlendFactor getBlendFactorDstAlpha() const { return mBlendFactorDstA; }

    nvn::BlendEquation getBlendEquationRGB() const { return mBlendEquationRGB; }

    nvn::BlendEquation getBlendEquationAlpha() const { return mBlendEquationA; }

    const Color4f& getBlendConstantColor() const { return mBlendConstantColor; }

    bool getAlphaTestEnable() const { return mAlphaTestEnable; }

    nvn::AlphaFunc getAlphaTestFunc() const { return mAlphaTestFunc; }

    f32 getAlphaTestRef() const { return mAlphaTestRef; }

    bool getColorMaskR() const { return mColorMaskR; }

    bool getColorMaskG() const { return mColorMaskG; }

    bool getColorMaskB() const { return mColorMaskB; }

    bool getColorMaskA() const { return mColorMaskA; }

    bool getStencilTestEnable() const { return mStencilTestEnable; }

    nvn::StencilFunc getStencilTestFunc() const { return mStencilTestFunc; }

    s32 getStencilTestRef() const { return mStencilTestRef; }

    u32 getStencilTestMask() const { return mStencilTestMask; }

    nvn::StencilOp getStencilTestOpFail() const { return mStencilOpFail; }

    nvn::StencilOp getStencilTestOpZFail() const { return mStencilOpZFail; }

    nvn::StencilOp getStencilTestOpZPass() const { return mStencilOpZPass; }

#ifdef cafe

    Graphics::PolygonMode getPolygonModeFront() const { return mPolygonModeFront; }

    Graphics::PolygonMode getPolygonModeBack() const { return mPolygonModeBack; }

    bool getPolygonOffsetFrontEnable() const { return mPolygonOffsetFrontEnable; }

    bool getPolygonOffsetBackEnable() const { return mPolygonOffsetBackEnable; }

    bool getPolygonOffsetPointLineEnable() const { return mPolygonOffsetPointLineEnable; }

#endif  // cafe

    // private:
    bool mDepthTestEnable;
    bool mDepthWriteEnable;
    nvn::DepthFunc mDepthFunc;
    CullingMode mCullingMode;
    bool mBlendEnable;
    nn::gfx::BlendFactor mBlendFactorSrcRGB;
    nn::gfx::BlendFactor mBlendFactorSrcA;
    nn::gfx::BlendFactor mBlendFactorDstRGB;
    nn::gfx::BlendFactor mBlendFactorDstA;
    nvn::BlendEquation mBlendEquationRGB;
    nvn::BlendEquation mBlendEquationA;
    Color4f mBlendConstantColor;
    bool mAlphaTestEnable;
    nvn::AlphaFunc mAlphaTestFunc;
    f32 mAlphaTestRef;
    bool mColorMaskR;
    bool mColorMaskG;
    bool mColorMaskB;
    bool mColorMaskA;
    bool mStencilTestEnable;
    nvn::StencilFunc mStencilTestFunc;
    s32 mStencilTestRef;
    u32 mStencilTestMask;
    nvn::StencilOp mStencilOpFail;
    nvn::StencilOp mStencilOpZFail;
    nvn::StencilOp mStencilOpZPass;
#ifdef cafe
    Graphics::PolygonMode mPolygonModeFront;
    Graphics::PolygonMode mPolygonModeBack;
    bool mPolygonOffsetFrontEnable;
    bool mPolygonOffsetBackEnable;
    bool mPolygonOffsetPointLineEnable;
#endif  // cafe
};
#ifdef cafe
static_assert(sizeof(GraphicsContext) == 0x74, "sead::GraphicsContext size mismatch");
#endif  // cafe

}  // namespace sead

#endif  // SEAD_GRAPHICS_CONTEXT_H_
