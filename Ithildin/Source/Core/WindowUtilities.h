#pragma once
#include "Core.h"
#include <string>
#include <iostream>

namespace Vulkan
{
    struct WindowSettings final
    {
        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
        bool m_IsCursorDisabled;
        bool m_IsFullscreen;
        bool m_IsResizable;
    }; 
}