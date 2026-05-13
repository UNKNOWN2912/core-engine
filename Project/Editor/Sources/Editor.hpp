#pragma once
#include <Engine.hpp>


class Editor : public Application
{
    void OnStart() override;
    void OnUpdate() override;
    void OnEnd() override;

    void OnKeyPress(Key key) override;
    void OnWindowResize(const glm::uvec2 &size) override;
    void OnMouseMove(const glm::vec2 &position, const glm::vec2 &offset) override;
    void UpdateCamera();

};
