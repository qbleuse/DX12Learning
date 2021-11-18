#pragma once

#ifdef _MSC_VER
#   define WIN32_LEAN_AND_MEAN
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#endif

#include "Types.hpp"
#include <math.h>

namespace GPM
{

f32 clamp(const f32 k, const f32 lo, const f32 hi);

bool f32AreEqual(const f32 a, const f32 b);
bool f32AreEqual(const f32 a, const f32 b, const f32 eps);

f32 lerpf(const f32 a, const f32 b, const f32 alpha);

#include "Calc.inl"

}