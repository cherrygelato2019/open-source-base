#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include "../struct.h"
#include "../../../../um/um.h"
#include "../../init_cheat/offsets/offsets.h"
#include "../../math/math.h"
using namespace Math;
using namespace Structs::camera;

Camera Camera::GetCameraInstance() {
    return driver.read<Camera>(this->address + Offsets::Workspace::CurrentCamera);
}

Vector3 Camera::GetCameraPosition() {
    return driver.read<Vector3>(this->address + Offsets::Camera::Position);
}

void Camera::SetCameraPosition(const Vector3& position) {
    driver.write<Vector3>(this->address + Offsets::Camera::Position, position);
}

Matrix3 Camera::GetCameraRotation() {
    return driver.read<Matrix3>(this->address + Offsets::Camera::Rotation);
}

float Camera::GetFov() {
    return driver.read<float>(this->address + Offsets::Camera::FieldOfView);
}

Camera Camera::GetCameraSubject() {
    return driver.read<Camera>(this->address + Offsets::Camera::CameraSubject);
}

int Camera::GetCameraType() {
    return driver.read<int>(this->address + Offsets::Camera::CameraType);
}

void Camera::SetCameraRotation(Math::Matrix3 Rotation)
{
    driver.write<Math::Matrix3>(this->address + Offsets::Camera::Rotation, Rotation);
}

void Camera::SetCameraMaxZoom(int zoom) {
    driver.write<int>(this->address + 0x240, zoom);  
}

void Camera::SetCameraMinZoom(int zoom) {
    driver.write<int>(this->address + 0x244, zoom);  
}
