/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Vector3.hpp"
#include "constants.hpp"
#include "Types.hpp"

namespace GPM
{

union alignas(16) Quaternion
{
    // Data members. The following data members can be accessed publicly:
    // - Vec3 v, which is the same as {x, y, z}
    // - f32 x
    // - f32 y
    // - f32 z
    // - f32 w
    // - f32 e[4], which is the same as {x, y, z, w}
    struct
    {
        union
        {
            Vec3 v;
            struct
            {
                f32 x;
                f32 y;
                f32 z;
            };
        };
        f32 w;
    };
    f32 e[4];

    static constexpr Quaternion identity  ()                                       noexcept;
    static Quaternion           angleAxis (const f32 angle, const Vec3& axis)      noexcept;
    static Quaternion           fromEuler (const Vec3& angles)                     noexcept;
    static Quaternion           lookAt    (const Vec3& sourcePoint,
                                           const Vec3& destPoint)                  noexcept;

    // Methods
    constexpr Vec3              axis            ()                                      const noexcept;
    Vec3                        unitAxis        ()                                      const noexcept;
    f32                         angle           ()                                      const noexcept;
    f32                         angleWith       (const Quaternion& q)                   const noexcept;
    constexpr Vec3              rotate          (const Vec3& v)                         const noexcept;
    Vec3                        eulerAngles     ()                                      const noexcept;
    constexpr f32               sqrLength       ()                                      const noexcept;
    f32                         length          ()                                      const noexcept;
    constexpr f32               dot             (const Quaternion& q)                   const noexcept;
    Quaternion                  normalized      ()                                      const noexcept;
    Quaternion                  safelyNormalized()                                      const noexcept;
    void                        normalize       ()                                      noexcept;
    void                        safelyNormalize ()                                      noexcept;
    constexpr Quaternion        conjugate       ()                                      const noexcept;
    constexpr Quaternion        inversed        ()                                      const noexcept;
    constexpr Quaternion        to              (const Quaternion& target)              const noexcept;
    Quaternion                  slerp           (const Quaternion& target, const f32 t) const noexcept;
    Quaternion                  nlerp           (const Quaternion& target, const f32 t) const noexcept;
    bool                        isEqualTo       (const Quaternion& q,
                                                 const f32 eps = FLT_EPSILON)           const noexcept;

    // Operator overloads
    constexpr bool              operator== (const Quaternion& q)                   const noexcept;
    constexpr Quaternion        operator*= (const f32 k)                           const noexcept;
    constexpr Quaternion        operator/= (const f32 k)                           const noexcept;
    constexpr Quaternion        operator+  (const Quaternion& q)                   const noexcept;
    constexpr Quaternion        operator-  (const Quaternion& q)                   const noexcept;
    constexpr Quaternion        operator-  ()                                      const noexcept;
    constexpr Quaternion        operator*  (const Quaternion& q)                   const noexcept;
    constexpr Vec3              operator*  (const Vec3& v)                         const noexcept;
    constexpr Quaternion        operator*  (const f32 k)                           const noexcept;
    constexpr Quaternion        operator/  (const f32 k)                           const noexcept;
};

using Quat = Quaternion;

#include "Quaternion.inl"

} // End of namespace GPM
