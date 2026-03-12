#pragma once
#include <Core/Event.hpp>
#include <Maths/Vector.hpp>
#include <string>

struct WindowSpecification
{
	Vector2u size;
	Vector2u position;
	std::string title;
};

enum class WindowEvent
{
	WindowClose
};

class Window
{
public:
	void Create(const WindowSpecification &specification);
	void Destroy();

	Vector2u GetSize() const;
	Vector2u GetPosition() const;
	std::string GetTitle() const;

	void SetSize(const Vector2u& size);
	void SetPosition(const Vector2u& position);
	void SetTitle(const std::string& title);

	void AddListener(std::function<bool(uint32_t code, void* data)> listener);

	void ProcessEvent();

	void* GetNativeWindow() const;

private:
	void* mPlatformData = nullptr;
};


