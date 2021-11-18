#include "GPM/Shape3D/Plane.hpp"
#include "GPM/Matrix3.hpp"

using namespace GPM;


Plane::Plane (float distance, const Vec3& normal)
    :   m_normal     (normal.normalized()),
        m_distance   (distance)
{}

Plane::Plane (const Vec3& p1, const Vec3& p2, const Vec3& p3)
    :   m_normal     ((p2 - p1).cross(p3 - p1).normalized()),
        m_distance   (p1.dot(m_normal))
{}


Plane::Plane (const Vec3& p1, const Vec3& normal)
    :   m_normal     (normal.normalized()),
        m_distance   (p1.dot(m_normal))
{}


void Plane::getPlansIntersection (const Plane& plane1, const Plane& plane2, const Plane& plane3, Vec3** point)
{
    Mat3 eqCoef;
    eqCoef.c[0] = plane1.m_normal;
    eqCoef.c[1] = plane2.m_normal;
    eqCoef.c[2] = plane3.m_normal;

    Vec3 eqRslt {plane1.m_distance, plane2.m_distance, plane3.m_distance};
    **point = eqCoef.inversed() * eqRslt;
}