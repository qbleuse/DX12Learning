/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "../Vector3.hpp"
#include "Plane.hpp"
#include "../Referential.hpp"
#include "../Tools.hpp"

#include <limits>

namespace GPM
{
    #define QUAD_OUTCODE_INSIDE   0  // 0000
    #define QUAD_OUTCODE_LEFT     1  // 0001
    #define QUAD_OUTCODE_RIGHT    2  // 0010
    #define QUAD_OUTCODE_BOTTOM   4  // 0100
    #define QUAD_OUTCODE_TOP      8  // 1000
    
    class Quad
    {
        protected:
    
        Referential m_referential;
        float m_iI, m_iJ;

        public:
    
        Quad ()                       = default;
        Quad(const Quad& other)       = default;
        Quad( Quad&& other)           = default;
        virtual ~Quad()               = default;
        Quad& operator=(Quad const&)  = default;
        Quad& operator=(Quad &&)      = default;

        /**
         * @brief Construct a new Quad object
         * 
         * @param referential   :   A central point and three unit vectors, two by two 
         *                          orthogonal. Two of the vectors are collinear alongside the quad and the third
         *                          vector represents the vector normal to the plane of the rectangle.
         * @param iI            :   lengths in i vector
         * @param iJ            :   lengths in j vector
         */
         explicit Quad (const Referential& referential, float iI, float iJ)
            :   m_referential    {referential},
                m_iI             {iI},
                m_iJ             {iJ}
        {}

        Vec3 PtTopLeft      ()  const noexcept  { return m_referential.origin - (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ); }
        Vec3 PtTopRight     ()  const noexcept  { return m_referential.origin + (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ); }
        Vec3 PtBottomLeft   ()  const noexcept  { return m_referential.origin - (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ); }
        Vec3 PtBottomRight  ()  const noexcept  { return m_referential.origin + (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ); }

        bool isPointInside(const Vec3& pt) const noexcept
        {
            return  isBetween(Vec3::dot(m_referential.unitI, pt - m_referential.origin), -m_iI, m_iI) &&
                    isBetween(Vec3::dot(m_referential.unitJ, pt - m_referential.origin), -m_iJ, m_iJ) &&
                    isBetween(Vec3::dot(m_referential.unitK, pt - m_referential.origin), -std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon());
        }

        bool isPointInsideQuadZone(const Vec3& pt) const noexcept 
        {
            return  isBetween(Vec3::dot(m_referential.unitI, pt - m_referential.origin), -m_iI, m_iI) &&
                    isBetween(Vec3::dot(m_referential.unitJ, pt - m_referential.origin), -m_iJ, m_iJ);
        }

        int isPointInsideQuadZoneOutCode(const Vec3& pt) const noexcept
        {
            int outCode = 0;
            float projectionPtOnI = Vec3::dot(m_referential.unitI, pt - m_referential.origin);
            float projectionPtOnJ = Vec3::dot(m_referential.unitJ, pt - m_referential.origin);
            outCode += projectionPtOnI <  m_iI ? 0 : QUAD_OUTCODE_RIGHT;
            outCode += projectionPtOnI > -m_iI ? 0 : QUAD_OUTCODE_LEFT;
            outCode += projectionPtOnJ <  m_iJ ? 0 : QUAD_OUTCODE_TOP;
            outCode += projectionPtOnJ > -m_iJ ? 0 : QUAD_OUTCODE_BOTTOM;
            return outCode;
        }

        const Referential&  getReferential()    const noexcept  { return m_referential; }
        Referential&        getReferential()          noexcept  { return m_referential; }
        const float&        getExtI()           const noexcept  { return m_iI; }
        const float&        getExtJ()           const noexcept  { return m_iJ; }

        void setReferential (const Referential& newReferential)    noexcept   {  m_referential = newReferential; }
        void setExtI        (const float& newExtI)                 noexcept   {  m_iI = newExtI; }
        void setExtJ        (const float& newExtJ)                 noexcept   {  m_iJ = newExtJ; }

        explicit operator Plane() const noexcept //use static_cast<Plane>(quad) to convert quad to plane
        {
            return Plane(m_referential.origin, m_referential.unitK);
        }
   
    };

} /*namespace GPM*/