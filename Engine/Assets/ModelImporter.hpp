#pragma once
#include "EntityComponentSystem/EntityComponentSystem.hpp"

class ModelImporter
{
  public:
    void Import(std::string_view filename, Scene &scene);

  private:
};