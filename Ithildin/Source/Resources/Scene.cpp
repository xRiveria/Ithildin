#include "Scene.h"
#include "Vulkan/VulkanDebugUtilities.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanBufferUtilities.h"
#include "Vulkan/VulkanImage.h"
#include "Vulkan/VulkanImageView.h"
#include "Vulkan/SingleTimeCommands.h"
#include "Vulkan/VulkanSampler.h"
#include "Texture.h"
#include "TextureImage.h"
#include "Material.h"
#include "Model.h"
#include "Vertex.h"
#include "Sphere.h"

namespace Resources
{

    Scene::Scene(Vulkan::VulkanCommandPool& commandPool, std::vector<Model>&& models, std::vector<Texture>&& textures, bool usedForRayTracing)
        : m_Models(std::move(models)), m_Textures(std::move(textures))
    {
        // Concatenate all the models in our scene.
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Material> materials;
        std::vector<glm::vec4> procedurals;
        std::vector<VkAabbPositionsKHR> aabbs; // Specifying two opposing corners of an axis-aligned bounding box.
        std::vector<glm::uvec2> offsets;

        for (const auto& model : m_Models)
        {
            // Remember the index, vertex offset.
            const uint32_t indexOffset = static_cast<uint32_t>(indices.size());
            const uint32_t vertexOffset = static_cast<uint32_t>(vertices.size());
            const uint32_t materialOffset = static_cast<uint32_t>(materials.size());

            offsets.emplace_back(indexOffset, vertexOffset);

            // Copy model data one after the other by appending to the end of our vector.
            vertices.insert(vertices.end(), model.GetVertices().begin(), model.GetVertices().end());
            indices.insert(indices.end(), model.GetIndices().begin(), model.GetIndices().end());
            materials.insert(materials.end(), model.GetMaterials().begin(), model.GetMaterials().end());

            // Adjust the material ID.
            for (size_t i = vertexOffset; i != vertices.size(); ++i)
            {
                vertices[i].m_MaterialIndex += materialOffset;
            }

            // Add optional procedurals.
            const Sphere* const sphere = dynamic_cast<const Sphere*>(model.GetProcedural());
            if (sphere != nullptr)
            {
                const std::pair<glm::vec3, glm::vec3> aabb = sphere->GetBoundingBox();
                aabbs.push_back({ aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z });
                procedurals.emplace_back(sphere->m_Center, sphere->m_Radius);
            }
            else
            {
                aabbs.emplace_back();
                procedurals.emplace_back();
            }
        }

        const int flag = usedForRayTracing ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR : 0;

        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "Vertices", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | flag, vertices, m_VertexBuffer, m_VertexBufferMemory);
        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "Indices", VK_BUFFER_USAGE_INDEX_BUFFER_BIT | flag, indices, m_IndexBuffer, m_IndexBufferMemory);
        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "Materials", flag, materials, m_MaterialBuffer, m_MaterialBufferMemory);
        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "Offsets", flag, offsets, m_OffsetBuffer, m_OffsetBufferMemory);

        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "AA BBs", flag, aabbs, m_AABBBuffer, m_AABBBufferMemory);
        Vulkan::VulkanBufferUtilities::CreateDeviceBuffer(commandPool, "Procedurals", flag, procedurals, m_ProceduralBuffer, m_ProceduralBufferMemory);

        // Update all textures.
        m_TextureImages.reserve(m_Textures.size());
        m_TextureImageViews.resize(m_Textures.size());
        m_TextureSamplers.resize(m_Textures.size());

        for (size_t i = 0; i != m_Textures.size(); ++i)
        {
            m_TextureImages.emplace_back(new TextureImage(commandPool, m_Textures[i]));
            m_TextureImageViews[i] = m_TextureImages[i]->GetImageView().GetHandle();
            m_TextureSamplers[i] = m_TextureImages[i]->GetSampler().GetHandle();
        }
    }

    Scene::~Scene()
    {
        m_TextureSamplers.clear();
        m_TextureImageViews.clear();
        m_TextureImages.clear();

        m_ProceduralBuffer.reset();
        m_ProceduralBufferMemory.reset(); // Release memory after bound buffer has been destroyed.
        m_AABBBuffer.reset();
        m_AABBBufferMemory.reset();       // Release memory after bound buffer has been destroyed.
        m_OffsetBuffer.reset();
        m_OffsetBufferMemory.reset();     // Release memory after bound buffer has been destroyed.
        m_MaterialBuffer.reset();
        m_MaterialBufferMemory.reset();   // Release memory after bound buffer has been destroyed.
        m_IndexBuffer.reset();
        m_IndexBufferMemory.reset();      // Release memory after bound buffer has been destroyed.
        m_VertexBuffer.reset();
        m_VertexBufferMemory.reset();     // Release memory after bound buffer has been destroyed.
    }
}