#pragma once
#include "Core/Core.h"
#include <memory>
#include <vector>

namespace Raytracing
{
    class VulkanBuffer;
    class VulkanCommandPool;
    class VulkanDeviceMemory;
    class VulkanImage;
}

namespace Resources
{
    class Model;
    class Texture;
    class TextureImage;

    class Scene final
    {
    public:
        //Scene(Raytracing::VulkanCommandPool commandPool, std::vector<Model>&& models, std::vector<Texture>&& textures, bool usedForRayTracing);
        //~Scene();

        // const std::vector<Model>& GetModels() const { return m_Models; }
        bool HasProcedurals() const { return static_cast<bool>(m_ProceduralBuffer); }

        const Raytracing::VulkanBuffer& GetVertexBuffer() const { return *m_VertexBuffer; }
        const Raytracing::VulkanBuffer& GetIndexBuffer() const { return *m_IndexBuffer; }
        const Raytracing::VulkanBuffer& GetMaterialBuffer() const { return *m_MaterialBuffer; }
        const Raytracing::VulkanBuffer& GetOffsetBuffer() const { return *m_OffsetBuffer; }
        const Raytracing::VulkanBuffer& GetAABBBuffer() const { return *m_AABBBuffer; }
        const Raytracing::VulkanBuffer& GetProceduralBuffer() const { return *m_ProceduralBuffer; }
        const std::vector<VkImageView>& GetTextureImageViews() const { return m_TextureImageViews; }
        const std::vector<VkSampler>& GetTextureSamplers() const { return m_TextureSamplers; }

    private:
        // const std::vector<Model> m_Models;
        // const std::vector<Texture> m_Textures;

        std::unique_ptr<Raytracing::VulkanBuffer> m_VertexBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_VertexBufferMemory;

        std::unique_ptr<Raytracing::VulkanBuffer> m_IndexBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_IndexBufferMemory;

        std::unique_ptr<Raytracing::VulkanBuffer> m_MaterialBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_MaterialBufferMemory;

        std::unique_ptr<Raytracing::VulkanBuffer> m_OffsetBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_OffsetBufferMemory;

        std::unique_ptr<Raytracing::VulkanBuffer> m_AABBBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_AABBBufferMemory;

        std::unique_ptr<Raytracing::VulkanBuffer> m_ProceduralBuffer;
        std::unique_ptr<Raytracing::VulkanDeviceMemory> m_ProceduralBufferMemory;

        std::vector<std::unique_ptr<TextureImage>> m_TextureImages;
        std::vector<VkImageView> m_TextureImageViews;
        std::vector<VkSampler> m_TextureSamplers;
    };
}