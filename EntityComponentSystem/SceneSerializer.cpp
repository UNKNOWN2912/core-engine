#include "SceneSerializer.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

void SceneSerializer::Export(std::string_view filename, const Scene &scene)
{
    json json;
}

void SceneSerializer::Import(std::string_view filename, Scene &scene)
{
}
