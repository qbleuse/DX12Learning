/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */
 
#pragma once

#include "../Vector3.hpp"

namespace GPM
{
    class Line
    {
        protected :

        Vec3    m_origin, 
                m_normal;

        public : 

        Line ()                             = default;
        Line(const Line& other)             = default;
        Line(Line&& other)                  = default;
        ~Line()                             = default;
        Line& operator=(Line const&)        = default;
        Line& operator=(Line &&)            = default; 

        explicit Line (const Vec3& origin, const Vec3& normal)
            :   m_origin     {origin},
                m_normal {normal}
        {}

        const Vec3& getOrigin() const noexcept    { return m_origin; }
        const Vec3& getNormal() const noexcept    { return m_normal; }

        void setOrigin(const Vec3& newOrigin) noexcept { m_origin = newOrigin; }
        void setNormal(const Vec3& newNormal) noexcept { m_normal = newNormal; }
    };
} /*namespace GPM*/