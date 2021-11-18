/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Volume.hpp"
#include "../Vector3.hpp"

namespace GPM
{
    class Sphere : public Volume
    {
        protected:
    
        Vec3            m_center {Vec3::zero()};
        float           m_radius {0.f};

        public :

        Sphere ()                           = default;
        Sphere(const Sphere& other)         = default;
        Sphere(Sphere&& other)              = default;
        virtual ~Sphere()                   = default;
        Sphere& operator=(Sphere const&)    = default;
        Sphere& operator=(Sphere &&)        = default; 

        explicit Sphere (float radius, const Vec3& localCenter = Vec3::zero())
            :   Volume          {},
                m_center         {localCenter},
                m_radius         {radius}
        {}
    
        virtual Vec3     getCenter() const noexcept { return m_center;}
        virtual float    getRadius() const noexcept { return m_radius;}

        void    setCenter(const Vec3&  newCenter) noexcept { m_center = newCenter;}
        void    setRadius(const float& newRadius) noexcept { m_radius = newRadius;}
    };
} /*namespace GPM*/


