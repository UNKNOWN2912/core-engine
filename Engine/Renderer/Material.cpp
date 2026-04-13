#include "Material.hpp"


void Material::LoadAlbedo(std::string_view filename)
{
    mAlbedo.Load(filename, 0);
}

void Material::LoadShaders(std::string_view vertexShader, std::string_view fragmentShader)
{
    mPipeline.LoadVertexShader(vertexShader);
    mPipeline.LoadFragmentShader(fragmentShader);
}

void Material::Create()
{
    
}
