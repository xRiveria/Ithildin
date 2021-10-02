#pragma once
#include "Material.h"
#include "Vertex.h"
#include <vector>

namespace Resources
{
    class CornellBox final
    {
    public:
        static void Create(float scale, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Material>& materials);
    };
}