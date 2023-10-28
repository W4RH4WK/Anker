add_library(imgui STATIC
	code/backends/imgui_impl_dx11.cpp
	code/backends/imgui_impl_dx11.h
	code/backends/imgui_impl_sdl2.cpp
	code/backends/imgui_impl_sdl2.h
	code/imconfig.h
	code/imgui_demo.cpp
	code/imgui_draw.cpp
	code/imgui_internal.h
	code/imgui_tables.cpp
	code/imgui_widgets.cpp
	code/imgui.cpp
	code/imgui.h
	code/imstb_rectpack.h
	code/imstb_textedit.h
	code/imstb_truetype.h
	code/misc/cpp/imgui_stdlib.cpp
	code/misc/cpp/imgui_stdlib.h)
target_include_directories(imgui SYSTEM PUBLIC code code/backends code/misc/cpp)
target_link_libraries(imgui PRIVATE sdl2)
set_target_properties(imgui PROPERTIES FOLDER external)
