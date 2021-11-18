/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

#include "../Vector3.hpp"
#include "Plane.hpp"
#include "Line.hpp"

#include <limits>

namespace GPM
{
    class Segment
    {
        protected:

        Vec3      m_pt1, m_pt2;

        public:

        Segment ()					                = default;
        Segment (const Segment& other)			    = default;
        Segment (Segment&& other)				    = default;
        virtual ~Segment ()				            = default;
        Segment& operator=(Segment const& other)	= default;
        Segment& operator=(Segment && other)		= default;

        explicit Segment (const Vec3& pt1, const Vec3& pt2)
            :   m_pt1    {pt1},
                m_pt2    {pt2}
        {}

        Plane getLeftPlane () const noexcept
        {
            return Plane(m_pt1, (m_pt1 - m_pt2).normalized());
        }

        Plane getRightPlane () const noexcept
        {
            return Plane(m_pt2, (m_pt2 - m_pt1).normalized());
        }

        float getDistanceWithPoint(Vec3 pt) const noexcept
        {
            Vec3 AB = m_pt2 - m_pt1;

            if (Plane::getSignedDistanceToPlane(getLeftPlane(), pt) > std::numeric_limits<float>::epsilon())
            {
                if(Plane::getSignedDistanceToPlane(getRightPlane(), pt) > std::numeric_limits<float>::epsilon())
                {
                    Vec3 AC = pt - m_pt1;
                    //The point in between the 2 plan. So The result is the rejection of the vectoo AC on AB
                    return (AC - ((Vec3::dot(AC, AB) / Vec3::dot(AB, AB)) * AB)).length();
                }   
                else
                {
                    //The point is behind the plane of point B. The result is the length of the vector BC
                    return (pt - m_pt2).length();
                }
            }
            else
            {
                //The point is behind the plane of point A. The result is the length of the vector AC
                return (pt - m_pt1).length();
            }
        }

        Vec3 getCenter() const noexcept
        {
            return m_pt1 + 0.5f * (m_pt2 - m_pt1);
        }

        float getLenght() const noexcept
        {
            return (m_pt2 - m_pt1).length();
        }

        const Vec3& getPt1() const noexcept { return m_pt1; }
        const Vec3& getPt2() const noexcept { return m_pt2; }

        void setPt1(const Vec3& newPt) noexcept { m_pt1 = newPt; }
        void setPt2(const Vec3& newPt) noexcept { m_pt2 = newPt; }

        explicit operator Line () const noexcept //use static_cast<Line>(seg) to convert seg to Line
        {
            return Line(m_pt1, (m_pt2 - m_pt1).normalized());
        }
    };
} /*namespace GPM*/