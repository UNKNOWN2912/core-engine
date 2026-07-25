#pragma once
#include "Assets/Font.hpp"
#include "Assets/MeshManager.hpp"
#include "Assets/ShaderManager.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Transform.hpp"
#include "Renderer/UniformBuffer.hpp"
#include <string>

struct TextInstanceData
{
    uint32_t charaterTextureId;
    glm::mat4 model = glm::mat4(1.f);
    glm::vec4 color = glm::vec4(1.0);
};

struct TextUniformData
{
    glm::mat4 view;
    glm::mat4 projection;
};

class TextRenderer
{
public:
    static void Initialize();
    static void Terminate();

    static void DrawText(const Font &font, const std::string &text, float spacing = 1.f, const glm::vec4 &color = glm::vec4(1), const Transform &transform = {});
    static void SetCamera(const Camera &camera);
    static void DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const glm::vec4 &color = glm::vec4(1), const Transform &transform = {});
    static void SetSpacing(float spacing);
    static void Flush();

private:
    static GraphicsPipeline mTextPipeline;
    static UniformBuffer mUniformBuffer;
    static Descriptor mUniformDescriptor;
    static TextUniformData mUniformData;

    static Camera mCamera;
    static ShaderID mShaderID;

    static Buffer mVertexBuffer;
    static Buffer mIndexBuffer;

    static MeshID mQuadMeshId;

    static Buffer mQuadVertexBuffer;
    static Buffer mQuadIndexBuffer;

    static InstanceBuffer mInstanceBuffer;

    static std::vector<TextInstanceData> mInstanceData;
};
