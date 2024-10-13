#include "Model.h"
#include "CornellBox.h"
#include "Sphere.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "Importers/Internal/tiny_obj_loader.h"

#include <iostream>
#include <chrono>
#include <filesystem>

namespace std
{
    template<> struct hash<Resources::Vertex> final
    {
        size_t operator()(Resources::Vertex const& vertex) const noexcept
        {
            return
                Combine(hash<glm::vec3>()(vertex.m_Position),
                    Combine(hash<glm::vec3>()(vertex.m_Normal),
                        Combine(hash<glm::vec2>()(vertex.m_TexCoords),
                            hash<int>()(vertex.m_MaterialIndex))));
        }

    private:
        static size_t Combine(size_t hash0, size_t hash1)
        {
            return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
        }
    };
}

namespace Resources
{
    Model Model::LoadModel(const std::string& filePath)
    {
        std::cout << "Loading: " << filePath << "... \n";

        const std::chrono::steady_clock::time_point timer = std::chrono::high_resolution_clock::now();
        const std::string materialPath = std::filesystem::path(filePath).parent_path().string(); // Returns path to parent directory.

        tinyobj::ObjReader modelImporter;

        if (!modelImporter.ParseFromFile(filePath))
        {
            std::runtime_error("Failed to load model: " + filePath + " - " + modelImporter.Error() + "\n");
        }

        if (!modelImporter.Warning().empty())
        {
            std::cout << "Warning: " << modelImporter.Warning() << "\n";
        }

        // Materials
        std::vector<Material> materials;

        for (const tinyobj::material_t& material : modelImporter.GetMaterials())
        {
            Material customMaterial = {};
            customMaterial.m_Diffuse = glm::vec4(material.diffuse[0], material.diffuse[1], material.diffuse[2], 1.0f);
            customMaterial.m_DiffuseTextureID = -1;

            materials.emplace_back(customMaterial);
        }

        if (materials.empty())
        {
            Material customMaterial = {};
            customMaterial.m_Diffuse = glm::vec4(glm::vec4(0.7f, 0.7f, 0.7f, 1.0));
            customMaterial.m_DiffuseTextureID = -1;

            materials.emplace_back(customMaterial);
        }

        // Geometry
        const tinyobj::attrib_t& modelAttributes = modelImporter.GetAttrib();

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<Vertex, uint32_t> uniqueVertices(modelAttributes.vertices.size());
        size_t faceID = 0;

        for (const tinyobj::shape_t& shape : modelImporter.GetShapes())
        {
            const tinyobj::mesh_t& mesh = shape.mesh;

            for (const tinyobj::index_t& index : mesh.indices)
            {
                Vertex vertex = {};
                vertex.m_Position =
                {
                    modelAttributes.vertices[3 * index.vertex_index + 0], // Index 0, 1, 2 and so on.
                    modelAttributes.vertices[3 * index.vertex_index + 1],
                    modelAttributes.vertices[3 * index.vertex_index + 2],
                };

                if (!modelAttributes.normals.empty())
                {
                    vertex.m_Normal =
                    {
                        modelAttributes.normals[3 * index.normal_index + 0],
                        modelAttributes.normals[3 * index.normal_index + 1],
                        modelAttributes.normals[3 * index.normal_index + 2],
                    };
                }

                if (!modelAttributes.texcoords.empty())
                {
                    vertex.m_TexCoords =
                    {
                        modelAttributes.texcoords[2 * index.texcoord_index + 0],
                        1 - modelAttributes.texcoords[2 * index.texcoord_index + 1] // In Vulkan, the Y coordinate of an image begins in the top to bottom orientation, where 0 means the top of the image. Hence, we will flip the Y coordinate as TinyOBJ assumes a coordinate system where 0 means the bottom of the image.
                    };
                }

                vertex.m_MaterialIndex = std::max(0, mesh.material_ids[faceID++ / 3]);

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        // If the model did not specify models, then create smooth normals that conserve the same number of vertices.
        // Usiong flat normals would mean creating more vertices than we currently have, so for simplicity and better visuals, we don't do it.
        // See: https://stackoverflow.com/questions/12139840/obj-file-averaging-normals.
        if (modelAttributes.normals.empty())
        {
            std::vector<glm::vec3> normals(vertices.size());

            for (size_t i = 0; i < indices.size(); i += 3)
            {
                const auto normal = glm::normalize(glm::cross(
                    glm::vec3(vertices[indices[i + 1]].m_Position) - glm::vec3(vertices[indices[i]].m_Position),
                    glm::vec3(vertices[indices[i + 2]].m_Position) - glm::vec3(vertices[indices[i]].m_Position)));

                vertices[indices[i + 0]].m_Normal += normal;
                vertices[indices[i + 1]].m_Normal += normal;
                vertices[indices[i + 2]].m_Normal += normal;
            }

            for (Vertex& vertex : vertices)
            {
                vertex.m_Normal = glm::normalize(vertex.m_Normal);
            }
        }

        const float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();

        std::cout << "Successfully Loaded Model (" << modelAttributes.vertices.size() << " Vertices, " << uniqueVertices.size() << " Unique Vertices, " << materials.size() << " Materials)\n";
        std::cout << "Elapsed: " << elapsedTime << " Seconds.\n";

        return Model(std::move(vertices), std::move(indices), std::move(materials), nullptr);
    }

    Model Model::CreateCornellBox(const float scale)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Material> materials;

        CornellBox::Create(scale, vertices, indices, materials);

        return Model(std::move(vertices), std::move(indices), std::move(materials), nullptr);
    }

    Model Model::CreateBox(const glm::vec3& p0, const glm::vec3& p1, const Material& material)
    {
        std::vector<Vertex> vertices =
        {
            Vertex{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(-1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p0.y, p1.z), glm::vec3(-1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p1.z), glm::vec3(-1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p0.z), glm::vec3(-1, 0, 0), glm::vec2(0), 0},

            Vertex{glm::vec3(p1.x, p0.y, p1.z), glm::vec3(1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p0.y, p0.z), glm::vec3(1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p1.y, p0.z), glm::vec3(1, 0, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p1.y, p1.z), glm::vec3(1, 0, 0), glm::vec2(0), 0},

            Vertex{glm::vec3(p1.x, p0.y, p0.z), glm::vec3(0, 0, -1), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(0, 0, -1), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p0.z), glm::vec3(0, 0, -1), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p1.y, p0.z), glm::vec3(0, 0, -1), glm::vec2(0), 0},

            Vertex{glm::vec3(p0.x, p0.y, p1.z), glm::vec3(0, 0, 1), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p0.y, p1.z), glm::vec3(0, 0, 1), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p1.y, p1.z), glm::vec3(0, 0, 1), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p1.z), glm::vec3(0, 0, 1), glm::vec2(0), 0},

            Vertex{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(0, -1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p0.y, p0.z), glm::vec3(0, -1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p0.y, p1.z), glm::vec3(0, -1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p0.y, p1.z), glm::vec3(0, -1, 0), glm::vec2(0), 0},

            Vertex{glm::vec3(p1.x, p1.y, p0.z), glm::vec3(0, 1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p0.z), glm::vec3(0, 1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p0.x, p1.y, p1.z), glm::vec3(0, 1, 0), glm::vec2(0), 0},
            Vertex{glm::vec3(p1.x, p1.y, p1.z), glm::vec3(0, 1, 0), glm::vec2(0), 0},
        };

        std::vector<uint32_t> indices =
        {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        return Model(std::move(vertices), std::move(indices), std::vector<Material>{material}, nullptr);
    }

    Model Model::CreateSphere(const glm::vec3& center, float radius, const Material& material, bool isProcedural)
    {
        const int slices = 32;
        const int stacks = 16;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        const float pi = 3.14159265358979f;

        for (int j = 0; j <= stacks; ++j)
        {
            const float j0 = pi * j / stacks;

            // Vertex
            const float v = radius * -std::sin(j0);
            const float z = radius * std::cos(j0);

            // Normals		
            const float n0 = -std::sin(j0);
            const float n1 = std::cos(j0);

            for (int i = 0; i <= slices; ++i)
            {
                const float i0 = 2 * pi * i / slices;

                const glm::vec3 position(
                    center.x + v * std::sin(i0),
                    center.y + z,
                    center.z + v * std::cos(i0));

                const glm::vec3 normal(
                    n0 * std::sin(i0),
                    n1,
                    n0 * std::cos(i0));

                const glm::vec2 texCoord(
                    static_cast<float>(i) / slices,
                    static_cast<float>(j) / stacks);

                vertices.push_back(Vertex{ position, normal, texCoord, 0 });
            }
        }

        for (int j = 0; j < stacks; ++j)
        {
            for (int i = 0; i < slices; ++i)
            {
                const auto j0 = (j + 0) * (slices + 1);
                const auto j1 = (j + 1) * (slices + 1);
                const auto i0 = i + 0;
                const auto i1 = i + 1;

                indices.push_back(j0 + i0);
                indices.push_back(j1 + i0);
                indices.push_back(j1 + i1);

                indices.push_back(j0 + i0);
                indices.push_back(j1 + i1);
                indices.push_back(j0 + i1);
            }
        }

        return Model(std::move(vertices), std::move(indices), std::vector<Material>{material}, isProcedural ? new Sphere(center, radius) : nullptr);
    }

    void Model::SetMaterial(const Material& material)
    {
        if (m_Materials.size() != 1)
        {
            std::runtime_error("Cannot change material on a multi-material mode.\n");
        }

        m_Materials[0] = material;
    }

    void Model::SetTransform(const glm::mat4& transform)
    {
        const auto transformInverse = glm::inverseTranspose(transform);

        for (Vertex& vertex : m_Vertices)
        {
            vertex.m_Position = transform * glm::vec4(vertex.m_Position, 1);
            vertex.m_Normal = transformInverse * glm::vec4(vertex.m_Normal, 0);
        }
    }

    Model::Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Material>&& materials, const Procedural* procedural)
               : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Materials(std::move(materials)), m_Procedural(procedural)
    {

    }
}