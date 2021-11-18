/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once
#include "Types.hpp"
#include "Vector3.hpp"

namespace GPM
{

#define MAT3_COL  3u
#define MAT3_COEF 9u

union Matrix3
{
    // Data members. The following data members can be accessed publicly:
    // - f32  e[9], which is the same as {c[0].x, c[0].y, c[0].z, c[1].x, ...}
    // - Vec4 c[3], which is the same as {Vec3{e[0], e[1], e[2]}, Vec3{e[3], ...}, ...}
    f32  e[MAT3_COEF];
    Vec3 c[MAT3_COL];

    // Static methods, pseudo-constructors
    static constexpr Matrix3 zero       ()                  noexcept;
    static constexpr Matrix3 identity   ()                  noexcept;

    // Methods
    constexpr Matrix3   cofactor        ()                  const noexcept;
    constexpr Matrix3   transposed      ()                  const noexcept;
    constexpr Matrix3   adjugate        ()                  const noexcept;
    constexpr f32       det             ()                  const noexcept;
    constexpr Matrix3   inversed        ()                  const noexcept;
    constexpr f32       trace           ()                  const noexcept;

    // Operator overloads
    constexpr Matrix3&  operator*=      (const Matrix3& m)  noexcept;
    constexpr Matrix3&  operator/=      (const f32 k)       noexcept;
    constexpr Matrix3   operator*       (const Matrix3& m)  const noexcept;
    constexpr Vec3      operator*       (const Vec3& v)     const noexcept;
    constexpr Matrix3   operator/       (const f32 k)       const noexcept;
};

using Mat3 = Matrix3;
using mat3 = Matrix3;

#include "Matrix3.inl"

} // End of namespace GPM