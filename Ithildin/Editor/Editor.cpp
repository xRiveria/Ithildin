#include "Editor.h"
#include "UserSettings.h"
#include "SceneList.h"
#include "Core/Window.h"
#include "Vulkan/VulkanDescriptorPool.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanFramebuffer.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/SingleTimeCommands.h"
#include "Vulkan/VulkanSurface.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanRenderPass.h"
#include "../Resources/Scene.h"
#include "Backend/Source/imgui.h"
#include "Backend/Source/imgui_impl_glfw.h"
#include "Backend/Source/imgui_impl_vulkan.h"

namespace EditorUtilities
{
    void CheckVulkanResultCallback(const VkResult error)
    {
        if (error != VK_SUCCESS)
        {
            std::runtime_error(std::string("ImGui Vulkan Error: (") + Vulkan::ToString(error) + ")");
        }
    }
}

Editor::Editor(Vulkan::VulkanCommandPool& commandPool, const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanDepthBuffer& depthBuffer, UserSettings& userSettings)
    : m_UserSettings(userSettings)
{
    const Vulkan::VulkanDevice& device = swapChain.GetDevice();
    const Vulkan::Window& window = device.GetSurface().GetInstance().GetWindow();

    // Initialize descriptor pool and render pass for ImGui.
    const std::vector<Vulkan::VulkanDescriptorBinding> descriptorBindings =
    {
        { 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 }
    };

    // Initialize Descriptor Pool and Render Pass.
    m_DescriptorPool.reset(new Vulkan::VulkanDescriptorPool(device, descriptorBindings, 1));
    // Set as LoadOp as we want to draw over our main rendering.
    m_RenderPass.reset(new Vulkan::VulkanRenderPass(swapChain, depthBuffer, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_LOAD_OP_LOAD));

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Initialize ImGui Vulkan Renderer
    if (!ImGui_ImplGlfw_InitForVulkan(window.GetWindowHandle(), false))
    {
        std::runtime_error("Failed to initialize ImGui GLFW Adapter.\n");
    }

    // Initialize ImGui Vulkan Adapter
    ImGui_ImplVulkan_InitInfo vulkanInitializationInfo = {};
    vulkanInitializationInfo.Instance = device.GetSurface().GetInstance().GetHandle();
    vulkanInitializationInfo.PhysicalDevice = device.GetPhysicalDevice();
    vulkanInitializationInfo.Device = device.GetHandle();
    vulkanInitializationInfo.QueueFamily = device.GetGraphicsQueueFamilyIndex();
    vulkanInitializationInfo.Queue = device.GetGraphicsQueue();
    vulkanInitializationInfo.PipelineCache = nullptr;
    vulkanInitializationInfo.DescriptorPool = m_DescriptorPool->GetHandle();
    vulkanInitializationInfo.MinImageCount = swapChain.GetMinimumImageCount();
    vulkanInitializationInfo.ImageCount = static_cast<uint32_t>(swapChain.GetImages().size());
    vulkanInitializationInfo.Allocator = nullptr;
    vulkanInitializationInfo.CheckVkResultFn = EditorUtilities::CheckVulkanResultCallback;

    if (!ImGui_ImplVulkan_Init(&vulkanInitializationInfo, m_RenderPass->GetHandle()))
    {
        std::runtime_error("Failed to initialize ImGui Vulkan Adapter.\n");
    }

    ImGuiIO& io = ImGui::GetIO();

    // No Ini File
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    // Font
    Vulkan::SingleTimeCommands::Submit(commandPool, [](VkCommandBuffer commandBuffer)
    {
        if (ImGui_ImplVulkan_CreateFontsTexture(commandBuffer))
        {
            std::runtime_error("Failed to create ImGui Font Textures.\n");
        }
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

Editor::~Editor()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Render(VkCommandBuffer commandBuffer, const Vulkan::VulkanFramebuffer& frameBuffer, const Statistics& statistics)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    DrawSettings();
    DrawOverlay(statistics);
    ImGui::Render();

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass->GetHandle();
    renderPassInfo.framebuffer = frameBuffer.GetHandle();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_RenderPass->GetSwapChain().GetExtent();
    renderPassInfo.clearValueCount = 0;
    renderPassInfo.pClearValues = nullptr;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
}

bool Editor::WantsToCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool Editor::WantsToCaptureMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

void Editor::DrawSettings()
{
    if (!GetSettings().m_ShowSettings)
    {
        return;
    }

    const float distance = 10.0f;
    const ImVec2 pos = ImVec2(distance, distance);
    const ImVec2 posPivot = ImVec2(0.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);

    const auto flags =
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("Settings", &GetSettings().m_ShowSettings, flags))
    {
        std::vector<const char*> scenes;
        for (const auto& scene : SceneList::s_AllScenes)
        {
            scenes.push_back(scene.first.c_str());
        }

        const Vulkan::Window& window = m_DescriptorPool->GetDevice().GetSurface().GetInstance().GetWindow();

        ImGui::Text("Help");
        ImGui::Separator();
        ImGui::BulletText("F1: Toggle Settings.");
        ImGui::BulletText("F2: Toggle Statistics.");
        ImGui::BulletText(
            "%c%c%c%c/Shift/CTRL: Move Camera.",
            std::toupper(window.GetKeyName(GLFW_KEY_W, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_A, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_S, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_D, 0)[0]));
        ImGui::BulletText("L/R Mouse: Rotate Camera/Scene.");
        ImGui::NewLine();

        ImGui::Text("Scene");
        ImGui::Separator();
        ImGui::PushItemWidth(-1);
        ImGui::Combo("", &GetSettings().m_SceneIndex, scenes.data(), static_cast<int>(scenes.size()));
        ImGui::PopItemWidth();
        ImGui::NewLine();

        ImGui::Text("Ray Tracing");
        ImGui::Separator();
        ImGui::Checkbox("Enable Ray Tracing", &GetSettings().m_IsRaytracingEnabled);
        ImGui::Checkbox("Accumulate Rays between Frames", &GetSettings().m_IsRayAccumulationEnabled);
        uint32_t min = 1, max = 128;
        ImGui::SliderScalar("Samples", ImGuiDataType_U32, &GetSettings().m_NumberOfSamples, &min, &max);
        min = 1, max = 32;
        ImGui::SliderScalar("Bounces", ImGuiDataType_U32, &GetSettings().m_NumberOfBounces, &min, &max);
        ImGui::NewLine();

        ImGui::Text("Camera");
        ImGui::Separator();
        ImGui::SliderFloat("FOV", &GetSettings().m_FieldOfView, UserSettings::m_FieldOfViewValueMinimum, UserSettings::m_FieldOfViewValueMaximum, "%.0f");
        ImGui::SliderFloat("Aperture", &GetSettings().m_Aperture, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Focus", &GetSettings().m_FocusDistance, 0.1f, 20.0f, "%.1f");
        ImGui::NewLine();

        ImGui::Text("Profiler");
        ImGui::Separator();
        ImGui::Checkbox("Show Heatmap", &GetSettings().m_ShowHeatmap);
        ImGui::SliderFloat("Scaling", &GetSettings().m_HeatmapScale, 0.10f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::NewLine();
    }

    ImGui::End();
}

void Editor::DrawOverlay(const Statistics& statistics)
{
    if (!GetSettings().m_ShowOverlay)
    {
        return;
    }

    const auto& io = ImGui::GetIO();
    const float distance = 10.0f;
    const ImVec2 pos = ImVec2(io.DisplaySize.x - distance, distance);
    const ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background

    const auto flags =
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoNav |
                        ImGuiWindowFlags_NoSavedSettings;

    if (ImGui::Begin("Statistics", &GetSettings().m_ShowOverlay, flags))
    {
        ImGui::Text("Statistics (%dx%d):", statistics.m_FramebufferSize.width, statistics.m_FramebufferSize.height);
        ImGui::Separator();
        ImGui::Text("Frame Rate: %.1f FPS", statistics.m_FrameRate);
        ImGui::Text("Primary Ray Rate: %.2f Gr/s", statistics.m_RayRate);
        ImGui::Text("Accumulated Samples:  %u", statistics.m_TotalSamples);
    }

    ImGui::End();
}
