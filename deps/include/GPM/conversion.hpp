/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Quaternion.hpp"
#include "Transform.hpp"

namespace GPM
{

constexpr Mat3       toMatrix3       (const Mat4& m)                 noexcept;
constexpr Mat4       toMatrix4       (const Mat3& m)                 noexcept;

constexpr Mat3       toMatrix3       (const Quat& q)                 noexcept;
constexpr Mat4       toMatrix4       (const Quat& q)                 noexcept;

Quat                 toQuaternion    (const Mat3& m)                 noexcept;
Quat                 toQuaternion    (const Mat4& m)                 noexcept;

SplitTransform       toSplitTransform(const Transform& transfo)      noexcept;
Transform            toTransform     (const SplitTransform& transfo) noexcept;

#include "conversion.inl"

} // End of namespace GPM