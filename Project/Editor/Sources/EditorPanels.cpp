#include "EditorPanels.hpp"
#include "ImGuiHelper.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

GameViewPanel::GameViewPanel(RenderTarget *renderTarget, Camera *camera, CameraController *controller)
    : mTarget(renderTarget), mCamera(camera), mController(controller) 
{
    SetTitle("Game View");
    SetIcon('R');
}

void GameViewPanel::OnAttach() 
{
    mTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(
    Application::GetInstance()->GetRendererRef().GetDefaultSampler().GetHandle(), mTarget->GetImage().view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
void GameViewPanel::OnUpdate() 
{
    if (mTarget->GetImage().size != mSize) 
    {
        vkDeviceWaitIdle(getDevice());
        mTarget->Resize(mSize);

        VkDescriptorImageInfo desc_image[1] = {};
        desc_image[0].sampler = Application::GetInstance()->GetRendererRef().GetDefaultSampler().GetHandle();
        desc_image[0].imageView = mTarget->GetImage().view;
        desc_image[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkWriteDescriptorSet write_desc[1] = {};
        write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc[0].dstSet = (VkDescriptorSet)mTexture;
        write_desc[0].descriptorCount = 1;
        write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_desc[0].pImageInfo = desc_image;
        vkUpdateDescriptorSets(getDevice(), 1, write_desc, 0, nullptr);
        mCamera->SetAspectRatio(float(mSize.x) / float(mSize.y));
    }
}
void GameViewPanel::OnRenderUi() 
{
    ImGui::Begin(GetTitle().c_str(), &mEnabled, ImGuiWindowFlags_NoCollapse);

    mSize = glm::uvec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

    ImGui::Image(mTexture, ImVec2(mSize.x, mSize.y), ImVec2(0, 1), ImVec2(1, 0));
    mController->EnableKeyboardControl(ImGui::IsItemHovered());
    mController->EnableMouseControl(ImGui::IsItemHovered() &&
    ImGui::IsMouseDown(ImGuiMouseButton_Left));

    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        Application::GetInstance()->HideCursor();
    else
        Application::GetInstance()->ShowCursor();

    ImGui::End();
}

PerformancePanel::PerformancePanel()
{
    SetTitle("Performance");
    SetIcon('s');
}

void PerformancePanel::OnRenderUi() 
{
    ImGui::Begin(GetTitle().c_str(), &mEnabled);

    const uint32_t count = 512;

    struct FrameData
    {
        float values[count];
        float sortedValues[count];
        float max = 0;
        float average = 0;

    } static frameData;

    for (int i = 0; i < count; i++)
    {
        if(i == count - 1)
            frameData.values[count-1] = Application::GetInstance()->GetDeltaTime();
        else
            frameData.values[i] = frameData.values[i+1];
        
        if(frameData.max < frameData.values[i])
            frameData.max = frameData.values[i];
        frameData.average += frameData.values[i];
    }

    memcpy(frameData.sortedValues, frameData.values, sizeof(frameData.values));
    std::sort(&frameData.sortedValues[0], &frameData.sortedValues[count], std::greater<float>());

    uint32_t onePercentFrameCount = count * 0.01f;

    float onePercentAverage = 0;
    for (int i = 0; i < onePercentFrameCount; i++)
    {
        onePercentAverage += frameData.sortedValues[i];
    }

    onePercentAverage /= onePercentFrameCount;

    uint32_t zeroPointOnePercentFrameCount = count * 0.001f;
    float zeroPointOnePercentAverage = 0;
    for (int i = 0; i < zeroPointOnePercentFrameCount; i++)
    {
        zeroPointOnePercentAverage += frameData.sortedValues[i];
    }

    zeroPointOnePercentAverage /= zeroPointOnePercentFrameCount;

    frameData.average /= float(count);
    static int zoom = 0;
    
    if(ImGuiHelper::IconButton('W'))
    {
        memset(frameData.values, 0, sizeof(frameData.values));
    }
    ImGui::SameLine();
    ImGui::SliderInt("Zoom", &zoom, 0, count - 1);
    ImGui::Text("Sample count: %u", count);
    if(frameData.values[0] == 0)
    {
        ImGui::Text("Frame average: --.--");
        ImGui::Text("Fps average: --.--");
        ImGui::Text("%%1 Fps: --.--");
        ImGui::Text("%%0.1 Fps: --.--");
    }
    else 
    {
        ImGui::Text("Frame average: %.4f", frameData.average);
        ImGui::Text("Fps average: %.2f", 1.f / frameData.average);
        ImGui::Text("%%1 Fps: %0.2f", 1.f / onePercentAverage);
        ImGui::Text("%%0.1 Fps: %f", 1.f / zeroPointOnePercentAverage);
    }

    ImGui::Text("Fps: %d", Application::GetInstance()->GetFps());
    ImGui::PlotLines("Frame time", frameData.values + zoom, count - zoom, 0, NULL, 0, frameData.max, {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f});
    ImGui::End();    
}

ImageViewerPanel::ImageViewerPanel()
{
    SetTitle("Image Viewer");
    SetIcon('6');
}

void ImageViewerPanel::OnAttach() 
{
}

void ImageViewerPanel::OnUpdate() 
{
    if(mImageMap.contains(mSelection))
    {
        if(mTexture == 0)
            mTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(Application::GetInstance()->GetRendererRef().GetDefaultSampler().GetHandle(), mImageMap[mSelection]->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        else
        {
            VkDescriptorImageInfo imageInfo = 
            {
                .sampler = Application::GetInstance()->GetRendererRef().GetDefaultSampler().GetHandle(), 
                .imageView = mImageMap[mSelection]->view,
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            };
            
            VkWriteDescriptorSet writeDescriptor = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = (VkDescriptorSet)mTexture,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            };
            
            vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptor, 0, nullptr);
        }
    }
}

void ImageViewerPanel::OnRenderUi() 
{
    ImGui::Begin(GetTitle().c_str(), &mEnabled);

    if(ImGui::BeginCombo("Images", mSelection.c_str()))
    {
        for(const auto& [name, image] : mImageMap)
        {
            if(ImGui::Selectable(name.c_str(), name == mSelection))
            {
                mSelection = name;
                ImGui::EndCombo();
                ImGui::End();
                return;
            }
        }
        ImGui::EndCombo();
    }

    if(mTexture != 0)
    {
        auto nsize = mImageMap[mSelection]->size;
        ImVec2 size = {float(nsize.x), float(nsize.y)};
        ImGui::Image((ImTextureID)mTexture, size, {0,1}, {1,0});
    }
    ImGui::End();
}

void ImageViewerPanel::AddImage(std::string identifier, Image* image) 
{
    mImageMap[identifier] = image;
}

EntityPanel::EntityPanel()
{
    SetTitle("Entity");
    SetIcon('Q');
}

void EntityPanel::OnRenderUi() 
{
    ImGui::Begin(GetTitle().c_str(), &mEnabled, ImGuiWindowFlags_NoCollapse);

    if(mScene == nullptr) { ImGui::End(); return; }

    bool focus = false;
    static bool createEntityBox = false;

    static std::string search;

    ImGui::InputText("Search", &search);

    search = ImGuiHelper::toLower(search);

    if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("Right click menu");
    }

    static bool cfocus = false;

    if(ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        cfocus = true;
        createEntityBox = true;
    }

    if(ImGui::BeginPopup("Right click menu"))
    {
        if(ImGui::Button("Create entity"))
        {
            cfocus = true;
            createEntityBox = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


    auto entities = mScene->GetEntities<EntityMetadata>();

    for (auto& [entity, metadata] : entities) 
    {
        ImGui::PushID((uint32_t)entity.GetId());
        if(search.size() != 0)
        {
            std::string name = ImGuiHelper::toLower(metadata.name);
            if(!name.contains(search))
            {
                ImGui::PopID();
                continue;
            }
        }
        bool selected = false;
        if(mSelectedEntity.IsValid())
        {
            if(mSelectedEntity == entity)
                selected = true;
        }
        ImGuiHelper::IconCharacterSameLine('Q');
        if(ImGui::Selectable(metadata.name.c_str(), selected))
        {
            mSelectedEntity = entity;
        }
        
        ImGui::PopID();
    }

    if(createEntityBox)
    {
        static std::string name;

        ImGuiHelper::IconCharacterSameLine('r');
        if(ImGui::InputText("##name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            mScene->CreateEntity(name);
            createEntityBox = false;
        }
        
        if(!ImGui::IsItemActive() && cfocus == false)
        {
            createEntityBox = false;
        }

        if(cfocus)
        {
            ImGui::ActivateItemByID(ImGui::GetItemID());
            cfocus = false;
        }

        if(ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            createEntityBox = false;
        }
    }

    ImGui::End();
}

PropertyPanel::PropertyPanel()
{
    SetTitle("Properties");
    SetIcon('q');
}

void PropertyPanel::SetEntity(Entity entity) 
{
    mEntity = entity;    
}

void PropertyPanel::OnRenderUi() 
{
    ImGui::Begin(GetTitle().c_str(), &mEnabled, ImGuiWindowFlags_NoCollapse);

    if(!mEntity.IsValid())
    {
        ImGui::End();
        return;
    }

    if(ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("Components");
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, {0.5, 0.5});
    }

    if(ImGui::BeginPopup("Components"))
    {
        if((ImGui::Button("Transform")))
        {
            mEntity.AddComponent<Transform>();
        }
        ImGui::EndPopup();
    }


    ImGui::SeparatorText("Entity");
    ImGui::Text("Id: %d", mEntity.GetId());

    std::string& name = mEntity.GetComponent<EntityMetadata>().name;
    ImGui::InputText("Name", &name);
    if(name.size() == 0)
        name = "Untitled";


    if(mEntity.HasComponent<Transform>())
    {
        ImGui::SeparatorText("Transform");
        Transform& transform = mEntity.GetComponent<Transform>();

        bool hideCursor = false;

        ImGuiHelper::DragVec3("Position", transform.position, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) ? true : hideCursor;
        ImGuiHelper::DragVec3("Rotation", transform.rotation, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) ? true : hideCursor;
        ImGuiHelper::DragVec3("Scale", transform.scale, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) ? true : hideCursor;

        if(hideCursor)
            Application::GetInstance()->HideCursor();
    }


    ImGui::End();

}
