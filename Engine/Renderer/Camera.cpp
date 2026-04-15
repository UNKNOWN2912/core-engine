#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera& Camera::SetPosition(const glm::vec3 &position) { mPosition = position; return *this; }
Camera& Camera::SetFront(const glm::vec3 &front) { mFront = front; return *this; }
Camera& Camera::SetUp(const glm::vec3 &up) { mUp = glm::normalize(up); return *this; }

const glm::vec3 &Camera::GetPosition() const { return mPosition; }
const glm::vec3 &Camera::GetFront() const { return mFront; }
const glm::vec3 &Camera::GetUp() const { return mUp; }

void Camera::Calculate()
{
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
    }
}

void Camera::CalculatePerspective()
{
    mView = glm::lookAt(mPosition, mFront + mPosition, mUp);
    mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, mNearPlane, mFarPlane);
}

void Camera::CalculateOrthographic()
{
    mView = glm::lookAt(mPosition, mFront + mPosition, mUp);
    mProjection = glm::ortho(-mAspectRatio, mAspectRatio, -1.f, 1.f, mNearPlane, mFarPlane);
}

void Camera::CalculateOrbital()
{
    mView = glm::lookAt(mPosition, mFront, mUp);
    mProjection = glm::perspective(glm::radians(mFov), mAspectRatio, mNearPlane, mFarPlane);
}