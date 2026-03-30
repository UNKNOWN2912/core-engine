#pragma once
#include <Core/Event.hpp>
#include <glm/glm.hpp>
#include <string>

struct WindowSpecification
{
	glm::uvec2 size;
	glm::uvec2 position;
	std::string title;
};

enum class WindowEvent
{
	WindowClose
};

struct WindowData;

class Window
{
public:
	void Create(const WindowSpecification &specification);
	void Destroy();
	glm::uvec2 GetSize() const;
	glm::uvec2 GetPosition() const;
	std::string GetTitle() const;
	void SetSize(const glm::uvec2& size);
	void SetPosition(const glm::uvec2& position);
	void SetTitle(const std::string& title);
	void AddListener(std::function<bool(uint32_t code, void* data)> listener);
	void ProcessEvent();
	void* GetNativeWindow() const;

private:
	WindowData* mData = nullptr;
};


