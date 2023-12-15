cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0091 NEW) # MSVC_RUNTIME_LIBRARY

project(anker LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT WIN32)
	message(FATAL_ERROR "Anker only works on Windows as of right now!")
endif()

# CMake usually adds a pragma system_header to the precompiled header. We don't
# want that as we suppresses warnings from system headers.
set(CMAKE_PCH_PROLOGUE "")

function(anker_compile_options target)
	set_target_properties(${target} PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF)

	target_compile_options(${target} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP /W4 /external:W0 /bigobj /utf-8>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Oi>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wtype-limits -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -g>

		# Enable MSVC debug symbols and edit-and-continue support
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/ZI>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Zi>

		# Colorize compiler output
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>
		$<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>)

	target_compile_definitions(${target} PUBLIC
		$<$<BOOL:${WIN32}>:ANKER_PLATFORM_WINDOWS>
		$<$<BOOL:${UNIX}>:ANKER_PLATFORM_LINUX>)

	get_target_property(target_type ${target} TYPE)
	if(target_type STREQUAL EXECUTABLE)
		target_link_options(${target} PRIVATE
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/OPT:REF /OPT:ICF /DEBUG>)
		set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endif()

	get_target_property(target_srcs ${target} SOURCES)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${target_srcs})
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

add_library(anker STATIC ${anker_srcs})
anker_compile_options(anker)
target_include_directories(anker PUBLIC code)
target_precompile_headers(anker PUBLIC code/anker/anker_pch.hpp)
target_link_libraries(anker PUBLIC
	mimalloc fmt cppbase64 cppitertools reflcpp stb ddspp glm rapidjson entt
	imgui sdl2 box2d tracy)

add_executable(anker_main WIN32 code/anker_main.cpp)
anker_compile_options(anker_main)
target_link_libraries(anker_main PRIVATE anker sdl2main)
add_custom_command(TARGET anker_main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:anker_main> $<TARGET_FILE_DIR:anker_main>
	COMMAND_EXPAND_LISTS)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT anker_main)
