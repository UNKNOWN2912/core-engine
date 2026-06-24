#include "TextureManager.hpp"

TextureID TextureManager::GenerateID()
{
    return (TextureID)mLastTextureId++;
}

void TextureManager::Initialize()
{
    mDescriptor.AddBindlessDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment, 1024);
    mDescriptor.CreateDescriptor();

    mSampler.CreateSampler();
}
void TextureManager::Terminate()
{
}

TextureID TextureManager::LoadTexture(std::string_view filename)
{
    TextureID id = GenerateID();

    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Load(filename);

    mTextureMap[id] = texture;

    mDescriptor.UpdateImageIndex(mTextureMap[id]->GetImage(), ImageLayout::ShaderRead, mSampler, 0, (uint32_t)id);

    return id;
}

TextureID TextureManager::CreateTexture(void *data, const glm::uvec2 &size, ImageFormat format)
{
    TextureID id = GenerateID();

    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Create(data, size, format);

    mTextureMap[id] = texture;

    mDescriptor.UpdateImageIndex(mTextureMap[id]->GetImage(), ImageLayout::ShaderRead, mSampler, 0, (uint32_t)id);

    return id;
}

void TextureManager::DestroyTexture(TextureID id)
{
    mTextureMap[id].reset();
}

std::shared_ptr<Texture> TextureManager::GetTexture(TextureID id)
{
    return mTextureMap[id];
}

bool TextureManager::HasTexture(TextureID id)
{
    return mTextureMap.contains(id);
}

uint32_t TextureManager::GetCount()
{
    return mTextureMap.size();
}
const std::unordered_map<TextureID, std::shared_ptr<Texture>> &TextureManager::GetMap()
{
    return mTextureMap;
}

const Descriptor &TextureManager::GetDescriptor()
{
    return mDescriptor;
}
void TextureManager::Clear()
{
    mTextureMap.clear();
}

uint64_t TextureManager::mLastTextureId = 0;
std::unordered_map<TextureID, std::shared_ptr<Texture>> TextureManager::mTextureMap;
Sampler TextureManager::mSampler;
Descriptor TextureManager::mDescriptor;