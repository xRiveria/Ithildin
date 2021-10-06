#pragma once
#include <memory>

struct UserSettings final
{
    // Application
    bool m_IsBenchmarkingEnabled;

    // Benchmark
    bool m_BenchmarkNextScenes = {};
    uint32_t m_BenchmarkMaxTime = {};

    // Renderer
    bool m_IsRaytracingEnabled;
    bool m_IsRayAccumulationEnabled;
    uint32_t m_NumberOfSamples;
    uint32_t m_NumberOfBounces;
    uint32_t m_MaxNumberOfSamples;

    // Scene
    int m_SceneIndex;

    // Camera
    float m_FieldOfView;
    float m_Aperture;
    float m_FocusDistance;

    // Profiler
    bool m_ShowHeatmap;
    float m_HeatmapScale;

    // UI
    bool m_ShowSettings;
    bool m_ShowOverlay;

    inline const static float m_FieldOfViewValueMinimum = 10.0f;
    inline const static float m_FieldOfViewValueMaximum = 90.0f;

    bool RequireAccumulationReset(const UserSettings& previousSettings) const
    {
        return m_IsRaytracingEnabled      != previousSettings.m_IsRaytracingEnabled      ||
               m_IsRayAccumulationEnabled != previousSettings.m_IsRayAccumulationEnabled ||
               m_NumberOfBounces          != previousSettings.m_NumberOfBounces          ||
               m_FieldOfView              != previousSettings.m_FieldOfView              ||
               m_Aperture                 != previousSettings.m_Aperture                 ||
               m_FocusDistance            != previousSettings.m_FocusDistance;
    }
};