-- Ithildin
workspace "Ithildin"
    architecture "x64"
    startproject "Ithildin"
    location "../"          -- Premake files are generated in the same directory as this script. Hence, redirect.

    BinariesDirectoryFormat = "%{cfg.buildcfg}"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    defines
    {
        "NOMINMAX"
    }

IncludeDirectories = {}
IncludeDirectories["GLM"] = "%{wks.location}Dependencies/GLM/Include"
IncludeDirectories["GLFW"] = "%{wks.location}Dependencies/GLFW/Include"
IncludeDirectories["Vulkan"] = "%{wks.location}Dependencies/Vulkan/Include"
IncludeDirectories["ImGui"] = "%{wks.location}Ithildin/Dependencies/ImGui"

LibraryDirectoriesDebug = {}
LibraryDirectoriesDebug["GLFW"] = "%{wks.location}Dependencies/GLFW/Library/Debug/glfw3.lib" 
LibraryDirectoriesDebug["Vulkan"] = "%{wks.location}Dependencies/Vulkan/Library/vulkan-1.lib"

LibraryDirectoriesRelease = {}
LibraryDirectoriesRelease["GLFW"] = "%{wks.location}Dependencies/GLFW/Library/Release/glfw3.lib" 
LibraryDirectoriesRelease["Vulkan"] = "%{wks.location}Dependencies/Vulkan/Library/vulkan-1.lib"

include "../Ithildin"