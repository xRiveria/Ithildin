#include "Window.h"
#include "WindowUtilities.h"
#include "../Importers/ImageImporter.h"

namespace Raytracing
{
    namespace WindowCallbacks
    {
        void WindowErrorCallback(const int errorCode, const char* const description)
        {
            std::cerr << "GLFW Error: " << description << "(Code: " << errorCode << ")" << "\n";
        }

        void WindowKeyCallback(GLFWwindow* window, const int key, const int scanCode, const int action, const int mods)
        {
            Window* retrievedWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            if (retrievedWindow->OnKey) // Invokes our callback if it exists.
            {
                retrievedWindow->OnKey(key, scanCode, action, mods);
            }
        }

        void WindowCursorPositionCallback(GLFWwindow* window, const double xPosition, const double yPosition)
        {
            Window* retrievedWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            if (retrievedWindow->OnCursorMoved)
            {
                retrievedWindow->OnCursorMoved(xPosition, yPosition);
            }

        }

        void WindowMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
        {
            Window* retrievedWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            if (retrievedWindow->OnMouseButton)
            {
                retrievedWindow->OnMouseButton(button, action, mods);
            }
        }

        void WindowScrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
        {
            Window* retrievedWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            if (retrievedWindow->OnScroll)
            {
                retrievedWindow->OnScroll(xOffset, yOffset);
            }
        }
    }

    Window::Window(const WindowSettings& windowSettings) : m_Settings(windowSettings)
    {
        glfwSetErrorCallback(WindowCallbacks::WindowErrorCallback);

        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW.\n";
        }

        if (!glfwVulkanSupported())
        {
            std::cout << "GLFW does not support a Vulkan context.\n";
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, windowSettings.m_IsResizable ? GLFW_TRUE : GLFW_FALSE);

        GLFWmonitor* monitor = windowSettings.m_IsFullscreen ? glfwGetPrimaryMonitor() : nullptr;

        m_Window = glfwCreateWindow(windowSettings.m_Width, windowSettings.m_Height, windowSettings.m_Title.c_str(), monitor, nullptr);

        if (m_Window == nullptr)
        {
            std::cout << "Failed to create GLFW Window.\n";
        }

        GLFWimage windowIcon;
        windowIcon.pixels = stbi_load("../Assets/Textures/WindowIcon.png", &windowIcon.width, &windowIcon.height, nullptr, 4);
        if (windowIcon.pixels == nullptr)
        {
            std::cout << "Failed to load Window Icon.\n";
        }

        glfwSetWindowIcon(m_Window, 1, &windowIcon);
        stbi_image_free(windowIcon.pixels);

        if (windowSettings.m_IsCursorDisabled)
        {
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetKeyCallback(m_Window, WindowCallbacks::WindowKeyCallback);
        glfwSetCursorPosCallback(m_Window, WindowCallbacks::WindowCursorPositionCallback);
        glfwSetMouseButtonCallback(m_Window, WindowCallbacks::WindowMouseButtonCallback);
        glfwSetScrollCallback(m_Window, WindowCallbacks::WindowScrollCallback);
    }

    Window::~Window()
    {
        if (m_Window != nullptr)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }

        glfwTerminate();
        glfwSetErrorCallback(nullptr);
    }

    VkExtent2D Window::GetFramebufferSize() const
    {
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
        
        return VkExtent2D { static_cast<uint32_t>(framebufferWidth), static_cast<uint32_t>(framebufferHeight) };
    }

    VkExtent2D Window::GetWindowSize() const
    {
        int windowWidth, windowHeight;
        glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);

        return VkExtent2D { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) };
    }

    const char* Window::GetKeyName(int key, int scanCode) const
    {
        return glfwGetKeyName(key, scanCode);
    }

    std::vector<const char*> Window::GetRequiredInstanceExtensions() const
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    }

    double Window::GetTime() const
    {
        return glfwGetTime();
    }

    void Window::Close()
    {
        glfwSetWindowShouldClose(m_Window, 1);
    }

    bool Window::IsMinimized() const
    {
        return false;
    }

    void Window::Update()
    {
        glfwSetTime(0.0);

        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();

            if (DrawFrame)
            {
                DrawFrame();
            }
        }
    }

    void Window::WaitForEvents() const
    {
        glfwWaitEvents();
    }
}