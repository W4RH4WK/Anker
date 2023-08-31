file(GLOB_RECURSE stb_srcs *.c *.h)

add_library(stb STATIC ${stb_srcs})
target_include_directories(stb SYSTEM PUBLIC .)
set_target_properties(stb PROPERTIES FOLDER external)
