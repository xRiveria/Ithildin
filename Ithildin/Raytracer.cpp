#include "Raytracer.h"
#include "Resources/Scene.h"
#include "Resources/UniformBuffer.h"
#include "Resources/Texture.h"
#include "Resources/Model.h"
#include "Vulkan/VulkanCommandPool.h"
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

Raytracer::Raytracer(const Vulkan::WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode) 
    : Vulkan::Raytracing::RaytracingApplication(windowSettings, requestedPresentationMode, RaytracerUtilities::EnableValidationLayers)
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
    uniformBufferObject.m_Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.1f, 10000.0f);
    uniformBufferObject.m_Projection[1][1] *= -1; // Inverting Y for Vulkan, https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    uniformBufferObject.m_ModelViewInverse = glm::inverse(uniformBufferObject.m_ModelView);
    uniformBufferObject.m_ProjectionInverse = glm::inverse(uniformBufferObject.m_Projection);
    uniformBufferObject.m_Aperture = 0.05f;
    uniformBufferObject.m_FocusDistance = 13.1f;
    uniformBufferObject.m_TotalSamplesCount = m_TotalNumberOfSamples;
    uniformBufferObject.m_SampleCount = m_NumberOfSamples;
    uniformBufferObject.m_BounceCount = 32;
    uniformBufferObject.m_RandomSeed = 1;
    uniformBufferObject.m_HasSky = true;
    uniformBufferObject.m_ShowHeatMap = false;
    uniformBufferObject.m_HeatmapScale = 1.0f;

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

    LoadScene(0);

    CreateAccelerationStructures();
}

void Raytracer::CreateSwapChain()
{
    RaytracingApplication::CreateSwapChain();
    /// User Interface Stuff
    CheckFramebufferSize();
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

    m_ModelViewController.Reset(m_CameraInitialState.m_ModelView);

    m_ResetAccumulation = true;
}

void Raytracer::DeleteSwapChain()
{
    /// Delete UI
    RaytracingApplication::DeleteSwapChain();
}

void Raytracer::DrawFrame()
{
    if (m_ResetAccumulation)
    {
        m_TotalNumberOfSamples = 0;
        m_ResetAccumulation = false;
    }

    // Keep track of our sample count.
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

    // Render the scene.
    if (true)
    {
        Vulkan::Raytracing::RaytracingApplication::Render(commandBuffer, imageIndex);
    }
    else
    {
        Vulkan::Application::Render(commandBuffer, imageIndex);
    }
}

void Raytracer::OnKey(int key, int scanCode, int action, int mods)
{
}

void Raytracer::OnCursorMoved(double xPosition, double yPosition)
{
}

void Raytracer::OnMouseButton(int button, int action, int mods)
{
}

void Raytracer::OnScroll(double xOffset, double yOffset)
{
}

void Raytracer::CheckFramebufferSize() const
{
}
