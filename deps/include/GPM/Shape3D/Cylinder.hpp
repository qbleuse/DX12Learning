/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Volume.hpp"
#include "../Vector3.hpp"
#include "Segment.hpp"
#include "Plane.hpp"
#include "InfiniteCylinder.hpp"

namespace GPM
{
    class Cylinder : public Volume
    {
        protected:

        Segment    m_segment;
        float   m_radius;    

        public:
    
        Cylinder ()                           = default;
        Cylinder(const Cylinder& other)       = default;
        Cylinder( Cylinder&& other)           = default;
        virtual ~Cylinder()                   = default;
        Cylinder& operator=(Cylinder const&)  = default;
        Cylinder& operator=(Cylinder &&)      = default;

        explicit Cylinder (const Vec3& center, const Vec3& normal, float height, float radius)
        :       Volume          {},
                m_segment  {Vec3(center - normal * (height / 2.f)), Vec3(center + normal * (height / 2.f))},
                m_radius   {radius}
        {}

        explicit Cylinder (const Segment& segment, float radius)
        :   Volume          {},
            m_segment    {segment},
            m_radius     {radius}
        {}

        explicit Cylinder (const Vec3& pt1, const Vec3& pt2, float radius)
        :   Volume          {},
            m_segment    (pt1, pt2),
            m_radius  {radius}
        {}
    
        InfiniteCylinder getInfiniteCylinder() const noexcept
        {
            return InfiniteCylinder(static_cast<Line>(m_segment), m_radius);
        }

        Plane LeftPlane () const noexcept
        {
            return Plane(m_segment.getPt1(), (m_segment.getPt1() - m_segment.getPt2()).normalized());
        }

        Plane RightPlane () const noexcept
        {
            return Plane(m_segment.getPt2(), (m_segment.getPt2() - m_segment.getPt1()).normalized());
        }

        Vec3 getCenter() const noexcept
        {
            return m_segment.getCenter();
        }

        const Segment&  getSegment  () const noexcept   { return m_segment;}
        Segment&        getSegment  () noexcept         { return m_segment;}
        const float&    getRadius   () const noexcept   { return m_radius;}

        void setSegment     (const Segment& newSegment)noexcept { m_segment = newSegment;}
        void setRadius      (const float& newRadius)   noexcept { m_radius = newRadius;}
    };

} /*namespace GPM*/