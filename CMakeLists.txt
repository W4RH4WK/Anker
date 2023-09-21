cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0091 NEW) # MSVC_RUNTIME_LIBRARY

project(anker LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CMake usually adds a pragma system_header to the precompiled header. We don't
# want that as we suppresses warnings from system headers.
set(CMAKE_PCH_PROLOGUE "")

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

	target_compile_definitions(${target} PUBLIC
		$<$<BOOL:${WIN32}>:ANKER_PLATFORM_WINDOWS>
		$<$<BOOL:${UNIX}>:ANKER_PLATFORM_LINUX>)
endfunction()

# Include all external dependencies
file(GLOB externals CONFIGURE_DEPENDS LIST_DIRECTORIES TRUE external/*)
foreach(external ${externals})
	add_subdirectory(${external})
endforeach()

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
	cppbase64
	cppitertools
	reflcpp
	stb
	ddspp
	glm
	rapidjson
	entt
	imgui
	glfw
	box2d
	tracy)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT anker)
