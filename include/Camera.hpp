#pragma once

#include "types.hpp"

enum CameraKeyInputFlags
{
    CAM_NO_MOVE       = 0,
    CAM_MOVE_FORWARD  = 1 << 0,
    CAM_MOVE_BACKWARD = 1 << 1,
    CAM_STRAFE_LEFT   = 1 << 2,
    CAM_STRAFE_RIGHT  = 1 << 3,
    CAM_MOVE_FAST     = 1 << 4,
    CAM_MOVE_UP       = 1 << 5,
    CAM_MOVE_DOWN     = 1 << 6,
};

// Camera input data (needed to update movement)
struct CameraInputs
{
    double deltaTime;
    int keyInputsFlags; // Using CameraKeyInputFlags
    float mouseDX;
    float mouseDY;
};

struct Camera
{
    float3 position = {};
    float pitch     = 0.f;
    float yaw       = 0.f;
    float roll      = 0.f;

    void UpdateFreeFly(const CameraInputs& inputs);
    mat4 GetViewMatrix() const;
};
