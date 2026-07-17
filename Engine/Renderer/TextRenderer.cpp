#include "TextRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include <cstring>

struct TextPushConstantData
{
    glm::mat4 model;
    TextureID textureId;
};

struct TextVertex
{
    glm::vec3 position;
    glm::vec2 uv;
};

void TextRenderer::Initialize()
{
}

void TextRenderer::Terminate()
{
}

void TextRenderer::DrawCharacter(const Font &font, char ch, const glm::vec3 &position)
{
}

void TextRenderer::DrawText(const Font &font, const std::string &text)
{
}

void TextRenderer::SetCamera(const Camera &camera)
{
}

GraphicsPipeline TextRenderer::mTextPipeline;
UniformBuffer TextRenderer::mUniformBuffer;
Descriptor TextRenderer::mUniformDescriptor;
TextUniformData TextRenderer::mUniformData;

Camera TextRenderer::mCamera;
ShaderID TextRenderer::mShaderID;

Buffer TextRenderer::mVertexBuffer;
Buffer TextRenderer::mIndexBuffer;
float TextRenderer::mSpacing = 1.f;