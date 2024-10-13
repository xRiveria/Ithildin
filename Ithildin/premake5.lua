project "Ithildin"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    warnings "Extra"

    location	"" -- Override solution settings.
	targetdir	("../Binaries/Output/" .. BinariesDirectoryFormat .. "/%{prj.name}")
	objdir		("../Binaries/Intermediates/" .. BinariesDirectoryFormat .. "/%{prj.name}")

    files
	{
		"Source/**.h",
		"Source/**.c",
		"Source/**.hpp",
		"Source/**.cpp"
	}

    includedirs
    {
        "Source",
        "%{IncludeDirectories.GLM}",
        "%{IncludeDirectories.GLFW}",
        "%{IncludeDirectories.Vulkan}",
        "%{IncludeDirectories.ImGui}",
    }

    defines 
    {
        "NOMINMAX",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE",
        "GLM_FORCE_RIGHT_HANDED",
        "GLM_FORCE_RADIANS"
    }

    filter "configurations:Debug"
        runtime "Debug"
        optimize "Off"
        symbols "On"
        links { "%{LibraryDirectoriesDebug.Vulkan}", "%{LibraryDirectoriesDebug.GLFW}" }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        symbols "On"
        links { "%{LibraryDirectoriesRelease.Vulkan}", "%{LibraryDirectoriesRelease.GLFW}" }

group "Dependencies"
	include "Dependencies/ImGui"
group ""