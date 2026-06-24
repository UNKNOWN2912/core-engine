#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Camera.hpp"
#include "Core/Macro.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera &Camera::SetPosition(const glm::vec3 &position)
{
    CHROME_TRACE_FUNCTION();
    mPosition = position;
    return *this;
}
Camera &Camera::SetFront(const glm::vec3 &front)
{
    CHROME_TRACE_FUNCTION();
    mFront = front;
    return *this;
}
Camera &Camera::SetUp(const glm::vec3 &up)
{
    CHROME_TRACE_FUNCTION();
    mUp = glm::normalize(up);
    return *this;
}

Camera &Camera::SetFov(float fov)
{
    mFov = fov;
    return *this;
}

Camera &Camera::SetAspectRatio(float aspectRatio)
{
    mAspectRatio = aspectRatio;
    return *this;
}

Camera &Camera::SetNearPlane(float nearPlane)
{
    mNearPlane = nearPlane;
    return *this;
}

Camera &Camera::SetFarPlane(float farPlane)
{
    mFarPlane = farPlane;
    return *this;
}

Camera &Camera::SetCameraType(CameraType type)
{
    mType = type;
    return *this;
}

Camera &Camera::SetZoom(float zoom)
{
    mZoom = zoom;
    return *this;
}

const glm::vec3 &Camera::GetPosition() const
{
    CHROME_TRACE_FUNCTION();
    return mPosition;
}
const glm::vec3 &Camera::GetFront() const
{
    CHROME_TRACE_FUNCTION();
    return mFront;
}
const glm::vec3 &Camera::GetUp() const
{
    CHROME_TRACE_FUNCTION();
    return mUp;
}

const glm::mat4 &Camera::GetView() const
{
    return mView;
}

const glm::mat4 &Camera::GetProjection() const
{
    return mProjection;
}

float Camera::GetFov() const
{
    return mFov;
}

float Camera::GetAspectRatio() const
{
    return mAspectRatio;
}

float Camera::GetNearPlane() const
{
    return mNearPlane;
}

float Camera::GetFarPlane() const
{
    return mFarPlane;
}

CameraType Camera::GetCameraType() const
{
    return mType;
}

float Camera::GetZoom() const
{
    return mZoom;
}
void Camera::Calculate()
{
    CHROME_TRACE_FUNCTION();
    switch (mType)
    {
    case CameraType::Perspective:
        CalculatePerspective();
        break;
    case CameraType::Orthographic:
        CalculateOrthographic();
        break;
    case CameraType::Orbital:
        CalculateOrbital();
        break;
    case CameraType::Light:
        CalculateLight();
        break;
    }

    mProjection[1][1] *= -1;
}

void Camera::CalculatePerspective()
{
    CHROME_TRACE_FUNCTION();
    mView = glm::lookAt(mPosition, mFront + mPosition, mUp);
    mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, mNearPlane, mFarPlane);
}

void Camera::CalculateOrthographic()
{
    CHROME_TRACE_FUNCTION();
    mView = glm::lookAt(mPosition, mFront + mPosition, mUp);
    mProjection = glm::ortho(-mAspectRatio * mZoom, mAspectRatio * mZoom, -1.f * mZoom, 1.f * mZoom, mNearPlane, mFarPlane);
}

void Camera::CalculateOrbital()
{
    CHROME_TRACE_FUNCTION();
    mView = glm::lookAt(mPosition, mFront, mUp);
    mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, mNearPlane, mFarPlane);
}
void Camera::CalculateLight()
{
    CHROME_TRACE_FUNCTION();
    mView = glm::lookAt(mPosition, mFront, mUp);
    mProjection = glm::ortho(-mZoom, mZoom, -1.f * mZoom, 1.f * mZoom, mNearPlane, mFarPlane);
}
