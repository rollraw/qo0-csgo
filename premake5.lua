workspace "base"
	configurations { "Debug", "Release" }
	system "windows"
	architecture "x86"

	project "csgo"
		location "base"
		targetname "base"
		kind "SharedLib"

		language "C++"
		cppdialect "c++20"

		-- specify physical include file paths
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

		-- specify virtual filter file paths
		vpaths
		{
			{ ["*"] = "base/*" },
			
			{ ["core/*"] = "base/core/*" },
			
			{ ["features/*"] = "base/features/*" },
			{ ["features/legit/*"] = "base/features/legit/*" },
			{ ["features/misc/*"] = "base/features/misc/*" },
			{ ["features/rage/*"] = "base/features/rage/*" },
			{ ["features/visual/*"] = "base/features/visual/*" },
			
			{ ["sdk/*"] = "base/sdk/*" },
			{ ["sdk/datatypes/*"] = "base/sdk/datatypes/*" },
			{ ["sdk/hash/*"] = "base/sdk/hash/*" },
			{ ["sdk/interfaces/*"] = "base/sdk/interfaces/*" },
			
			{ ["utilities/*"] = "base/utilities/*" },

			{ ["dependencies/imgui/*"] = "dependencies/imgui/*" },
			{ ["dependencies/imgui/dx9/*"] = "dependencies/imgui/dx9/*" },
			{ ["dependencies/imgui/win32/*"] = "dependencies/imgui/win32/*" },
			{ ["dependencies/minhook/*"] = "dependencies/minhook/*" },
			{ ["dependencies/minhook/hde/*"] = "dependencies/minhook/hde/*" },

			{ ["resources/*"] = "resources/*.h" }
		}

		-- @note: use the "!" prefix to force a specific directory using msvs's provided environment variables instead of premake tokens
		targetdir "$(SolutionDir)build/$(Configuration)/"
		objdir "!$(SolutionDir)intermediate/$(ProjectName)/$(Configuration)/"
		implibname "$(OutDir)$(TargetName)"

		-- link
		includedirs { "$(SolutionDir)dependencies", "$(SolutionDir)dependencies/freetype/include" }
		libdirs { "$(SolutionDir)dependencies/freetype/win32" }
		links { "d3d9.lib" }
		flags { "MultiProcessorCompile", "NoImportLib", "NoManifest", "NoPCH" } -- @test: NoImplicitLink

		-- compile
		conformancemode "on"
		editandcontinue "off"
		entrypoint "CoreEntryPoint"
		exceptionhandling "off"
		staticruntime "on"
		symbols "full"
		vectorextensions "SSE2"

		-- configuration specific
		filter "configurations:Debug"
			defines { "_DEBUG" }
			flags { "NoIncrementalLink" }
			justmycode "off"
			rtti "on"

		filter "configurations:Release"
			defines { "NDEBUG" }
			flags { "LinkTimeOptimization" } -- @test: NoRuntimeChecks
			optimize "speed"
			rtti "off"
			-- @test: "/Zc:threadSafeInit-" to disable thread-safe local static initialization ('__Init_thread_header'/'__Init_thread_footer' calls)
