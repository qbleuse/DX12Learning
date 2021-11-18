/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Vector4.hpp"
#include "Types.hpp"

namespace GPM
{

#define MAT4_COL 4u
#define MAT4_COEF 16u

// Aligned to 16 to enable auto-vectorization
union alignas(16) Matrix4
{
    // Data members. The following data members can be accessed publicly:
    // - f32  e[16], which is the same as {c[0].x, c[0].y, c[0].z, c[0].w, c[1].x, ...}
    // - Vec4 c[4], which is the same as {{e[0], e[1], e[2], e[3]}, {...}}
    f32  e[MAT4_COEF];
    Vec4 c[MAT4_COL];

    // Constructors
    Matrix4() noexcept = default;
    constexpr Matrix4(const f32 e0,  const f32 e1,  const f32 e2,  const f32 e3,
                      const f32 e4,  const f32 e5,  const f32 e6,  const f32 e7,
                      const f32 e8,  const f32 e9,  const f32 e10, const f32 e11,
                      const f32 e12, const f32 e13, const f32 e14, const f32 e15)     noexcept;
    constexpr Matrix4(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3) noexcept;

    // Static methods, pseudo-constructors
    static constexpr Matrix4 zero      ()                     noexcept;
    static constexpr Matrix4 identity  ()                     noexcept;

    // Methods
    constexpr Matrix4        cofactor  ()                     const noexcept;
    constexpr Matrix4        transposed()                     const noexcept;
    constexpr Matrix4        adjugate  ()                     const noexcept;
    constexpr f32            det       ()                     const noexcept;
    constexpr Matrix4        inversed  ()                     const noexcept;
    constexpr f32            trace     ()                     const noexcept;
    bool                     isEqualTo (const Matrix4& m,
                                        const f32 eps = 1e-6) const noexcept;

    // Operator overloads
    constexpr bool           operator== (const Matrix4& m)    const noexcept;
    constexpr Matrix4&       operator*= (const Matrix4& m)    noexcept;
    constexpr Matrix4&       operator/= (const f32 k)         noexcept;
    constexpr Matrix4        operator*  (const Matrix4& m)    const noexcept;
    constexpr Vec4           operator*  (const Vec4& v)       const noexcept;
    constexpr Matrix4        operator/  (const f32 k)         const noexcept;
};

using Mat4 = Matrix4;
using mat4 = Matrix4;

#include "Matrix4.inl"

} // End of namespace GPM
