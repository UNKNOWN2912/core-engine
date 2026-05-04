#include "Editor.hpp"
#include "CameraController.hpp"

void Editor::OnStart()
{
    AttachLayer<GameLayer>();
    AttachLayer<EditorLayer>();
}

void Editor::OnUpdate()
{

}

void Editor::OnEnd()
{
    
}

void Editor::OnKeyPress(Key key) 
{
}

void Editor::OnWindowResize(const glm::uvec2 &size) 
{
}

void Editor::OnMouseMove(const glm::vec2 &position, const glm::vec2 &offset) 
{
}


void WriteColorStyleFromFile()
{

}

CREATE_APPLICATION(Editor);