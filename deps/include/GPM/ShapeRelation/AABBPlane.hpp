/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "../Shape3D/AABB.hpp"
#include "../Shape3D/Plane.hpp"
#include "../Vector3.hpp"

namespace GPM::AABBPlane
{

/**
 * @brief
 * @see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
 * @param aabb
 * @param plane
 * @return
 */
bool isAABBOnOrForwardPlane(const AABB& aabb, const Plane& plane)
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = aabb.extents.x * std::abs(plane.getNormal().x) + aabb.extents.y * std::abs(plane.getNormal().y) +
                    aabb.extents.z * std::abs(plane.getNormal().z);

    return -r <= plane.getSignedDistanceToPlane(aabb.center);
}

} // namespace GPM::AABBPlane
