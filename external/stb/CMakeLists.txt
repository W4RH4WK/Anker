add_library(stb STATIC
	src/stb_image.c
	src/stb_image_write.c)
target_include_directories(stb SYSTEM PUBLIC include)
set_target_properties(stb PROPERTIES FOLDER external)
