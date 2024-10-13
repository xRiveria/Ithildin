#pragma once
#include "Math/Math.h"
#include "Core/Core.h"
#include <array>

namespace Resources
{
    struct Vertex final
    {
        glm::vec3 m_Position;
        glm::vec3 m_Normal;
        glm::vec2 m_TexCoords;
        int32_t m_MaterialIndex;

        bool operator==(const Vertex& otherVertex) const
        {
            return m_Position      == otherVertex.m_Position  &&
                   m_Normal        == otherVertex.m_Normal    &&
                   m_TexCoords     == otherVertex.m_TexCoords &&
                   m_MaterialIndex == otherVertex.m_MaterialIndex;
        }

        static VkVertexInputBindingDescription GetBindingDescription() // Specify our vertex's input binding description.
        {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Specify the rate at which vertex attributes are pulled from the buffer (vertex/instance).

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, m_Position);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, m_Normal);

            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, m_TexCoords);

            attributeDescriptions[3].binding  = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format   = VK_FORMAT_R32_SINT;
            attributeDescriptions[3].offset   = offsetof(Vertex, m_MaterialIndex);

            return attributeDescriptions;
        }
    };
}