#pragma once
#include "Core.h"
#include "WindowUtilities.h"
#include <functional>
#include <vector>

namespace Raytracing
{
    class Window final
    {
    public:
        explicit Window(const WindowSettings& windowSettings);
        ~Window();

        // Properties
        const WindowSettings& GetSettings() const { return m_Settings; }
        GLFWwindow* GetWindowHandle() const { return m_Window; }
        VkExtent2D GetFramebufferSize() const;
        VkExtent2D GetWindowSize() const;

        // Instance
        const char* GetKeyName(int key, int scanCode) const;
        std::vector<const char*> GetRequiredInstanceExtensions() const;
        double GetTime() const;
        
        void Close();
        bool IsMinimized() const;
        void Update();
        void WaitForEvents() const;

        // Callbacks
        std::function<void()> DrawFrame;
        std::function<void(int key, int scanCode, int action, int mods)> OnKey;
        std::function<void(double xPosition, double yPosition)> OnCursorMoved;
        std::function<void(int button, int action, int mods)> OnMouseButton;
        std::function<void(double xOffset, double yOffset)> OnScroll;

    private:
        const WindowSettings m_Settings;
        GLFWwindow* m_Window = nullptr;
    };
}