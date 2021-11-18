/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Volume.hpp"
#include "../Vector3.hpp"
#include "Line.hpp"

namespace GPM
{
    class InfiniteCylinder : public Volume
    {
        protected:

        Line    m_line;
        float   m_radius;  

        public:
        
        InfiniteCylinder ()                                   = default;
        InfiniteCylinder(const InfiniteCylinder& other)       = default;
        InfiniteCylinder( InfiniteCylinder&& other)           = default;
        virtual ~InfiniteCylinder()                           = default;
        InfiniteCylinder& operator=(InfiniteCylinder const&)  = default;
        InfiniteCylinder& operator=(InfiniteCylinder &&)      = default;

        explicit InfiniteCylinder (const Line& line, float radius)
        :   Volume          {},
            m_line    {line},
            m_radius  {radius}
        {}

        explicit InfiniteCylinder (const Vec3& pt, const Vec3& normal, float radius)
        :   Volume          {},
            m_line    (pt, normal),
            m_radius  {radius}
        {}

        bool isPointInside(const Vec3& pt) 
        {
            return Vec3::cross(m_line.getNormal(), pt - m_line.getOrigin()).length() <= m_radius;
        }

        const Line&     getLine     () const noexcept   { return m_line;}
        Line&           getLine     () noexcept         { return m_line;}
        const float&    getRadius   () const noexcept   { return m_radius;}

        void setLine     (const Line& newLine)      noexcept { m_line = newLine;}
        void setRadius   (const float& newRadius)   noexcept { m_radius = newRadius;}  
    };

} /*namespace GPM*/