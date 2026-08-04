#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

enum class CameraType
{
    Perspective,
    Orthographic,
    Orbital,
    Light,
    Menu
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

    glm::vec3 ScreenToWorld(const glm::vec3 &position) const
    {
        return mInverse * glm::vec4(position, 1.f);
    };

    glm::vec2 ScreenToWorld(const glm::vec2 &position) const
    {
        return ScreenToWorld(glm::vec3(position, 0));
    };

    void SetViewMatrix(const glm::mat4 &matrix)
    {
        mView = matrix;
    }
    void SetProjectionMatrix(const glm::mat4 &matrix)
    {
        mProjection = matrix;
    }

    void Calculate();

    const glm::mat4 &GetInverse() const;

    void SetBounds(float left, float right, float top, float bottom)
    {
        mLeft = left;
        mRight = right;
        mTop = top;
        mBottom = bottom;
    }

private:
    void CalculatePerspective();
    void CalculateOrthographic();
    void CalculateOrbital();
    void CalculateLight();
    void CalculateMenuCamera();

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

    float mLeft = 0.f;
    float mRight = 0.f;
    float mTop = 0.f;
    float mBottom = 0.f;

    glm::mat4 mInverse = glm::mat4(1.f);

    CameraType mType = CameraType::Perspective;
};