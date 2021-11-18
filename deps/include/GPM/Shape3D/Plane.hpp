/*
 * Copyright (C) 2021 Amara Sami, Dallard Thomas, Nardone William, Six Jonathan
 * This file is subject to the LGNU license terms in the LICENSE file
 * found in the top-level directory of this distribution.
 */

#pragma once

#include "../Vector3.hpp"
#include <limits>

namespace GPM
{
class Plane
{
protected:
    Vec3  m_normal   = Vec3::up(); // unit vector
    float m_distance = 0.f;        // Distance with origin

public:
    Plane(float distance, const Vec3& normal);
    Plane(const Vec3& p1, const Vec3& p2, const Vec3& p3);
    Plane(const Vec3& p1, const Vec3& normal);

    Plane()                   = default;
    Plane(const Plane& other) = default;
    Plane(Plane&& other)      = default;
    virtual ~Plane()          = default;
    Plane& operator=(Plane const&) = default;
    Plane& operator=(Plane&&) = default;

    /**
     * @brief Get the Plans Intersection between 3 plan. Throw if
     *
     * @param plane1
     * @param plane2
     * @param plane3
     * @param point : return pointor with nullptr if intersection not found
     * @return void
     */
    static void getPlansIntersection(const Plane& plane1, const Plane& plane2, const Plane& plane3, Vec3** point);

    /**
     * @brief Get the Signed Distance To Plane object with a point
     *
     * @param plane
     * @param point
     * @return float
     */
    static float getSignedDistanceToPlane(const Plane& plane, const Vec3& point)
    {
        return Vec3::dot(point, plane.m_normal) - plane.m_distance;
    }

    inline float getSignedDistanceToPlane(const Vec3& point) const
    {
        return Vec3::dot(m_normal, point) + m_distance;
    }

    /**
     * @brief Get the Closest Point To Plane object
     *
     * @param plane
     * @param point
     * @return Vec3
     */
    static Vec3 getClosestPointToPlane(const Plane& plane, const Vec3& point)
    {
        return point + plane.m_normal * getSignedDistanceToPlane(plane, point);
    }

    /**
     * @brief Get the Distance From Point To Plan object
     *
     * @param plane
     * @param point
     * @return * float
     */
    static float getDistanceFromPointToPlan(const Plane& plane, const Vec3& point)
    {
        return plane.m_normal.dot(point) + plane.m_distance;
    }

    /**
     * @brief return true if point is on plane
     *
     * @param plane
     * @param point
     * @return true
     * @return false
     */
    static bool isOnPlane(const Plane& plane, const Vec3& point)
    {
        float rst = plane.m_normal.dot(point);
        return rst < plane.m_distance + std::numeric_limits<float>::epsilon() &&
               rst > plane.m_distance - std::numeric_limits<float>::epsilon();
    }

    /**
     * @brief return true if a point is between two plane inclueding. [plane, plane]
     *
     * @param plan1
     * @param plan2
     * @param pt
     * @return float
     */
    static bool vecIsBetweenPlanes(const Plane& plane1, const Plane& plane2, const Vec3& pt)
    {
        return (getSignedDistanceToPlane(plane1, pt) <= -std::numeric_limits<float>::epsilon()) &&
               (getSignedDistanceToPlane(plane2, pt) <= -std::numeric_limits<float>::epsilon());
    }

    inline const Vec3& getNormal() const noexcept
    {
        return m_normal;
    }

    inline const float& getDistance() const noexcept
    {
        return m_distance;
    }

    inline void setNormal(const Vec3& newNormal) noexcept
    {
        m_normal = newNormal.normalized();
    }

    inline void setDistance(float newDistance) noexcept
    {
        m_distance = newDistance;
    }
};
} // namespace GPM
