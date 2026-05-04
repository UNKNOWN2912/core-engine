#pragma once
#include "Core/Application.hpp"

class ProjectManagerLayer : public Layer
{
    public:
        void OnAttach() override;
        void OnUpdate() override;
        void OnDetach() override;
};