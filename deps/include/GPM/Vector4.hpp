/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include <cfloat>
#include <cmath>

#include "Vector3.hpp"

namespace GPM
{

union alignas(16) Vector4
{
    // Data members. The following data members can be accessed publicly:
    // - Vec2 xyz, which is the same as {x, y, z}
    // - f32 x, which is the same as xyz.x or xy.x
    // - f32 y, which is the same as xyz.y or xy.y
    // - f32 z, which is the same as xyz.z or xy.z
    // - f32 w
    // - f32 e[4], which is the same as {x, y, z, w}
    struct
    {
        union 
        {
            Vec3 xyz;

            struct
            {
                union
                {
                    Vec2 xy;
                    struct { f32 x; f32 y; };
                };

                f32 z;
            };
        };

        f32 w;
    };

    f32 e[4];

    Vector4() noexcept = default;
    constexpr Vector4(const f32 k)                                              noexcept;
    constexpr Vector4(const f32 x, const f32 y, const f32 z, const f32 w = 1.f) noexcept;
    constexpr Vector4(const Vec2& v, const f32 z = .0f, const f32 w = 1.f)      noexcept;
    constexpr Vector4(const Vec3& v, const f32 w = 1.f)                         noexcept;
    constexpr Vector4(const f32 coef[4])                                        noexcept;

    // Methods
    inline constexpr f32       sqrLength           ()                              const noexcept;
    inline f32                 length              ()                              const noexcept;
    inline constexpr f32       dot                 (const Vector4& v)              const noexcept;

    constexpr Vector4 homogenized ()  const noexcept;
    constexpr void    homogenize  ()  noexcept;
    bool              isEqualTo   (const Vector4& v,
                                   const f32 eps = 1e-6)                        const noexcept;

    inline constexpr Vector4   lerp                (const Vector4& v, const f32 t) const noexcept;

    constexpr bool     operator== (const Vector4& v)                            const noexcept;
    constexpr Vector4&	operator+=          (const Vector4& v)              noexcept;
    constexpr Vector4&	operator+=          (const Vector4&& v)             noexcept;
    constexpr Vector4&	operator-=          (const Vector4& v)              noexcept;
    constexpr Vector4&	operator-=          (const Vector4&& v)             noexcept;
    constexpr Vector4& operator*= (const Vector4& v)                            noexcept;
    constexpr Vector4  operator*  (const Vector4& v)                            const noexcept;
    constexpr Vector4  operator/  (const Vector4& v)                            const noexcept;
    constexpr Vector4  operator*  (const f32 k)                                 const noexcept;
    constexpr Vector4  operator/  (const f32 k)                                 const noexcept;
    constexpr Vector4  operator+  (const Vector4& v) const noexcept;
    constexpr Vector4  operator+  (const Vector4&& v)const noexcept;
    constexpr Vector4  operator-  (const Vector4& v) const noexcept;
    constexpr Vector4  operator-  (const Vector4&& v)const noexcept;
};

using Vec4 = Vector4;
using vec4 = Vector4;

#include "Vector4.inl"

} // End of namespace GPM