#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>

enum class CameraType
{
    Perspective, Orthographic, Orbital, Light
};

class Camera
{
    public:
        const glm::vec3& GetPosition() const;
        const glm::vec3& GetFront() const;
        const glm::vec3& GetUp() const;
        const glm::mat4& GetView() const { return mView; }
        const glm::mat4& GetProjection() const { return mProjection; }
        float GetFov() const { return mFov; }
        float GetAspectRatio() const { return mAspectRatio; }
        float GetNearPlane() const { return mNearPlane; }
        float GetFarPlane() const { return mFarPlane; }
        CameraType GetCameraType() const { return mType; }
        float GetZoom() const { return mZoom; }

        glm::vec3& GetPositionRef() { return mPosition; }
        glm::vec3& GetFrontRef() { return mFront; }
        glm::vec3& GetUpRef() { return mUp; }
        glm::mat4& GetViewRef() { return mView; }
        glm::mat4& GetProjectionRef() { return mProjection; }
        float& GetFovRef() { return mFov; }
        float& GetAspectRatioRef() { return mAspectRatio; }
        float& GetNearPlaneRef() { return mNearPlane; }
        float& GetFarPlaneRef() { return mFarPlane; }
        CameraType& GetCameraTypeRef() { return mType; }
        float& GetZoomRef() { return mZoom; }
    
        Camera& SetPosition(const glm::vec3 &position);
        Camera& SetFront(const glm::vec3 &front);
        Camera& SetUp(const glm::vec3 &up);
        Camera& SetFov(float fov) { mFov = fov; return *this; }
        Camera& SetAspectRatio(float aspectRatio) { mAspectRatio = aspectRatio; return *this; }
        Camera& SetNearPlane(float nearPlane) { mNearPlane = nearPlane; return *this; }
        Camera& SetFarPlane(float farPlane) { mFarPlane = farPlane; return *this; }
        Camera& SetCameraType(CameraType type) { mType = type; return *this; }
        Camera& SetZoom(float zoom) { mZoom = zoom; return *this; }
        

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