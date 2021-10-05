#pragma once
#include "SceneList.h"
#include "Vulkan/Raytracing/RaytracingApplication.h"
#include "ModelViewController.h"

class Raytracer final : public Vulkan::Raytracing::RaytracingApplication
{
public:
    Raytracer(const Vulkan::WindowSettings& windowSettings, VkPresentModeKHR requestedPresentationMode);
    ~Raytracer();

protected:
    virtual Resources::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const override;

    virtual void SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures) override;
    virtual void OnDeviceSet() override;
    virtual void CreateSwapChain() override;
    virtual void DeleteSwapChain() override;
    virtual void DrawFrame() override;
    virtual void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

    virtual void OnKey(int key, int scanCode, int action, int mods) override;
    virtual void OnCursorMoved(double xPosition, double yPosition) override;
    virtual void OnMouseButton(int button, int action, int mods) override;
    virtual void OnScroll(double xOffset, double yOffset) override;

    const Resources::Scene& GetScene() const override;

private:
    void CheckFramebufferSize() const;
    void LoadScene(uint32_t sceneIndex);

private:
    SceneList::CameraInitialState m_CameraInitialState;
    std::unique_ptr<const Resources::Scene> m_Scene;
    uint32_t m_SceneIndex = 0;

    ModelViewController m_ModelViewController;
    double m_Time = {};

    uint32_t m_TotalNumberOfSamples = 0;
    uint32_t m_NumberOfSamples = 64;
    bool m_ResetAccumulation = false;

};