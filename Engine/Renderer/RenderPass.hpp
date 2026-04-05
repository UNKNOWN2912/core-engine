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
    public:
        Attachment& SetUsage(ImageUsage usage) { mUsage = usage; return *this; }
        Attachment& SetLoadOperation(LoadOperation loadOperation) { mLoadOperation = loadOperation; return *this; }
        Attachment& SetStoreOperation(StoreOperation storeOperation) { mStoreOperation = storeOperation; return *this; }
        Attachment& SetClearValue(const glm::vec4& clearValue) { mClearValue = clearValue; return *this; }
        Attachment& SetFormat(ImageFormat format) { mFormat = format; return *this; }

        ImageUsage GetImageUsage() const { return mUsage; }
        LoadOperation GetLoadOperation() const { return mLoadOperation; }
        StoreOperation GetStoreOperation() const { return mStoreOperation; }
        const glm::vec4& GetClearValue() const { return mClearValue; }
        ImageFormat GetFormat() const { return mFormat; }
    private:
        ImageUsage mUsage = ImageUsage::ColorOutput;
        LoadOperation mLoadOperation = LoadOperation::Load;
        StoreOperation mStoreOperation = StoreOperation::Store;
        glm::vec4 mClearValue = glm::vec4(1.f);
        ImageFormat mFormat;
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
        Dependency& SetSourceSubpass(uint32_t index) { mSourceSubpassIndex = index; return *this; }
        Dependency& SetDestinationSubpass(uint32_t index) { mDestinationSubpassIndex = index; return *this; }

        uint32_t GetSourceSubpass() const { return mSourceSubpassIndex; }
        uint32_t GetDestinationSubpass() const { return mDestinationSubpassIndex; }

    private:
        friend class Graphic;
        uint32_t mSourceSubpassIndex = UINT32_MAX;
        uint32_t mDestinationSubpassIndex = UINT32_MAX;
};

class RenderPass
{
    public:
        RenderPass& SetId(uint64_t id) { mId = id; return *this; }
        RenderPass& AddAttachment(const Attachment& attachment) { mAttachments.push_back(attachment); return *this; }
        RenderPass& AddSubpass(const Subpass& subpass, const Dependency& dependency) 
        { 
            mSubpasses.push_back(subpass);
            mDependencies.push_back(dependency);
            return *this;
        }

        uint64_t GetId() const { return mId; }
        const std::vector<Subpass>& GetSubpasses() const { return mSubpasses; }
        const std::vector<Dependency>& GetDependencies() const { return mDependencies; } 
        const std::vector<Attachment>& GetAttachments() const { return mAttachments; }

    private:
        uint64_t mId = 0;
        std::vector<Subpass> mSubpasses;
        std::vector<Dependency> mDependencies;
        std::vector<Attachment> mAttachments;
};