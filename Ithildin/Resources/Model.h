#pragma once
#include "Material.h"
#include "Vertex.h"
#include <string>
#include <vector>
#include <memory>

namespace Resources
{
    class Model final
    {
    public:
        static Model LoadModel(const std::string& fileName);
        static Model CreateCornellBox(const float scale);
        static Model CreateBox(const glm::vec3& point1, const glm::vec3& point2, const Material& material);
        static Model CreateSphere(const glm::vec3& center, float radius, const Material& material, bool isProcedural);

        Model() = default;

        void SetMaterial(const Material& material);
        void Transform(const glm::mat4& transform);

        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
        // const std::vector<Material>& GetMaterials() const { return m_Materials; }

        // const Procedural* GetProcedural() const { return m_Procedurals.get(); }

        uint32_t GetNumberOfVertices() const { return static_cast<uint32_t>(m_Vertices.size()); }
        uint32_t GetNumberOfIndices() const { return static_cast<uint32_t>(m_Indices.size()); }
        // uint32_t GetNumberOfMaterials() const { return static_cast<uint32_t>(m_Materials.size()); }

    private:
        Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const class Procedural* procedural);

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        // std::vector<Material> m_Materials;
        // std::shared_ptr<const class Procedural> m_Procedurals;
    };
}