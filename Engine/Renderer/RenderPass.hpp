#pragma once
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
    Clear,
    Store,
    DontCare
};

struct Attachment
{
    AttachmentUsage usage = AttachmentUsage::ColorOutput;
    LoadOperation loadOperation = LoadOperation::Load;
    StoreOperation storeOperation = StoreOperation::Store;
    glm::vec4 clearValue = glm::vec4(1.f);

    Attachment(AttachmentUsage usage, LoadOperation loadOperation, StoreOperation storeOperation, const glm::vec4& clearValue)
        : usage(usage), loadOperation(loadOperation), storeOperation(storeOperation), clearValue(clearValue) 
    {}
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
        std::vector<uint32_t> mColorAttachmentIndex;
        std::vector<uint32_t> mInputAttachmentIndex;
        uint32_t mDepthAttachmentIndex = UINT32_MAX;
};

class RenderPass
{
    public:
        uint32_t AddAttachment(const Attachment& attachment) { mAttachments.push_back(attachment); return mAttachments.size() - 1; }
        void AddSubpass(const Subpass& subpass) { mSubpasses.push_back(subpass); }
        uint32_t GetId() { return mId; }
    private:
        uint32_t mId = 0;
        std::vector<Subpass> mSubpasses;
        std::vector<Attachment> mAttachments;
};