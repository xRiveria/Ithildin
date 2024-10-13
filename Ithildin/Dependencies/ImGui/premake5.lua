project "ImGui"
	kind "StaticLib"
	location ""
	language "C++"
    staticruntime "Off"
	location	"" -- Override solution settings.
	targetdir	("Binaries/Output/" .. BinariesDirectoryFormat .. "/%{prj.name}")
	objdir		("Binaries/Intermediates/" .. BinariesDirectoryFormat .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime	"Release"
		optimize "On"
        symbols	"Off"