#include "EditorLayer.hpp"
#include "Core/Application.hpp"
#include "GameLayer.hpp"
#include "ImGuiHelper.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include <string>


void EditorLayer::OnAttach() 
{
    SetRenderTarget(&GetLayer<GameLayer>().mTarget);
    mEditorCameraController.SetCamera(mEditorCamera, Application::GetInstance()->GetWindowRef());
    InitializeImGui();
    LoadImGuiStyle("style.bin", mOriginalStyle);

    ImGui::GetStyle() = mOriginalStyle;

    ImGuiStyle &style = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; i++) 
    {
        ImVec4 &col = style.Colors[i];
        col.x = col.x <= 0.04045f ? col.x / 12.92f
                                : pow((col.x + 0.055f) / 1.055f, 2.4f);
        col.y = col.y <= 0.04045f ? col.y / 12.92f
                                : pow((col.y + 0.055f) / 1.055f, 2.4f);
        col.z = col.z <= 0.04045f ? col.z / 12.92f
                                : pow((col.z + 0.055f) / 1.055f, 2.4f);
    }

    LoadState("state.bin");


    mScene = &GetLayer<GameLayer>().scene;
}

void EditorLayer::LoadState(std::string_view filename)
{
    FILE* fp = fopen(filename.data(), "rb");
    if(fp == nullptr)
        return;

    fread(&mContentPanelEnable, sizeof(bool), 1, fp);
    fread(&mPropertyPanelEnable, sizeof(bool), 1, fp);
    fread(&mCustomizeWindowEnable, sizeof(bool), 1, fp);
    fread(&mDemoWindowEnable, sizeof(bool), 1, fp);
    fread(&mContentPanelEnable, sizeof(bool), 1, fp);
    fread(&mGameViewEnable, sizeof(bool), 1, fp);
    fread(&mEntityPanelEnable, sizeof(bool), 1, fp);
    fread(&mPerformancePanel, sizeof(bool), 1, fp);
    fclose(fp);
}

void EditorLayer::StoreState(std::string_view filename) 
{
    FILE* fp = fopen(filename.data(), "wb");

    fwrite(&mContentPanelEnable, sizeof(bool), 1, fp);
    fwrite(&mPropertyPanelEnable, sizeof(bool), 1, fp);
    fwrite(&mCustomizeWindowEnable, sizeof(bool), 1, fp);
    fwrite(&mDemoWindowEnable, sizeof(bool), 1, fp);
    fwrite(&mContentPanelEnable, sizeof(bool), 1, fp);
    fwrite(&mGameViewEnable, sizeof(bool), 1, fp);
    fwrite(&mEntityPanelEnable, sizeof(bool), 1, fp);
    fwrite(&mPerformancePanel, sizeof(bool), 1, fp);
    fclose(fp);
}

void EditorLayer::UpdateCamera()
{
    mEditorCameraController.Update();
    mEditorCamera.Calculate();
}

void EditorLayer::OnUpdate() 
{
    UpdateCamera();
    RenderImGui();
    ResizeRenderView(mViewSize);

    if(mDisableCursor)
        GetWindow().HideCursor();
    else
        GetWindow().ShowCursor();

    mDisableCursor = false;
}

void EditorLayer::OnDetach() 
{
    TerminateImGui();
}

void EditorLayer::CustomizationWindow()
{
    if(!mCustomizeWindowEnable)
        return;

    ImGuiStyle& style = mOriginalStyle;

    ImGui::Begin("Customization", &mCustomizeWindowEnable, ImGuiWindowFlags_NoCollapse);

    if(ImGui::Button("Load", {ImGui::GetContentRegionAvail().x, 0}))
    {
        LoadImGuiStyle("style.bin", style);
    }

    if(ImGui::Button("Store", {ImGui::GetContentRegionAvail().x, 0}))
    {
        StoreImGuiStyle("style.bin", style);
    }
    
    ImGui::SeparatorText("Colors");

    ImGui::ColorEdit4("Text", &style.Colors[ImGuiCol_Text].x);
    ImGui::ColorEdit4("TextDisabled", &style.Colors[ImGuiCol_TextDisabled].x);
    ImGui::ColorEdit4("WindowBg", &style.Colors[ImGuiCol_WindowBg].x);
    ImGui::ColorEdit4("ChildBg", &style.Colors[ImGuiCol_ChildBg].x);
    ImGui::ColorEdit4("PopupBg", &style.Colors[ImGuiCol_PopupBg].x);
    ImGui::ColorEdit4("Border", &style.Colors[ImGuiCol_Border].x);
    ImGui::ColorEdit4("BorderShadow", &style.Colors[ImGuiCol_BorderShadow].x);
    ImGui::ColorEdit4("FrameBg", &style.Colors[ImGuiCol_FrameBg].x);
    ImGui::ColorEdit4("FrameBgHovered", &style.Colors[ImGuiCol_FrameBgHovered].x);
    ImGui::ColorEdit4("FrameBgActive", &style.Colors[ImGuiCol_FrameBgActive].x);
    ImGui::ColorEdit4("TitleBg", &style.Colors[ImGuiCol_TitleBg].x);
    ImGui::ColorEdit4("TitleBgActive", &style.Colors[ImGuiCol_TitleBgActive].x);
    ImGui::ColorEdit4("TitleBgCollapsed", &style.Colors[ImGuiCol_TitleBgCollapsed].x);
    ImGui::ColorEdit4("MenuBarBg", &style.Colors[ImGuiCol_MenuBarBg].x);
    ImGui::ColorEdit4("ScrollbarBg", &style.Colors[ImGuiCol_ScrollbarBg].x);
    ImGui::ColorEdit4("ScrollbarGrab", &style.Colors[ImGuiCol_ScrollbarGrab].x);
    ImGui::ColorEdit4("ScrollbarGrabHovered", &style.Colors[ImGuiCol_ScrollbarGrabHovered].x);
    ImGui::ColorEdit4("ScrollbarGrabActive", &style.Colors[ImGuiCol_ScrollbarGrabActive].x);
    ImGui::ColorEdit4("CheckMark", &style.Colors[ImGuiCol_CheckMark].x);
    ImGui::ColorEdit4("SliderGrab", &style.Colors[ImGuiCol_SliderGrab].x);
    ImGui::ColorEdit4("SliderGrabActive", &style.Colors[ImGuiCol_SliderGrabActive].x);
    ImGui::ColorEdit4("Button", &style.Colors[ImGuiCol_Button].x);
    ImGui::ColorEdit4("ButtonHovered", &style.Colors[ImGuiCol_ButtonHovered].x);
    ImGui::ColorEdit4("ButtonActive", &style.Colors[ImGuiCol_ButtonActive].x);
    ImGui::ColorEdit4("Header", &style.Colors[ImGuiCol_Header].x);
    ImGui::ColorEdit4("HeaderHovered", &style.Colors[ImGuiCol_HeaderHovered].x);
    ImGui::ColorEdit4("HeaderActive", &style.Colors[ImGuiCol_HeaderActive].x);
    ImGui::ColorEdit4("Separator", &style.Colors[ImGuiCol_Separator].x);
    ImGui::ColorEdit4("SeparatorHovered", &style.Colors[ImGuiCol_SeparatorHovered].x);
    ImGui::ColorEdit4("SeparatorActive", &style.Colors[ImGuiCol_SeparatorActive].x);
    ImGui::ColorEdit4("ResizeGrip", &style.Colors[ImGuiCol_ResizeGrip].x);
    ImGui::ColorEdit4("ResizeGripHovered", &style.Colors[ImGuiCol_ResizeGripHovered].x);
    ImGui::ColorEdit4("ResizeGripActive", &style.Colors[ImGuiCol_ResizeGripActive].x);
    ImGui::ColorEdit4("InputTextCursor", &style.Colors[ImGuiCol_InputTextCursor].x);
    ImGui::ColorEdit4("TabHovered", &style.Colors[ImGuiCol_TabHovered].x);
    ImGui::ColorEdit4("Tab", &style.Colors[ImGuiCol_Tab].x);
    ImGui::ColorEdit4("TabSelected", &style.Colors[ImGuiCol_TabSelected].x);
    ImGui::ColorEdit4("TabSelectedOverline", &style.Colors[ImGuiCol_TabSelectedOverline].x);
    ImGui::ColorEdit4("TabDimmed", &style.Colors[ImGuiCol_TabDimmed].x);
    ImGui::ColorEdit4("TabDimmedSelected", &style.Colors[ImGuiCol_TabDimmedSelected].x);
    ImGui::ColorEdit4("TabDimmedSelectedOverline", &style.Colors[ImGuiCol_TabDimmedSelectedOverline].x);
    ImGui::ColorEdit4("DockingPreview", &style.Colors[ImGuiCol_DockingPreview].x);
    ImGui::ColorEdit4("DockingEmptyBg", &style.Colors[ImGuiCol_DockingEmptyBg].x);
    ImGui::ColorEdit4("PlotLines", &style.Colors[ImGuiCol_PlotLines].x);
    ImGui::ColorEdit4("PlotLinesHovered", &style.Colors[ImGuiCol_PlotLinesHovered].x);
    ImGui::ColorEdit4("PlotHistogram", &style.Colors[ImGuiCol_PlotHistogram].x);
    ImGui::ColorEdit4("PlotHistogramHovered", &style.Colors[ImGuiCol_PlotHistogramHovered].x);
    ImGui::ColorEdit4("TableHeaderBg", &style.Colors[ImGuiCol_TableHeaderBg].x);
    ImGui::ColorEdit4("TableBorderStrong", &style.Colors[ImGuiCol_TableBorderStrong].x);
    ImGui::ColorEdit4("TableBorderLight", &style.Colors[ImGuiCol_TableBorderLight].x);
    ImGui::ColorEdit4("TableRowBg", &style.Colors[ImGuiCol_TableRowBg].x);
    ImGui::ColorEdit4("TableRowBgAlt", &style.Colors[ImGuiCol_TableRowBgAlt].x);
    ImGui::ColorEdit4("TextLink", &style.Colors[ImGuiCol_TextLink].x);
    ImGui::ColorEdit4("TextSelectedBg", &style.Colors[ImGuiCol_TextSelectedBg].x);
    ImGui::ColorEdit4("TreeLines", &style.Colors[ImGuiCol_TreeLines].x);
    ImGui::ColorEdit4("DragDropTarget", &style.Colors[ImGuiCol_DragDropTarget].x);
    ImGui::ColorEdit4("DragDropTargetBg", &style.Colors[ImGuiCol_DragDropTargetBg].x);
    ImGui::ColorEdit4("UnsavedMarker", &style.Colors[ImGuiCol_UnsavedMarker].x);
    ImGui::ColorEdit4("NavCursor", &style.Colors[ImGuiCol_NavCursor].x);
    ImGui::ColorEdit4("NavWindowingHighlight", &style.Colors[ImGuiCol_NavWindowingHighlight].x);
    ImGui::ColorEdit4("NavWindowingDimBg", &style.Colors[ImGuiCol_NavWindowingDimBg].x);
    ImGui::ColorEdit4("ModalWindowDimBg", &style.Colors[ImGuiCol_ModalWindowDimBg].x);
    ImGui::ColorEdit4("COUNT", &style.Colors[ImGuiCol_COUNT].x);

    ImGui::SeparatorText("Properties");

    ImGui::DragFloat("Font size", &ImGui::GetIO().FontGlobalScale);
    ImGui::DragFloat("Indent spaceing", &style.IndentSpacing);
    ImGui::DragFloat("Frame rounding", &style.FrameRounding, 0.01);
    ImGui::DragFloat2("Item spaceing", &style.ItemSpacing.x);
    ImGui::DragFloat2("Item inner spaceing", &style.ItemInnerSpacing.x);
    ImGui::DragFloat2("Frame padding", &style.FramePadding.x, 0.01);
    ImGui::DragFloat2("Cell padding", &style.CellPadding.x, 0.01);
    ImGui::DragFloat2("Window padding", &style.WindowPadding.x, 0.01);

    ImGui::End();
}

void EditorLayer::StoreImGuiStyle(std::string_view filename, const ImGuiStyle& style) 
{
    FILE* fp = fopen(filename.data(), "wb");
    fwrite(&style.Colors, sizeof(style.Colors), 1, fp);
    fclose(fp);
}

void EditorLayer::LoadImGuiStyle(std::string_view filename, ImGuiStyle& style) 
{
    FILE* fp = fopen(filename.data(), "rb");
    if(fp == nullptr)
        return;
    
    fread(&style.Colors, sizeof(style.Colors), 1, fp);
    fclose(fp);
}

void EditorLayer::GameView()
{
    if(!mGameViewEnable)    
        return;


    ImGui::Begin("Game View", &mGameViewEnable, ImGuiWindowFlags_NoCollapse);
    
    mViewSize = glm::uvec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    
    ImGui::Image(mRenderViewTexture, ImVec2(mViewSize.x, mViewSize.y), ImVec2(0,1), ImVec2(1,0));
    mEditorCameraController.EnableKeyboardControl(ImGui::IsItemHovered());
    mEditorCameraController.EnableMouseControl(ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left));

    if(ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        mDisableCursor = true;
    }

    ImGui::End();
}

void EditorLayer::EntityPanel()
{
    if(!mEntityPanelEnable)
        return;

    ImGui::Begin("Entity Panel", &mEntityPanelEnable, ImGuiWindowFlags_NoCollapse);

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

void EditorLayer::PerformancePanel() 
{
    if(!mPerformancePanel)
        return;

    ImGui::Begin("Performance", &mPerformancePanel);

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
            frameData.values[count-1] = GetApplication()->GetDeltaTime();
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

    // float onePercentAverage = 
    // uint32_t onePercentFrameCount = count * 0.001f;

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

    ImGui::Text("Fps: %d", GetApplication()->GetFps());
    ImGui::PlotLines("Frame time", frameData.values + zoom, count - zoom, 0, NULL, 0, frameData.max, {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f});
    ImGui::End();


}

void EditorLayer::PropertyPanel()
{
    if(!mPropertyPanelEnable)    
        return;


    ImGui::Begin("Properties", &mPropertyPanelEnable, ImGuiWindowFlags_NoCollapse);

    if(!mSelectedEntity.IsValid())
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
            mSelectedEntity.AddComponent<Transform>();
        }
        ImGui::EndPopup();
    }


    ImGui::SeparatorText("Entity");
    ImGui::Text("Id: %d", mSelectedEntity.GetId());

    std::string& name = mSelectedEntity.GetComponent<EntityMetadata>().name;
    ImGui::InputText("Name", &name);
    if(name.size() == 0)
        name = "Untitled";


    if(mSelectedEntity.HasComponent<Transform>())
    {
        ImGui::SeparatorText("Transform");
        Transform& transform = mSelectedEntity.GetComponent<Transform>();

        bool hideCursor = false;

        ImGuiHelper::DragVec3("Position", transform.position, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) ? true : hideCursor;
        ImGuiHelper::DragVec3("Rotation", transform.rotation, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) ? true : hideCursor;
        ImGuiHelper::DragVec3("Scale", transform.scale, 0.01f);
        hideCursor = (ImGui::IsItemFocused() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) ? true : hideCursor;

        if(hideCursor)
        {
            mDisableCursor = true;
        }
    }


    ImGui::End();


    
}

void EditorLayer::ControlPanel()
{
    if(!mContentPanelEnable)    
        return;


    ImGui::Begin("Control Panel", &mContentPanelEnable, ImGuiWindowFlags_NoCollapse);
    
    ImGui::SeparatorText("Camera");

    ImGuiHelper::DragVec3("Position", mEditorCamera.GetPositionRef(), 0.01f);
    ImGuiHelper::DragVec3("Front", mEditorCamera.GetFrontRef(), 0.01f);
    ImGuiHelper::DragVec3("Up", mEditorCamera.GetUpRef(), 0.01f);
    ImGui::DragFloat("Fov", &mEditorCamera.GetFovRef(), 0.1f);
    ImGui::DragFloat("Aspect Ratio", &mEditorCamera.GetAspectRatioRef(), 0.1f);
    ImGui::DragFloat("Near Plane", &mEditorCamera.GetNearPlaneRef(), 0.1f);
    ImGui::DragFloat("Far Plane", &mEditorCamera.GetFarPlaneRef(), 0.1f);

    ImGui::SeparatorText("Camera Controller");

    ImGui::DragFloat("Speed", &mEditorCameraController.GetSpeedRef());
    ImGui::DragFloat("Sensitivity", &mEditorCameraController.GetSensitivityRef());

    ImGui::End();
}

void EditorLayer::RenderUI() 
{
    if(mDemoWindowEnable)
        ImGui::ShowDemoWindow(&mDemoWindowEnable);

    GameView();
    ControlPanel();
    CustomizationWindow();
    EntityPanel();
    PropertyPanel();
    PerformancePanel();
    MainMenuBar();
}

void IconWindowToggle(std::string_view label, char iconCharacter, bool& opened)
{
    ImGuiHelper::IconCharacterSameLine(iconCharacter);
    if(ImGui::MenuItem(label.data()))
    {
        opened = !opened;
    }
}

void EditorLayer::MainMenuBar()
{
    if(!ImGui::BeginMainMenuBar()) return;

    if(ImGui::BeginMenu("File"))
    {
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Edit"))
    {
        ImGuiHelper::IconCharacter('W');
        ImGui::SameLine();
        if(ImGui::MenuItem("Reload icon font"))
        {
            ImGui::GetIO().Fonts->RemoveFont(ImGuiHelper::iconFont);
            ImGuiHelper::iconFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("./icon.ttf");
        }
        ImGuiHelper::IconCharacter('W');
        ImGui::SameLine();
        if(ImGui::MenuItem("Reload material"))
        {
            GetLayer<GameLayer>().ReloadMaterial();
            GetRenderer().mLighting.pipeline.Destroy();
            GetRenderer().mLighting.pipeline.LoadShader("Shaders/swapchain.comp.spv");
            GetRenderer().mLighting.pipeline.Create({&GetRenderer().mLighting.descriptor, &GetRenderer().mDeferred.descriptor, &GetRenderer().mLighting.uniformDescriptor});
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("View"))
    {
        IconWindowToggle("Customization", '@', mCustomizeWindowEnable);
        IconWindowToggle("Demo", '\\', mDemoWindowEnable);
        IconWindowToggle("Control", 'Q', mContentPanelEnable);
        IconWindowToggle("Game", 'R', mGameViewEnable);
        IconWindowToggle("Property", '<', mPropertyPanelEnable);
        IconWindowToggle("Entity", 'r', mEntityPanelEnable);
        IconWindowToggle("Performance", 's', mPerformancePanel);
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Window"))
    {
        ImGuiHelper::IconCharacterSameLine('A');
        if (ImGui::MenuItem("Close"))
        {
            Application::GetInstance()->Close();
        }

        ImGuiHelper::IconCharacterSameLine(GetWindow().isFullscreen() ? '?' : '>');
        if(ImGui::MenuItem("Toggle Fullscreen"))
        {
            GetWindow().SetFullscreen(!GetWindow().isFullscreen());
        }

        
        if(GetWindow().IsMaximized())
        {
            ImGuiHelper::IconCharacterSameLine('m');
            if(ImGui::MenuItem("Minimize"))
                GetWindow().Restore();
        }
        else
        {
            ImGuiHelper::IconCharacterSameLine('n');
            if(ImGui::MenuItem("Maximize"))
                GetWindow().Maximize();

        }

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void EditorLayer::SetRenderTarget(RenderTarget *renderTarget) 
{
    mRenderTarget = renderTarget;
}
const Camera &EditorLayer::GetEditorCamera() const { return mEditorCamera; }

bool EditorLayer::OnEvent(uint32_t code, void *data) 
{
    WindowEvent windowEvent = (WindowEvent)code;

    if(windowEvent == WindowEvent::WindowResize)
    {
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            vkDeviceWaitIdle(getDevice());
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(GetRenderer().GetSwapchain().GetSize(), 
            {GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
    return false;
}


void EditorLayer::CustomStyle() 
{
    ImGui::GetIO().Fonts->AddFontFromFileTTF("/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf", 18);
    ImGuiHelper::iconFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("./icon.ttf", 16);

    ImGuiStyle &style = ImGui::GetStyle(); 
    style.FramePadding = ImVec2(8, 6);
    style.WindowPadding = ImVec2(8,8);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.FrameRounding = 5.5f;
}

void EditorLayer::InitializeImGui() 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    CustomStyle();

    VkDescriptorPoolSize poolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
    };


    VkDescriptorPoolCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(getDevice(), &createInfo, nullptr, &descriptorPool);

    mImGuiRenderPass.AddAttachment(GetRenderer().GetSwapchain().GetFormat(), ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mImGuiRenderPass.AddSubpass({0}, {}, UINT32_MAX, PipelineBindPoint::Graphic);
    mImGuiRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mImGuiRenderPass.CreateRenderPass();
    
    ImGui_ImplVulkan_InitInfo initInfo = 
    {
        .Instance = getInstance(),
        .PhysicalDevice = getPhysicalDevice(),
        .Device = getDevice(),
        .QueueFamily = getQueueIndices().graphics,
        .Queue = getQueues().graphics,
        .DescriptorPool = descriptorPool,
        .MinImageCount = GetRenderer().GetSwapchain().GetImageCount(),
        .ImageCount = GetRenderer().GetSwapchain().GetImageCount(),
        .PipelineInfoMain = 
        {
            .RenderPass = mImGuiRenderPass.GetHandle(),
            .Subpass = 0,
            .SwapChainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        },
    };

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::GetInstance()->GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    mImGuiFrameBuffer.reserve(GetRenderer().GetSwapchain().GetImageCount());
    for (int i = 0; i < GetRenderer().GetSwapchain().GetImageCount(); i++)
    {
        mImGuiFrameBuffer.emplace_back(GetRenderer().GetSwapchain().GetSize(), std::initializer_list<Image>{GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
    }

    mImGuiCommandBuffer.Create();
    mImageAcquiredSemaphore.Create();
    mRenderingFinished.Create();

    if(mRenderTarget != nullptr)
        mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRenderer().GetDefaultSampler().GetHandle(), mRenderTarget->GetImage().view, VK_IMAGE_LAYOUT_GENERAL);
}

void EditorLayer::TerminateImGui() 
{
    StoreState("state.bin");

    vkDeviceWaitIdle(getDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorLayer::RenderImGui() 
{
    ImGui_ImplVulkan_NewFrame();    
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    RenderUI();

    ImGui::Render();

    uint32_t imageIndex = GetRenderer().GetSwapchain().GetNextImageIndex(mImageAcquiredSemaphore, {});

    mImGuiCommandBuffer.BeginRecording();

    mImGuiRenderPass.CmdBeginRenderPass(mImGuiCommandBuffer, mImGuiFrameBuffer[imageIndex], Application::GetInstance()->GetRendererRef().GetSwapchain().GetSize(), {{0,0,0,1}});

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuiCommandBuffer.GetHandle());

    mImGuiRenderPass.CmdEndRenderPass(mImGuiCommandBuffer);

    mImGuiCommandBuffer.EndRecording();

    mImGuiCommandBuffer.QueueSubmit(getQueues().graphics, mImageAcquiredSemaphore, mRenderingFinished);

    VkSemaphore waitSemaphores[] = {mRenderingFinished.GetHandle()};
    VkSwapchainKHR swapchains[] = {GetRenderer().GetSwapchain().GetHandle()};

    VkPresentInfoKHR presentInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(getQueues().present, &presentInfo);    

    if(Application::GetInstance()->GetRendererRef().ResizeSwapchain(Application::GetInstance()->GetWindowRef().GetSize()))
    {
        vkDeviceWaitIdle(getDevice());
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(GetRenderer().GetSwapchain().GetSize(), 
            {GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
    
}

void EditorLayer::ResizeRenderView(const glm::uvec2& size)
{
    if(mRenderTarget == nullptr)
        return;
    if(mRenderTarget->GetImage().size == size)
        return;

    vkDeviceWaitIdle(getDevice());

    mRenderTarget->Resize(size);
    mRenderTarget->TransitionLayout(ImageLayout::General);

    VkDescriptorImageInfo imageInfo = 
    {
        .sampler = GetRenderer().GetDefaultSampler().GetHandle(), 
        .imageView = mRenderTarget->GetImage().view,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    VkWriteDescriptorSet writeDescriptor = 
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = (VkDescriptorSet)mRenderViewTexture,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
    };

    vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptor, 0, nullptr);

    mEditorCamera.SetAspectRatio(float(size.x) / float(size.y));
}