/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "Volume.hpp"
#include "../Vector3.hpp"
#include "../Transform.hpp"
#include "../Referential.hpp"
#include "AABB.hpp"

namespace GPM
{
    class OrientedBox : public Volume
    {
        protected:

        Referential m_referential;
        float       m_iI {0.f}, m_iJ {0.f}, m_iK {0.f}; 

        public :

        OrientedBox ()                              = default;
        OrientedBox(const OrientedBox& other)       = default;
        OrientedBox(OrientedBox&& other)            = default;
        virtual ~OrientedBox()                      = default;
        OrientedBox& operator=(OrientedBox const&)  = default;
        OrientedBox& operator=(OrientedBox &&)      = default;

        explicit OrientedBox (float rightLenght, float upLenght, float forwardLenght, const Vec3& center = Vec3::zero(), const Vec3& rotation = Vec3::zero())
            :   Volume          {},
                m_referential    {center},
                m_iI             {rightLenght}, 
                m_iJ             {upLenght}, 
                m_iK             {forwardLenght}
        {
            Transform rotationMatrix = Transform::rotation(rotation);
            m_referential.unitI = rotationMatrix.right();
            m_referential.unitJ = rotationMatrix.up();
            m_referential.unitK = rotationMatrix.forward();
        }

        explicit OrientedBox(const Referential& referential, float rightLenght, float upLenght, float forwardLenght)
            :   Volume          {},
                m_referential    {referential},
                m_iI             {rightLenght}, 
                m_iJ             {upLenght}, 
                m_iK             {forwardLenght}
        {}

        AABB getAABB() const noexcept
        {
            Vec3 vecIi = m_referential.unitI * m_iI;
            Vec3 vecIj = m_referential.unitJ * m_iJ;
            Vec3 vecIk = m_referential.unitK * m_iK;
            float AABBiI = std::abs(Vec3::dot(Vec3::right(), vecIi)) + std::abs(Vec3::dot(Vec3::right(), vecIj)) + std::abs(Vec3::dot(Vec3::right(), vecIk));
            float AABBiJ = std::abs(Vec3::dot(Vec3::up(), vecIi)) + std::abs(Vec3::dot(Vec3::up(), vecIj)) + std::abs(Vec3::dot(Vec3::up(), vecIk));
            float AABBiK = std::abs(Vec3::dot(Vec3::forward(), vecIi)) + std::abs(Vec3::dot(Vec3::forward(), vecIj)) + std::abs(Vec3::dot(Vec3::forward(), vecIk));
            return AABB{m_referential.origin, AABBiI, AABBiJ, AABBiK};
        }

        Vec3 ptForwardTopLeft     () const noexcept { return m_referential.origin - (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ) + (m_referential.unitK * m_iK); }
        Vec3 ptForwardTopRight    () const noexcept { return m_referential.origin + (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ) + (m_referential.unitK * m_iK); }
        Vec3 ptForwardBottomLeft  () const noexcept { return m_referential.origin - (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ) + (m_referential.unitK * m_iK); }
        Vec3 ptForwardBottomRight () const noexcept { return m_referential.origin + (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ) + (m_referential.unitK * m_iK); }
        Vec3 ptBackTopLeft        () const noexcept { return m_referential.origin - (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ) - (m_referential.unitK * m_iK); }
        Vec3 ptBackTopRight       () const noexcept { return m_referential.origin + (m_referential.unitI * m_iI) + (m_referential.unitJ * m_iJ) - (m_referential.unitK * m_iK); }
        Vec3 ptBackBottomLeft     () const noexcept { return m_referential.origin - (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ) - (m_referential.unitK * m_iK); }
        Vec3 ptBackBottomRight    () const noexcept { return m_referential.origin + (m_referential.unitI * m_iI) - (m_referential.unitJ * m_iJ) - (m_referential.unitK * m_iK); }

        virtual Referential  getReferential()    const noexcept  { return m_referential; }
        Referential&         getReferential()          noexcept  { return m_referential; }
        virtual float        getExtI()           const noexcept  { return m_iI; }
        virtual float        getExtJ()           const noexcept  { return m_iJ; }
        virtual float        getExtK()           const noexcept  { return m_iK; }

        void setReferential (const Referential& newReferential)    noexcept   {  m_referential = newReferential; }
        void setExtI        (const float& newExtI)                 noexcept   {  m_iI = newExtI; }
        void setExtJ        (const float& newExtJ)                 noexcept   {  m_iJ = newExtJ; }
        void setExtK        (const float& newExtK)                 noexcept   {  m_iK = newExtK; }
    };
} /*namespace GPM*/
