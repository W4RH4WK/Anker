cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0091 NEW) # MSVC_RUNTIME_LIBRARY

project(anker LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CMake usually adds a pragma system_header to the precompiled header. We don't
# want that as we suppresses warnings from system headers.
set(CMAKE_PCH_PROLOGUE "")

add_subdirectory(external/mimalloc)
add_subdirectory(external/fmt)
add_subdirectory(external/cppitertools)
add_subdirectory(external/reflcpp)
add_subdirectory(external/stb)
add_subdirectory(external/ddspp)
add_subdirectory(external/glm)
add_subdirectory(external/rapidjson)
add_subdirectory(external/entt)
# add_subdirectory(external/lua)
# add_subdirectory(external/sol2)
# add_subdirectory(external/lrdb)
add_subdirectory(external/imgui)
add_subdirectory(external/glfw)
add_subdirectory(external/tracy)

function(anker_compile_options target)
	set_target_properties(${target} PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF)

	target_compile_options(${target} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP /W4 /external:W0 /bigobj /utf-8>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wtype-limits -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -g>

		# Enable MSVC debug symbols and edit and continue support
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/ZI>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Zi>

		# Colorize compiler output
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>
		$<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>)
endfunction()

file(GLOB_RECURSE anker_srcs CONFIGURE_DEPENDS
	code/anker/*.cpp
	code/anker/*.hpp
	code/anker/*.inc)
source_group(TREE . FILES ${anker_srcs})

add_executable(anker WIN32 ${anker_srcs})
anker_compile_options(anker)
set_target_properties(anker PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
target_include_directories(anker PRIVATE code)
target_precompile_headers(anker PRIVATE code/anker/anker_pch.hpp)
target_link_options(anker PRIVATE /ENTRY:mainCRTStartup)
target_link_libraries(anker PRIVATE
	mimalloc
	fmt
	cppitertools
	reflcpp
	stb
	ddspp
	glm
	rapidjson
	entt
	# lualib
	# sol2
	# lrdb
	imgui
	glfw
	tracy)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT anker)
