/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Types.hpp"
#include "Vector3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"

#include <cmath>

namespace GPM
{

struct SplitTransform
{
    Quat rotation;
    Vec3 position;
    Vec3 scale;
};


struct Transform
{
    Mat4 model{Mat4::identity()};

    // Constructor
    Transform() = default;
    Transform(const Vec3& t,
              const Vec3& r = Vec3::zero(),
              const Vec3& s = Vec3::one())                                    noexcept;
    constexpr Transform(const Mat4& m)                                        noexcept;

    // Static methods
    static constexpr Mat4 translation (const Vec3& t)                         noexcept;
    static Mat4           rotationX   (const f32 angle)                       noexcept;
    static Mat4           rotationY   (const f32 angle)                       noexcept;
    static Mat4           rotationZ   (const f32 angle)                       noexcept;
    static Mat4           rotation    (const Vec3& t)                         noexcept;
    static Mat4           rotationAround(const Vec3& axis, const f32 angle)   noexcept;
    static constexpr Mat4 scaling     (const Vec3& t)                         noexcept;
    static Mat4           TRS         (const Vec3& t, const Vec3& r,
                                       const Vec3& s)                         noexcept;
    static Mat4           lookAt      (const Vec3& eyePos,
                                       const Vec3& targetPos,
                                       const Vec3& normalizedUp = Vec3::up()) noexcept;
    static constexpr Mat4 symFrustrum (const f32 right, const f32 top,
                                       const f32 near_, const f32 far_)      noexcept;
    static Mat4           perspective (const f32 fovY,  const f32 aspect,
                                       const f32 near_, const f32 far_)      noexcept;
    static constexpr Mat4 orthographic(const f32 right, const f32 left, const f32 top, const f32 bottom,
                                       const f32 nearVal, const f32 farVal) noexcept;
    static constexpr Mat4 viewport    (const f32 x,     const f32 y,
                                       const f32 width, const f32 height)     noexcept;

    // Getters
    Mat4                  normalMat   ()                                      const noexcept;
    constexpr Vec3        right       ()                                      const noexcept;
    constexpr Vec3        up          ()                                      const noexcept;
    constexpr Vec3        backward    ()                                      const noexcept;
    constexpr Vec3        forward     ()                                      const noexcept;
    constexpr Vec3        translation ()                                      const noexcept;
    Mat4                  rotation    ()                                      const noexcept;
    Vec3                  eulerAngles ()                                      const noexcept;
    Vec3                  scaling     ()                                      const noexcept;

    // Setters
    constexpr void        setLocalTranslation (const Vec3& t)                 noexcept;
    constexpr void        setGlobalTranslation(const Vec3& t)                 noexcept;
    void                  setRotation         (const Vec3& r)                 noexcept;
    constexpr void        setScale            (const Vec3& s)                 noexcept;

    constexpr void 	setVectorUp(const Vec3& newUp) noexcept;
    constexpr void 	setVectorRight(const Vec3& newRight) noexcept;
    constexpr void 	setVectorForward(const Vec3& newForward) noexcept;

    // Transformations
    constexpr void        translateX  (const f32 shift)                       noexcept;
    constexpr void        translateY  (const f32 shift)                       noexcept;
    constexpr void        translateZ  (const f32 shift)                       noexcept;
    constexpr void        translate   (const Vec3& t)                         noexcept;
    void                  rotateX     (const f32 angle)                       noexcept;
    void                  rotateY     (const f32 angle)                       noexcept;
    void                  rotateZ     (const f32 angle)                       noexcept;
    void                  rotate      (const Vec3& r)                         noexcept;
    void                  rotateAround(const Vec3& axis, const f32 angle)     noexcept;
    constexpr void        scaleX      (const f32 coef)                        noexcept;
    constexpr void        scaleY      (const f32 coef)                        noexcept;
    constexpr void        scaleZ      (const f32 coef)                        noexcept;
    constexpr void        scale       (const Vec3& s)                         noexcept;
    void                  lookAt      (const Vec3& targetPos)                 noexcept;

    void                  apply       (const Vec3& t,
                                       const Vec3& r,
                                       const Vec3& s)                         noexcept;
    constexpr void        apply       (const Mat4& m)                         noexcept;
};

#include "Transform.inl"

} // End of namespace GPM