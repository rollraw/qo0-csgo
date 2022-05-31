workspace "base"
	configurations { "Debug", "Release" }
	system "windows"
	architecture "x86"
	
	project "cinnamon"
		-- specify physical files paths to import
		files
		{
			"base/**.cpp",
			"base/**.h",
			"dependencies/imgui/**.cpp",
			"dependencies/imgui/**.h",
			"dependencies/minhook/**.c",
			"dependencies/minhook/**.h",
			"resources/*.h"
		}
		
		-- specify virtual files paths to filter (for us them mostly same as physical)
		vpaths
		{
			{ ["source/*"] = "base/*.cpp" },
			{ ["source/core/*"] = "base/core/*.cpp" },
			{ ["source/features/*"] = "base/features/*.cpp" },
			{ ["source/sdk/*"] = "base/sdk/*.cpp" },
			{ ["source/sdk/datatypes/*"] = "base/sdk/datatypes/*.cpp" },
			{ ["source/sdk/hash/*"] = "base/sdk/hash/*.cpp" },
			{ ["source/sdk/interfaces/*"] = "base/sdk/interfaces/*.cpp" },
			{ ["source/utilities/*"] = "base/utilities/*.cpp" },
			{ ["source/utilities/memory/*"] = "base/utilities/memory/*.cpp" },
			
			{ ["include/*"] = "base/*.h" },
			{ ["include/core/*"] = "base/core/*.h" },
			{ ["include/features/*"] = "base/features/*.h" },
			{ ["include/sdk/*"] = "base/sdk/*.h" },
			{ ["include/sdk/datatypes/*"] = "base/sdk/datatypes/*.h" },
			{ ["include/sdk/hash/*"] = "base/sdk/hash/*.h" },
			{ ["include/sdk/interfaces/*"] = "base/sdk/interfaces/*.h" },
			{ ["include/utilities/*"] = "base/utilities/*.h" },
			{ ["include/utilities/memory/*"] = "base/utilities/memory/*.h" },
			
			{ ["dependencies/imgui/*"] = "dependencies/imgui/*" },
			{ ["dependencies/imgui/dx9/*"] = "dependencies/imgui/dx9/*" },
			{ ["dependencies/imgui/win32/*"] = "dependencies/imgui/win32/*" },
			{ ["dependencies/imgui/cpp/*"] = "dependencies/imgui/cpp/*" },
			{ ["dependencies/minhook/*"] = "dependencies/minhook/*" },
			{ ["dependencies/minhook/hde/*"] = "dependencies/minhook/hde/*" },
			
			{ ["resources/*"] = "resources/*.h" }
		}
		
		-- common
		language "C++"
		cppdialect "c++20"
		kind "SharedLib"
		location "base"
		targetname "base"
		targetdir "$(SolutionDir)build/$(Configuration)/"
		objdir "!$(SolutionDir)log/$(ProjectName)/$(Configuration)/" --use the "!" prefix to force a specific directory using msvs's provided environment variables instead of premake tokens
		
		-- additional
		characterset "MBCS"
		
		-- vc++ directories
		includedirs { "$(SolutionDir)dependencies/freetype/include", "$(SolutionDir)dependencies/json", "$(DXSDK_DIR)Include;$(IncludePath)" }
		libdirs { "$(SolutionDir)dependencies/freetype/win32", "$(DXSDK_DIR)Lib/x86" }

		-- build
		buildoptions { "/sdl" }
		rtti "on"
		staticruntime "off"
		editandcontinue "off"
		conformancemode "on"
		
		-- linker
		links { "d3d9.lib", "d3dx9.lib" }
		flags { "MultiProcessorCompile", "NoImportLib", "NoManifest", "NoPCH" }
		
		-- configuration specific
		filter "configurations:Debug"
			symbols "full"
			links { "freetype_debug.lib" }
			defines { "DEBUG_CONSOLE", "NOMINMAX", "_DEBUG" }

		filter "configurations:Release"
			optimize "speed"
			links { "freetype.lib" }
			defines { "NOMINMAX", "NDEBUG" }
			flags { "LinkTimeOptimization" }
