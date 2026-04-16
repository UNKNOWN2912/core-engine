#include "CameraController.hpp"
#include "Input/Mouse.hpp"
#include "Core/Application.hpp"

void CameraController::SetCamera(Camera& camera, Window& window)
{
    mCamera = &camera;

    mCamera->SetCameraType(CameraType::Orbital)
        .SetFront(glm::vec3(0.0f))
        .SetPosition(glm::vec3(0.f,2.f,2.f))
        .SetAspectRatio(float(window.GetSize().x) / float(window.GetSize().y))
        .SetNearPlane(0.01f)
        .SetFarPlane(1000.f);

    ConnectWindow(window);
}
const Camera& CameraController::GetCamera() const 
{
    return *mCamera;    
}


void CameraController::Update()
{
    glm::vec3 cameraPosition = mCamera->GetPosition();
    glm::vec3 cameraFront = mCamera->GetFront();

    float speed = 0.05f;

    mCamera->SetCameraType(CameraType::Perspective);

    cameraFront.x = sin(glm::radians(mYaw)) * cos(glm::radians(-mPitch));
    cameraFront.y = sin(glm::radians(-mPitch));
    cameraFront.z = cos(glm::radians(mYaw)) * cos(glm::radians(-mPitch));


    glm::vec3 forward = normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    glm::vec3 side = normalize(cross(glm::vec3(cameraFront.x, 0, cameraFront.z), mCamera->GetUp()));

    if(mMoveForward)
    {
        cameraPosition +=  forward * speed;
    }
    if(mMoveBackward)
    {
        cameraPosition -= forward * speed;
    }
    if(mMoveLeft)
    {
        cameraPosition -= side * speed;
    }
    if(mMoveRight)
    {
        cameraPosition += side * speed;
    }
    if(mMoveUp)
    {
        cameraPosition += mCamera->GetUp() * speed;
    }
    if(mMoveDown)
    {
        cameraPosition -= mCamera->GetUp() * speed;
    }


    mCamera->SetFront(cameraFront);
    mCamera->SetPosition(cameraPosition);
}

void CameraController::OnKeyPress(Key key)
{
    if(key == Key::W)
        mMoveForward = true;
    if(key == Key::S)
        mMoveBackward = true;
    if(key == Key::A)
        mMoveLeft = true;
    if(key == Key::D)
        mMoveRight = true;
    if(key == Key::Space)
        mMoveUp = true;
    if(key == Key::LeftShift)
        mMoveDown = true;
}
void CameraController::OnKeyRelease(Key key)
{
    if(key == Key::W)
        mMoveForward = false;
    if(key == Key::S)
        mMoveBackward = false;
    if(key == Key::A)
        mMoveLeft = false;
    if(key == Key::D)
        mMoveRight = false;
    if(key == Key::Space)
        mMoveUp = false;
    if(key == Key::LeftShift)
        mMoveDown = false;
}
void CameraController::OnMouseMove(const glm::vec2& position, const glm::vec2& offset)
{

    if(!Application::GetInstance()->IsCursorHidden())
        return;
    mYaw -= offset.x;
    mPitch += offset.y;

    mPitch = glm::clamp(mPitch, -89.f, 89.f);
}
void CameraController::OnWindowResize(const glm::vec2& size)
{
    mCamera->SetAspectRatio(float(size.x) / float(size.y));
}

void CameraController::OnMouseButtonPress(MouseButton button) 
{
    
}

void CameraController::OnMouseButtonRelease(MouseButton button) 
{
    
}

void CameraController::OnScroll(const glm::vec2& scroll) 
{
    
}

void CameraController::ConnectWindow(Window& window) 
{
    window.AddListener(BindMember(CameraController::WindowEventCallback));
}

bool CameraController::WindowEventCallback(uint32_t code, void* data) 
{
    WindowEvent event = (WindowEvent)code;

    switch (event)
    {
		case WindowEvent::WindowResize:
			{
				glm::uvec2 size = *(glm::uvec2*)data;
				OnWindowResize(size);
				break;
			}
		case WindowEvent::WindowMousePress:
			{
				MouseButton button = *(MouseButton*)data;
				OnMouseButtonPress(button);
				break;
			}
		case WindowEvent::WindowMouseRelease:
			{
				MouseButton button = *(MouseButton*)data;
				OnMouseButtonRelease(button);
				break;
			}
		case WindowEvent::WindowMouseMove:
			{
				glm::vec2 position = *(glm::vec2*)data;
				glm::vec2 offset = position - previousMousePos;
				previousMousePos = position; 
				OnMouseMove(position, offset);
				break;
			}
		case WindowEvent::WindowScroll:
			{
				glm::vec2 scroll = *(glm::vec2*)data;
				OnScroll(scroll);
				break;
			}
		case WindowEvent::WindowKeyPress:
			{
				Key key = *(Key*)data;
				OnKeyPress(key);
				break;
			}
		case WindowEvent::WindowKeyRelease:
			{
				Key key = *(Key*)data;
				OnKeyRelease(key);
				break;
			}
        default:
            {
                break;
            }
    }
    
    return false;
}
