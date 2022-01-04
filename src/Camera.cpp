/* system */
#include <algorithm>

#include "Camera.hpp"
#include "GPM/Transform.hpp"


void Camera::UpdateFreeFly(const CameraInputs& inputs)
{
    const float CAM_MOUSE_SENSITIVITY_X = 0.0007f;
    const float CAM_MOUSE_SENSITIVITY_Y = 0.0007f;

    // Azimuth/inclination angles in radians
    float azimuth     = yaw;
    float inclination = pitch;

    // Spheric coordinates
    float cosAzimuth     = std::cos(azimuth);
    float sinAzimuth     = std::sin(azimuth);
    float cosInclination = std::cos(inclination);
    float sinInclination = std::sin(inclination);

    // Compute speed
    float speed = 4.f;
    if (inputs.keyInputsFlags & CAM_MOVE_FAST)
        speed *= 10.f;
    float frameSpeed = (float)(speed * inputs.deltaTime);

    // Move forward/backward
    float forwardVelocity = 0.f;
    if      (inputs.keyInputsFlags & CAM_MOVE_FORWARD)  forwardVelocity -= frameSpeed;
    else if (inputs.keyInputsFlags & CAM_MOVE_BACKWARD) forwardVelocity += frameSpeed;

    // Strafe left/right
    float strafeVelocity = 0.f;
    if      (inputs.keyInputsFlags & CAM_STRAFE_LEFT)  strafeVelocity -= frameSpeed;
    else if (inputs.keyInputsFlags & CAM_STRAFE_RIGHT) strafeVelocity += frameSpeed;

    // Forward movement
    position.z +=  cosAzimuth * cosInclination * forwardVelocity;
    position.x += -sinAzimuth * cosInclination * forwardVelocity;
    position.y +=  sinInclination * forwardVelocity;

    // Strafe movement
    position.z += sinAzimuth * strafeVelocity;
    position.x += cosAzimuth * strafeVelocity;

    // Up movement
    if (inputs.keyInputsFlags & CAM_MOVE_UP)   position.y += frameSpeed;
    if (inputs.keyInputsFlags & CAM_MOVE_DOWN) position.y -= frameSpeed;

    // Pitch
    pitch += inputs.mouseDY * CAM_MOUSE_SENSITIVITY_Y;
    pitch = GPM::clamp(pitch, -TAU / 4.f, TAU / 4.f); // Limit rotation to -90,90 range

    // Yaw
    yaw += inputs.mouseDX * CAM_MOUSE_SENSITIVITY_X;
    yaw = GPM::Modulo(((yaw + TAU) + TAU / 2.f), TAU) - TAU / 2.f; // Loop around -180,180
}

GPM::mat4 Camera::GetViewMatrix() const
{
    return GPM::Transform::translation(-position) * GPM::Transform::rotationY(yaw) * GPM::Transform::rotationX(pitch) * GPM::Transform::rotationZ(roll);
    //return GPM::Transform::rotationZ(roll) * GPM::Transform::rotationX(pitch) * GPM::Transform::rotationY(yaw) * GPM::Transform::translation(-position);
}