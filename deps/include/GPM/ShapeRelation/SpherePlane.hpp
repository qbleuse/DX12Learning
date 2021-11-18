/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

#include "../Shape3D/Plane.hpp"
#include "../Shape3D/Sphere.hpp"

namespace GPM::SpherePlane
{
    bool isSphereOnOrForwardPlaneCollided(const Sphere& sphere, const Plane& plane);

} /*namespace GPM*/