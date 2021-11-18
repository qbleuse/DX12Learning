/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include <cfloat>
#include <cmath>

#include "Types.hpp"

namespace GPM
{

union Vector2
{
    // Data members
    struct { f32 x; f32 y; };
    f32 e[2];

    Vector2() = default;
    constexpr Vector2(const f32 k)                              noexcept;
    constexpr Vector2(const f32 x, const f32 y = .0f)           noexcept;
    constexpr Vector2(const f32 coef[2])                        noexcept;
    
    // Static methods (pseudo-constructors)
    static constexpr Vector2 zero   ()                          noexcept;
    static constexpr Vector2 one    ()                          noexcept;
    static constexpr Vector2 right  ()                          noexcept;
    static constexpr Vector2 up     ()                          noexcept;

    // Member methods
    constexpr f32       sqrLength           ()                  const noexcept;
    f32                 length              ()                  const noexcept;
    constexpr f32       dot                 (const Vector2& v)  const noexcept;
    constexpr f32       cross               (const Vector2& v)  const noexcept;
    constexpr bool      isNull              ()                  const noexcept;
    constexpr bool      isOrthogonalTo      (const Vector2& v)  const noexcept;
    constexpr bool      isNormalized        ()                  const noexcept;
    constexpr bool      isOrthonormalTo     (const Vector2& v)  const noexcept;
    constexpr bool      isColinearTo        (const Vector2& v)  const noexcept;
    bool                isEqualTo           (const Vector2& v,
                                             const f32 eps = FLT_EPSILON) const noexcept;
    bool                isNotEqualTo        (const Vector2& v,
                                             const f32 eps = FLT_EPSILON) const noexcept;
    void                normalize           ()                  noexcept;
    void                safelyNormalize     ()                  noexcept;
    constexpr f32       sqrDistanceTo       (const Vector2& v)  const noexcept;
    f32                 distanceTo          (const Vector2& v)  const noexcept;
    f32                 angleWithUnitary    (const Vector2& v)  const noexcept;
    f32                 angleWith           (const Vector2& v)  const noexcept;
    constexpr f32       triangleArea        (const Vector2& v)  const noexcept;
    Vector2             normalized          ()                  const noexcept;
    Vector2             safelyNormalized    ()                  const noexcept;
    constexpr Vector2   projectedOnUnitary  (const Vector2& v)  const noexcept;
    constexpr Vector2   projectedOn         (const Vector2& v)  const noexcept;
    Vector2             rotated             (const f32 angle)   const noexcept;
    constexpr Vector2   rotated90           ()                  const noexcept;
    constexpr Vector2   lerp                (const Vector2& v, const f32 f32) const noexcept;

    // Operator overloads
    constexpr Vector2&  operator+=          (const Vector2& v)  noexcept;
    constexpr Vector2&  operator+=          (const Vector2&& v) noexcept;
    constexpr Vector2&  operator-=          (const Vector2& v)  noexcept;
    constexpr Vector2&  operator-=          (const Vector2&& v) noexcept;
    constexpr Vector2&  operator*=          (const Vector2& v)  noexcept;
    constexpr Vector2&  operator*=          (const Vector2&& v) noexcept;
    constexpr Vector2&  operator/=          (const Vector2& v)  noexcept;
    constexpr Vector2&  operator/=          (const Vector2&& v) noexcept;
    constexpr Vector2&  operator*=          (const f32 k)       noexcept;
    constexpr Vector2&  operator/=          (const f32 k)       noexcept;
    constexpr Vector2   operator+           (const Vector2& v)  const noexcept;
    constexpr Vector2   operator+           (const Vector2&& v) const noexcept;
    constexpr Vector2   operator-           (const Vector2& v)  const noexcept;
    constexpr Vector2   operator-           (const Vector2&& v) const noexcept;
    constexpr Vector2   operator*           (const Vector2& v)  const noexcept;
    constexpr Vector2   operator*           (const Vector2&& v) const noexcept;
    constexpr Vector2   operator/           (const Vector2& v)  const noexcept;
    constexpr Vector2   operator/           (const Vector2&& v) const noexcept;
    constexpr Vector2   operator-           ()                  const noexcept;
    constexpr Vector2   operator*           (const f32 k)       const noexcept;
    constexpr Vector2   operator/           (const f32 k)       const noexcept;
};

using Vec2 = Vector2;
using vec2 = Vector2;

#include "Vector2.inl"

} // End of namespace GPM