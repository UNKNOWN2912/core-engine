#pragma once
#include "Renderer/Enums.hpp"
#include "Renderer/Image.hpp"
#include <glm/glm.hpp>

enum class AttachmentUsage
{
    ColorOutput,
    Input,
    Depth,
    Presentation
};

enum class LoadOperation
{
    Clear,
    Load,
    DontCare
};

enum class StoreOperation
{
    Store,
    DontCare
};

struct Attachment
{
    ImageUsage usage = ImageUsage::ColorOutput;
    LoadOperation loadOperation = LoadOperation::Load;
    StoreOperation storeOperation = StoreOperation::Store;
    glm::vec4 clearValue = glm::vec4(1.f);
    ImageFormat format;
};

struct AttachmentRef
{
    uint32_t attachmentIndex;
    AttachmentUsage subpassUsage;
};

class Subpass
{
    public:
        void AddColorAttachment(uint32_t index) { mColorAttachmentIndex.push_back(index); }
        void AddInputAttachment(uint32_t index) { mInputAttachmentIndex.push_back(index); }
        void SetDepthAttachment(uint32_t index) { mDepthAttachmentIndex = index; }

    private:
        friend class Graphic;
        std::vector<uint32_t> mColorAttachmentIndex;
        std::vector<uint32_t> mInputAttachmentIndex;
        uint32_t mDepthAttachmentIndex = UINT32_MAX;
};

class Dependency
{
    public:
        void SetSourceSubpass(uint32_t index) { mSourceSubpassIndex = index; }
        void SetDestinationSubpass(uint32_t index) { mDestinationSubpassIndex = index; }


    private:
        friend class Graphic;
        uint32_t mSourceSubpassIndex = UINT32_MAX;
        uint32_t mDestinationSubpassIndex = UINT32_MAX;
};

class RenderPass
{
    public:
        RenderPass& AddAttachment(const Attachment& attachment) { mAttachments.push_back(attachment); return *this; }
        RenderPass& AddSubpass(const Subpass& subpass, const Dependency& dependency) 
        { 
            mSubpasses.push_back(subpass);
            mDependencies.push_back(dependency);
            return *this;
        }

        uint32_t GetId() { return mId; }

    private:
        friend class Graphic;
        uint64_t mId = 0;
        std::vector<Subpass> mSubpasses;
        std::vector<Dependency> mDependencies;
        std::vector<Attachment> mAttachments;
};