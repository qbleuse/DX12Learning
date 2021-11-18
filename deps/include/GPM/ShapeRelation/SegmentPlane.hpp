/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

#include "Intersection.hpp"
#include "../Shape3D/Segment.hpp"
#include "../Shape3D/Plane.hpp"

namespace GPM::SegmentPlane
{
    static bool isSegmentPlaneCollided(const Segment& seg, const Plane& plane, Intersection& intersection);

} /*namespace GPM*/