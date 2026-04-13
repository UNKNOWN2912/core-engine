#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>

enum class CameraType
{
    Perspective, Orthographic, Orbital
};

class Camera
{
    public:
        const glm::vec3 &GetPosition() const;
        const glm::vec3 &GetFront() const;
        const glm::vec3 &GetUp() const;
        const glm::mat4& GetView() const { return mView; }
        const glm::mat4& GetProjection() const { return mProjection; }
        float GetFov() const { return mFov; }
        float GetAspectRatio() const { return mAspectRatio; }
        float GetNearPlane() const { return mNearPlane; }
        float GetFarPlane() const { return mFarPlane; }
        CameraType GetCameraType() const { return mType; }
    
        void SetPosition(const glm::vec3 &position);
        void SetFront(const glm::vec3 &front);
        void SetUp(const glm::vec3 &up);
        void SetFov(float fov) { mFov = fov; }
        void SetAspectRatio(float aspectRatio) { mAspectRatio = aspectRatio; }
        void SetNearPlane(float nearPlane) { mNearPlane = nearPlane; }
        void SetFarPlane(float farPlane) { mFarPlane = farPlane; }
        void SetCameraType(CameraType type) { mType = type; }

        void Calculate();
    private:
        void CalculatePerspective();
        void CalculateOrthographic();
        void CalculateOrbital();
      
        glm::vec3 mPosition = glm::vec3(0, 0, 2);
        glm::vec3 mFront = glm::vec3(0, 0, -2);
        glm::vec3 mUp = glm::vec3(0, 1, 0);

        glm::mat4 mProjection = glm::mat4(1);
        glm::mat4 mView = glm::mat4(1);

        float mFov = 90.f;
        float mAspectRatio = 1.f;
        float mNearPlane = 0.01f;
        float mFarPlane = 100.f;

        CameraType mType = CameraType::Perspective;
};