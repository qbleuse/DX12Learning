/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "../Tools.hpp"
#include "../Vector3.hpp"
#include "Volume.hpp"

namespace GPM
{
class AABB : public Volume
{
public :
    Vec3  center;
    Vec3  extents;

    AABB()                  = default;
    AABB(const AABB& other) = default;
    AABB(AABB&& other)      = default;
    virtual ~AABB()         = default;
    AABB& operator=(AABB const& other) = default;
    AABB& operator=(AABB&& other) = default;

    AABB(const Vec3& center, float iI, float iJ, float iK) : Volume{}, center{center}, extents{iI, iJ, iK}
    {
    }

    AABB(const Vec3& min, const Vec3& max)
        : Volume{}, center{(max + min) * 0.5f}, extents{max.x - center.x, max.y - center.y, max.z - center.z}
    {}

    bool isPointInside(Vec3 pt) const noexcept
    {
        const Vec3 localPt = pt - center;

        return isBetween(localPt.x, -extents.x, extents.x) &&
               isBetween(localPt.y, -extents.y, extents.y) &&
               isBetween(localPt.z, -extents.z, extents.z);
    }
};

} // namespace GPM
