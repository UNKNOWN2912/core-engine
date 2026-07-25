#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

enum class CameraType
{
    Perspective,
    Orthographic,
    Orbital,
    Light
};

class Camera
{
public:
    const glm::vec3 &GetPosition() const;
    const glm::vec3 &GetFront() const;
    const glm::vec3 &GetUp() const;
    const glm::mat4 &GetView() const;
    const glm::mat4 &GetProjection() const;
    float GetFov() const;
    float GetAspectRatio() const;
    float GetNearPlane() const;
    float GetFarPlane() const;
    CameraType GetCameraType() const;
    float GetZoom() const;

    Camera &SetPosition(const glm::vec3 &position);
    Camera &SetFront(const glm::vec3 &front);
    Camera &SetUp(const glm::vec3 &up);
    Camera &SetFov(float fov);
    Camera &SetAspectRatio(float aspectRatio);
    Camera &SetNearPlane(float nearPlane);
    Camera &SetFarPlane(float farPlane);
    Camera &SetCameraType(CameraType type);
    Camera &SetZoom(float zoom);

    void SetViewMatrix(const glm::mat4 &matrix)
    {
        mView = matrix;
    }
    void SetProjectionMatrix(const glm::mat4 &matrix)
    {
        mProjection = matrix;
    }

    void Calculate();

private:
    void CalculatePerspective();
    void CalculateOrthographic();
    void CalculateOrbital();
    void CalculateLight();

    float mZoom = 1.f;

    glm::vec3 mPosition = glm::vec3(0, 0, -1);
    glm::vec3 mFront = glm::vec3(0, 0, 1);
    glm::vec3 mUp = glm::vec3(0, 1, 0);

    glm::mat4 mProjection = glm::mat4(1);
    glm::mat4 mView = glm::mat4(1);

    float mFov = 80.f;
    float mAspectRatio = 1.f;
    float mNearPlane = 0.01f;
    float mFarPlane = 100.f;

    CameraType mType = CameraType::Perspective;
};