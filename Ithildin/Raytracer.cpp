#include "Raytracer.h"
#include "Resources/Scene.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Texture.h"
#include "Resources/Model.h"
#include "Vulkan/VulkanSwapChain.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/VulkanDevice.h"
#include "Core/Window.h"

namespace RaytracerUtilities
{
    const bool EnableValidationLayers =
#ifdef NDEBUG
        false;
#else
        true;
#endif
}

Raytracer::Raytracer(const UserSettings& userSettings, const Vulkan::WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode)
                   : Vulkan::Raytracing::RaytracingApplication(windowSettings, requestedPresentationMode, RaytracerUtilities::EnableValidationLayers), 
                     m_UserSettings(userSettings)
{
    CheckFramebufferSize();
}

Raytracer::~Raytracer()
{
    m_Scene.reset();
}

const Resources::Scene& Raytracer::GetScene() const
{
    return *m_Scene;
}

Resources::UniformBufferObject Raytracer::GetUniformBufferObject(VkExtent2D extent) const
{
    const SceneList::CameraInitialState& initialState = m_CameraInitialState;

    Resources::UniformBufferObject uniformBufferObject = {};
    uniformBufferObject.m_ModelView = m_ModelViewController.GetModelView();
    uniformBufferObject.m_Projection = glm::perspective(glm::radians(m_UserSettings.m_FieldOfView), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.1f, 10000.0f);
    uniformBufferObject.m_Projection[1][1] *= -1; // Inverting Y for Vulkan, https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    uniformBufferObject.m_ModelViewInverse = glm::inverse(uniformBufferObject.m_ModelView);
    uniformBufferObject.m_ProjectionInverse = glm::inverse(uniformBufferObject.m_Projection);
    uniformBufferObject.m_Aperture = m_UserSettings.m_Aperture;
    uniformBufferObject.m_FocusDistance = m_UserSettings.m_FocusDistance;
    uniformBufferObject.m_TotalSamplesCount = m_TotalNumberOfSamples;
    uniformBufferObject.m_SampleCount = m_NumberOfSamples;
    uniformBufferObject.m_BounceCount = m_UserSettings.m_NumberOfBounces;
    uniformBufferObject.m_RandomSeed = 1;
    uniformBufferObject.m_HasSky = true;
    uniformBufferObject.m_ShowHeatMap = m_UserSettings.m_ShowHeatmap;
    uniformBufferObject.m_HeatmapScale = m_UserSettings.m_HeatmapScale;

    return uniformBufferObject;
}

void Raytracer::SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures)
{
    // Required Extensions
    requiredExtensions.insert(requiredExtensions.end(),
    {
        VK_KHR_SHADER_CLOCK_EXTENSION_NAME // Required for heatmap.
    });

    // Opt-in into mandary device features.
    VkPhysicalDeviceShaderClockFeaturesKHR shaderClockFeatures = {};
    shaderClockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
    shaderClockFeatures.pNext = nextDeviceFeatures;
    shaderClockFeatures.shaderSubgroupClock = true;

    deviceFeatures.fillModeNonSolid = true;
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.shaderInt64 = true;

    RaytracingApplication::SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, &shaderClockFeatures);
}

void Raytracer::OnDeviceSet()
{
    RaytracingApplication::OnDeviceSet();

    LoadScene(m_UserSettings.m_SceneIndex);

    CreateAccelerationStructures();
}

void Raytracer::CreateSwapChain()
{
    RaytracingApplication::CreateSwapChain();
    
    m_Editor.reset(new Editor(GetCommandPool(), GetSwapChain(), GetDepthBuffer(), m_UserSettings));
    m_ResetAccumulation = true;

    CheckFramebufferSize();
}

void Raytracer::DeleteSwapChain()
{
    m_Editor.reset();
    RaytracingApplication::DeleteSwapChain();
}

void Raytracer::DrawFrame()
{
    // Check if the scene has been changed by the user.
    if (m_SceneIndex != static_cast<uint32_t>(m_UserSettings.m_SceneIndex))
    {
        // Finish outstanding operations.
        GetDevice().WaitIdle();
        DeleteSwapChain();
        DeleteAccelerationStructures();
        LoadScene(m_UserSettings.m_SceneIndex);
        CreateAccelerationStructures();
        CreateSwapChain();
        return;
    }

    if (m_ResetAccumulation || m_UserSettings.RequireAccumulationReset(m_PreviousSettings) || !m_UserSettings.m_IsRayAccumulationEnabled)
    {
        m_TotalNumberOfSamples = 0;
        m_ResetAccumulation = false;
    }

    m_PreviousSettings = m_UserSettings;

    // Keep track of our sample count.
    m_NumberOfSamples = glm::clamp(m_UserSettings.m_MaxNumberOfSamples - m_TotalNumberOfSamples, 0u, m_UserSettings.m_NumberOfSamples);
    m_TotalNumberOfSamples += m_NumberOfSamples;

    Application::DrawFrame();
}


void Raytracer::Render(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // Record delta time between calls to Render.
    const auto previousTime = m_Time;
    m_Time = GetWindow().GetTime();
    const auto deltaTime = m_Time - previousTime;

    // Update the camera position/angle.
    m_ResetAccumulation = m_ModelViewController.UpdateCamera(m_CameraInitialState.m_ControlSpeed, deltaTime);

    // Check the current state of the benchmark and update it for the new frame.
    CheckAndUpdateBenchmarkState(previousTime);

    // Render the scene.
    if (m_UserSettings.m_IsRaytracingEnabled)
    {
        Vulkan::Raytracing::RaytracingApplication::Render(commandBuffer, imageIndex);
    }
    else
    {
        Vulkan::Application::Render(commandBuffer, imageIndex);
    }

    // Render the UI
    Statistics statistics = {};
    statistics.m_FramebufferSize = GetWindow().GetFramebufferSize();
    statistics.m_FrameRate = static_cast<float>(1 / deltaTime);

    if (m_UserSettings.m_IsRaytracingEnabled)
    {
        const VkExtent2D extent = GetSwapChain().GetExtent();

        statistics.m_RayRate = static_cast<float>(double(extent.width * extent.height) * m_NumberOfSamples / (deltaTime * 1000000000));
        statistics.m_TotalSamples = m_TotalNumberOfSamples;
    }

    m_Editor->Render(commandBuffer, GetSwapchainFramebuffer(imageIndex), statistics);
}

void Raytracer::LoadScene(uint32_t sceneIndex)
{
    auto [models, textures] = SceneList::s_AllScenes[sceneIndex].second(m_CameraInitialState);

    // If there are no textures, add a dummy one. It makes the pipeline setup a lot easier.
    if (textures.empty())
    {
        textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/White.png", Vulkan::SamplerConfiguration()));
    }

    m_Scene.reset(new Resources::Scene(GetCommandPool(), std::move(models), std::move(textures), true));
    m_SceneIndex = sceneIndex;

    m_UserSettings.m_FieldOfView = m_CameraInitialState.m_FieldOfView;
    m_UserSettings.m_Aperture = m_CameraInitialState.m_Aperture;
    m_UserSettings.m_FocusDistance = m_CameraInitialState.m_FocusDistance;

    m_ModelViewController.Reset(m_CameraInitialState.m_ModelView);

    m_PeriodTotalFrames = 0;
    m_ResetAccumulation = true;
}

void Raytracer::CheckAndUpdateBenchmarkState(double previousTime)
{
    if (!m_UserSettings.m_IsBenchmarkingEnabled)
    {
        return;
    }
}

void Raytracer::OnKey(int key, int scanCode, int action, int mods)
{
    if (m_Editor->WantsToCaptureKeyboard())
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                GetWindow().Close();
                break;

            default:
                break;
        }

        // Settings
        if (!m_UserSettings.m_IsBenchmarkingEnabled)
        {
            switch (key)
            {
                case GLFW_KEY_M: m_UserSettings.m_ShowSettings = !m_UserSettings.m_ShowSettings; break;
                case GLFW_KEY_P: m_UserSettings.m_ShowOverlay = !m_UserSettings.m_ShowOverlay; break;
                case GLFW_KEY_R:  m_UserSettings.m_IsRaytracingEnabled = !m_UserSettings.m_IsRaytracingEnabled; break;
                case GLFW_KEY_H:  m_UserSettings.m_ShowHeatmap = !m_UserSettings.m_ShowHeatmap; break;
                case GLFW_KEY_L:  m_IsWireframe = !m_IsWireframe; break;
                default: break;
            }
        }
    }

    // Camera Motions
    if (!m_UserSettings.m_IsBenchmarkingEnabled)
    {
        m_ResetAccumulation = m_ModelViewController.OnKey(key, scanCode, action, mods);
    }
}

void Raytracer::OnCursorMoved(double xPosition, double yPosition)
{
    if (!HasSwapChain() || m_UserSettings.m_IsBenchmarkingEnabled || m_Editor->WantsToCaptureKeyboard() || m_Editor->WantsToCaptureMouse())
    {
        return;
    }

    // Camera Position
    m_ResetAccumulation = m_ModelViewController.OnCursorPosition(xPosition, yPosition);
}

void Raytracer::OnMouseButton(int button, int action, int mods)
{
    if (!HasSwapChain() || m_UserSettings.m_IsBenchmarkingEnabled || m_Editor->WantsToCaptureMouse())
    {
        return;
    }

    // Camera Motions
    m_ResetAccumulation = m_ModelViewController.OnMouseButton(button, action, mods);
}

void Raytracer::OnScroll(double xOffset, double yOffset)
{
    if (!HasSwapChain() || m_UserSettings.m_IsBenchmarkingEnabled || m_Editor->WantsToCaptureMouse())
    {
        return;
    }

    const auto previousFOV = m_UserSettings.m_FieldOfView;
    m_UserSettings.m_FieldOfView = glm::clamp(static_cast<float>(previousFOV - yOffset), m_UserSettings.m_FieldOfViewValueMinimum, m_UserSettings.m_FieldOfViewValueMaximum);

    m_ResetAccumulation = previousFOV != m_UserSettings.m_FieldOfView;
}

void Raytracer::CheckFramebufferSize() const
{
}
