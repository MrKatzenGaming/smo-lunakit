#pragma once
#include "sead/math/seadQuat.h"

#include "imgui.h"

namespace ImGuiHelper {

// Vector editors
void Vector3Drag(const char* prefixName, const char* tooltip, sead::Vector3f* vec, float speed, float limit);
void Vector3Slide(const char* prefixName, const char* tooltip, sead::Vector3f* vec, float limit, bool isNormalize);

// Quaternion editors
void Quat(const char* tooltip, sead::Quatf* quat);

bool DragUInt(const char* label, u32* v, float v_speed, u32 v_min, u32 v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);

bool SliderUInt(const char* label, u32* v, u32 v_min, u32 v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);

};  // namespace ImGuiHelper
