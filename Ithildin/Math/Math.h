#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan [0, 1] Range, instead of OpenGL's [-1, +1]
#define GLM_FORCE_RIGHT_HANDED      // Vulkan has a left handed coordinate system (same as DirectX), while OpenGL is right handed.
#define GLM_FORCE_RADIANS           // Enforce our angles to be in radians.

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
