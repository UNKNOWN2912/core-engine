#include "CameraController.hpp"
#include "Input/Mouse.hpp"
#include "Core/Application.hpp"
#include "Maths/Noise.hpp"

void CameraController::SetCamera(Camera& camera, Window& window)
{
    CHROME_TRACE_FUNCTION();
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
    CHROME_TRACE_FUNCTION();
    return *mCamera;    
}


static float combinedPerlin(glm::vec3 st)
{
    CHROME_TRACE_FUNCTION();

	float l = 2.9;
	float p = 6.3;	
	float result = 0;
	float k = 0.01;

	float a = 10;
	
	for (int i = 0; i < 10; i++)
	{
		float fi = i;
		result += (PerlinNoise(st * glm::pow(l,fi) * k) * a) / pow(p,fi);
	}

	return result;
}


void CameraController::Update()
{
    CHROME_TRACE_FUNCTION();

    glm::vec3 cameraPosition = mCamera->GetPosition();
    glm::vec3 cameraFront = mCamera->GetFront();


    mCamera->SetCameraType(CameraType::Perspective);

    cameraFront.x = sin(glm::radians(mYaw)) * cos(glm::radians(-mPitch));
    cameraFront.y = sin(glm::radians(-mPitch));
    cameraFront.z = cos(glm::radians(mYaw)) * cos(glm::radians(-mPitch));


    glm::vec3 forward = normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    glm::vec3 side = normalize(cross(glm::vec3(cameraFront.x, 0, cameraFront.z), mCamera->GetUp()));

    if(mMoveForward)
    {
        cameraPosition +=  forward * mSpeed * Application::GetInstance()->GetDeltaTime();
    }
    if(mMoveBackward)
    {
        cameraPosition -= forward * mSpeed * Application::GetInstance()->GetDeltaTime();
    }
    if(mMoveLeft)
    {
        cameraPosition -= side * mSpeed * Application::GetInstance()->GetDeltaTime();
    }
    if(mMoveRight)
    {
        cameraPosition += side * mSpeed * Application::GetInstance()->GetDeltaTime();
    }
    if(mMoveUp)
    {
        cameraPosition += mCamera->GetUp() * mSpeed * Application::GetInstance()->GetDeltaTime();
    }
    if(mMoveDown)
    {
        cameraPosition -= mCamera->GetUp() * mSpeed * Application::GetInstance()->GetDeltaTime();
    }

    // cameraPosition.y = 0;
    // cameraPosition.y = combinedPerlin(cameraPosition) + 2.5;


    mCamera->SetFront(cameraFront);
    mCamera->SetPosition(cameraPosition);
}

void CameraController::OnKeyPress(Key key)
{
    CHROME_TRACE_FUNCTION();
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
    if(key == Key::LeftControl)
        mSpeed *= 5;
}
void CameraController::OnKeyRelease(Key key)
{
    CHROME_TRACE_FUNCTION();
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
    if(key == Key::LeftControl)
        mSpeed /= 5;
}
void CameraController::OnMouseMove(const glm::vec2& position, const glm::vec2& offset)
{
    CHROME_TRACE_FUNCTION();


    if(!Application::GetInstance()->IsCursorHidden())
        return;
    mYaw -= offset.x * mSensitivity;
    mPitch += offset.y * mSensitivity;

    mPitch = glm::clamp(mPitch, -89.f, 89.f);
}
void CameraController::OnWindowResize(const glm::vec2& size)
{
    CHROME_TRACE_FUNCTION();

    mCamera->SetAspectRatio(float(size.x) / float(size.y));
}

void CameraController::OnMouseButtonPress(MouseButton button)
{
    CHROME_TRACE_FUNCTION();

    
}

void CameraController::OnMouseButtonRelease(MouseButton button)
{
    CHROME_TRACE_FUNCTION();

    
}

void CameraController::OnScroll(const glm::vec2& scroll)
{
    CHROME_TRACE_FUNCTION();

    
}

void CameraController::ConnectWindow(Window& window)
{
    CHROME_TRACE_FUNCTION();

    window.AddListener(BindMember(CameraController::WindowEventCallback));
}

bool CameraController::WindowEventCallback(uint32_t code, void* data)
{
    CHROME_TRACE_FUNCTION();

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
