#pragma once
#include "Math/Math.h"
#include <functional>
#include <string>
#include <tuple>
#include <vector>

namespace Resources
{
    class Model;
    class Texture;
}

typedef std::tuple<std::vector<Resources::Model>, std::vector<Resources::Texture>> SceneAssets;

class SceneList final
{
public:
    struct CameraInitialState
    {
        glm::mat4 m_ModelView;
        float m_FieldOfView;
        float m_Aperture;
        float m_FocusDistance;
        float m_ControlSpeed;
        bool m_GammaCorrection;
        bool m_HasSky;
    };

    static SceneAssets CubeAndSpheres(CameraInitialState& cameraState);
    static SceneAssets RayTracingInOneWeekend(CameraInitialState& cameraState);
    static SceneAssets CornellBox(CameraInitialState& cameraState);
    static SceneAssets PlanetsInOneWeekend(CameraInitialState& cameraState);
    static SceneAssets LucyInOneWeekend(CameraInitialState& cameraState);
    static SceneAssets CornellBoxLucy(CameraInitialState& cameraState);

    static const std::vector<std::pair<std::string, std::function<SceneAssets(CameraInitialState& initialState)>>> s_AllScenes;
};