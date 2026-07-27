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
    glm::mat4 model = glm::mat4(1.f);
    glm::vec4 forgroundColor = glm::vec4(1.0);
    glm::vec4 backgroundColor = glm::vec4(1.0);
    uint32_t startIndex = 0;
    uint32_t count = 0;
};

struct TextUniformData
{
    glm::mat4 view;
    glm::mat4 projection;
};

struct TextPushConstant
{
    int mode = 0;
};

struct TextProperty
{
    float spacing = 1.f;
    glm::vec4 forgroundColor;
    glm::vec4 backgroundColor;
    Transform transform;
};

class TextRenderer
{
public:
    static void Initialize();
    static void Terminate();

    static void DrawText(const Font &font, const std::string &text, float spacing = 1.f, const glm::vec4 &forgroundColor = glm::vec4(1), const glm::vec4 &backgroundColor = glm::vec4(1), const Transform &transform = {});
    static void DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const glm::vec4 &forgroundColor = glm::vec4(1), const glm::vec4 &backgroundColor = glm::vec4(0), const Transform &transform = {});

    static void DrawText(const Font &font, const std::string &text, const TextProperty &property);
    static void DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const TextProperty &property);

    static void SetCamera(const Camera &camera);
    static void SetSpacing(float spacing);
    static void Flush();

    static TextPushConstant &GetPushConstant()
    {
        return mPushConstant;
    }

private:
    static GraphicsPipeline mTextPipeline;
    static UniformBuffer mUniformBuffer;
    static Descriptor mUniformDescriptor;
    static Descriptor mBezierDescriptor;
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

    static TextPushConstant mPushConstant;
};
