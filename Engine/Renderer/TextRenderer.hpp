#pragma once
#include "Assets/Font.hpp"
#include "Assets/MeshManager.hpp"
#include "Assets/ShaderManager.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/UniformBuffer.hpp"
#include <string>

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

    static void DrawText(const Font &font, const std::string &text);
    static void SetCamera(const Camera &camera);

    static void DrawCharacter(const Font &font, char ch, const glm::vec3 &position);

    static float GetSpacing()
    {
        return mSpacing;
    }

    static void SetSpacing(float spacing)
    {
        mSpacing = spacing;
    }

private:
    static GraphicsPipeline mTextPipeline;
    static UniformBuffer mUniformBuffer;
    static Descriptor mUniformDescriptor;
    static TextUniformData mUniformData;

    static Camera mCamera;
    static ShaderID mShaderID;

    static Buffer mVertexBuffer;
    static Buffer mIndexBuffer;

    static float mSpacing;
};
