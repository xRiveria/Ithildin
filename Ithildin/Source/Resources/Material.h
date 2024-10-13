#pragma once
#include "Math/Math.h"

namespace Resources
{
    struct alignas(16) Material final // 16 byte alignment.
    {
        static Material Lambertian(const glm::vec3& diffuse, const int32_t textureID = -1)
        {
            return Material { glm::vec4(diffuse, 1.0f), textureID, 0.0f, 0.0f, Material_Type::Material_Type_Lambertian };
        }

        static Material Metallic(const glm::vec3& diffuse, const float fuzziness, const int32_t textureID = -1)
        {
            return Material { glm::vec4(diffuse, 1.0f), textureID, fuzziness, 0.0f, Material_Type::Material_Type_Metallic };
        }

        static Material Dielectric(const float refractionIndex, const int32_t textureID = -1)
        {
            return Material { glm::vec4(0.7f, 0.7f, 1.0f, 1.0f), textureID, 0.0f, refractionIndex, Material_Type::Material_Type_Dieletric };
        }

        static Material Isotropic(const glm::vec3& diffuse, const int32_t textureID = -1)
        {
            return Material { glm::vec4(diffuse, 1.0f), textureID, 0.0f, 0.0f, Material_Type::Material_Type_Isotropic };
        }

        static Material DiffuseLight(const glm::vec3& diffuse, const int32_t textureID = -1)
        {
            return Material { glm::vec4(diffuse, 1.0f), textureID, 0.0f, 0.0f, Material_Type::Material_Type_DiffuseLight };
        }

        enum class Material_Type : uint32_t
        {
            Material_Type_Lambertian   = 0,
            Material_Type_Metallic     = 1,
            Material_Type_Dieletric    = 2,
            Material_Type_Isotropic    = 3,
            Material_Type_DiffuseLight = 4
        };

        // Base Material
        glm::vec4 m_Diffuse;
        int32_t m_DiffuseTextureID;

        // Metal Fuzziness
        float m_Fuzziness;

        // Dieletric Refraction Index
        float m_RefractionIndex;

        // The kind of material you're dealing with.
        Material_Type m_MaterialType;
    };
}