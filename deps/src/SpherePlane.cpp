#include "GPM/ShapeRelation/SpherePlane.hpp"
#include "GPM/Vector3.hpp"

using namespace GPM;

bool SpherePlane::isSphereOnOrForwardPlaneCollided(const Sphere& sphere, const Plane& plane)
{
    return plane.getSignedDistanceToPlane(sphere.getCenter()) > -sphere.getRadius();
}
