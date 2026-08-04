#pragma once
#include "EntityComponentSystem/EntityComponentSystem.hpp"

class SceneSerializer
{
public:
    void Export(std::string_view filename, const Scene &scene);
    void Import(std::string_view filename, Scene &scene);

private:
};