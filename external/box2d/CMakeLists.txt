file(GLOB_RECURSE box2d_srcs *.cpp *.h)

add_library(box2d STATIC ${box2d_srcs})
target_compile_definitions(box2d PUBLIC B2_USER_SETTINGS)
target_include_directories(box2d SYSTEM
	PUBLIC include
	PRIVATE src)
set_target_properties(box2d PROPERTIES FOLDER external)
