cmake_minimum_required(VERSION 3.16)
project("base")
add_library("csgo" MODULE)

# common
set_target_properties("csgo" PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	
	OUTPUT_NAME "base"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/$<CONFIG>"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/$<CONFIG>"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/$<CONFIG>"
)

# include
target_include_directories("csgo" PUBLIC "${CMAKE_SOURCE_DIR}/base")
target_include_directories("csgo" PUBLIC "${CMAKE_SOURCE_DIR}/resources")
target_include_directories("csgo" PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/imgui")
target_include_directories("csgo" PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/minhook")
target_include_directories("csgo" PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/freetype/include")

# source
target_sources("csgo" PRIVATE
	# dependencies
		# imgui
		"dependencies/imgui/imgui.cpp"
		"dependencies/imgui/imgui_draw.cpp"
		"dependencies/imgui/imgui_freetype.cpp"
		"dependencies/imgui/imgui_widgets.cpp"
        "dependencies/imgui/dx9/imgui_impl_dx9.cpp"
        "dependencies/imgui/dx9/imgui_impl_dx9.h"
        "dependencies/imgui/win32/imgui_impl_win32.cpp"
        "dependencies/imgui/win32/imgui_impl_win32.h"
		# minhook
		"dependencies/minhook/buffer.c"
		"dependencies/minhook/hook.c"
		"dependencies/minhook/trampoline.c"
		"dependencies/minhook/hde/hde32.c"
		"dependencies/minhook/hde/hde64.c"
	# base
	"base/core.cpp"
	"base/features.cpp"
	"base/sdk.cpp"
		# core
		"base/core/config.cpp"
		"base/core/convar.cpp"
		"base/core/entitylistener.cpp"
		"base/core/hooks.cpp"
		"base/core/interfaces.cpp"
		"base/core/menu.cpp"
		"base/core/netvar.cpp"
		"base/core/proxies.cpp"
		# features
			# legit
			"base/features/legit/triggerbot.cpp"
			# misc
			"base/features/misc/movement.cpp"
			# rage
			"base/features/rage/antiaim.cpp"
			# visual
			"base/features/visual/chams.cpp"
			"base/features/visual/glow.cpp"
			"base/features/visual/overlay.cpp"
			"base/features/visual/screen.cpp"
			"base/features/visual/world.cpp"
		"base/features/animationcorrection.cpp"
		"base/features/autowall.cpp"
		"base/features/lagcompensation.cpp"
		"base/features/legit.cpp"
		"base/features/misc.cpp"
		"base/features/prediction.cpp"
		"base/features/rage.cpp"
		"base/features/visual.cpp"
		# sdk
			# datatypes
			"base/sdk/datatypes/matrix.cpp"
			"base/sdk/datatypes/qangle.cpp"
			"base/sdk/datatypes/vector.cpp"
			# hash
			"base/sdk/hash/crc32.cpp"
			"base/sdk/hash/fnv1a.cpp"
			"base/sdk/hash/md5.cpp"
		"base/sdk/animation.cpp"
		"base/sdk/entity.cpp"
		"base/sdk/studio.cpp"
		# utilities
		"base/utilities/draw.cpp"
		"base/utilities/inputsystem.cpp"
		"base/utilities/log.cpp"
		"base/utilities/math.cpp"
		"base/utilities/memory.cpp"
)

# link
target_link_directories("csgo" PRIVATE "${CMAKE_SOURCE_DIR}/dependencies/freetype/win32")
target_link_libraries("csgo" "d3d9.lib")
target_link_options("csgo" PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/MANIFEST:NO>
)

# compile
set_target_properties("csgo" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_options("csgo" PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/MP /GR$<$<CONFIG:Release>:-> /permissive->
	$<$<OR:$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CXX_COMPILER_ID:Clang>>,$<CXX_COMPILER_ID:GNU>>:$<$<CONFIG:Release>:-fno-rtti>>
)

# configuration specific
target_compile_definitions("csgo" PRIVATE
	$<$<CONFIG:Debug>:_DEBUG>
	$<$<CONFIG:Release>:NDEBUG>
)
