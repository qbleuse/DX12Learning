/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Volume.hpp"
#include "../Vector3.hpp"
#include "Segment.hpp"
#include "InfiniteCylinder.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"

namespace GPM
{
    class Capsule : public Volume
    {
        protected:

        Segment m_segment;
        float   m_radius;

        public:

        Capsule ()					                = default;
        Capsule (const Capsule& other)			    = default;
        Capsule (Capsule&& other)				    = default;
        virtual ~Capsule ()				            = default;
        Capsule& operator=(Capsule const& other)    = default;
        Capsule& operator=(Capsule && other)		= default;

        explicit Capsule (const Vec3& center, const Vec3& normal, float height, float radius)
        :       Volume          {},
                m_segment    {Vec3(center - normal * (height / 2.f)), Vec3(center + normal * (height / 2.f))},
                m_radius     {radius}
        {}

        explicit Capsule (const Segment& segment, float radius)
        :       Volume          {},
                m_segment    {segment},
                m_radius     {radius}
        {}

        /**
         * @brief Get the InfiniteCapsule object on the capsule
         * 
         * @return InfiniteCapsule 
         */
        InfiniteCylinder getInfiniteCylinder() const noexcept
        {
            return InfiniteCylinder(static_cast<Line>(m_segment), m_radius);
        }

        Cylinder BodyCylinder () const noexcept
        {
            return Cylinder(m_segment.getPt1(), m_segment.getPt2(), m_radius);
        }

        Sphere LeftSphere () const noexcept
        {
            return Sphere(m_radius, m_segment.getPt1());
        }

        Sphere RightSphere () const noexcept
        {
            return Sphere(m_radius, m_segment.getPt2());
        }

        bool isInside(const Vec3& pt) const noexcept
        {
            return m_segment.getDistanceWithPoint(pt) <= m_radius;
        }

        Vec3 getCenter() const noexcept
        {
            return m_segment.getCenter();
        }

        const Segment&  getSegment      () const noexcept   { return m_segment;}
        Segment&        getSegment      () noexcept         { return m_segment;}
        const float&    getRadius       () const noexcept   { return m_radius;}

        void setSegment  (const Segment& newSegment)    noexcept { m_segment = newSegment;}
        void setRadius   (const float& newRadius)       noexcept { m_radius = newRadius;}
    };

} /*namespace GPM*/