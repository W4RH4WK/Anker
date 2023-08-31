add_library(glfw STATIC
	# common
	src/context.c
	src/init.c
	src/input.c
	src/monitor.c
	src/vulkan.c
	src/window.c

	# win32
	src/win32_init.c
	src/win32_joystick.c
	src/win32_monitor.c
	src/win32_time.c
	src/win32_thread.c
	src/win32_window.c
	src/wgl_context.c
	src/egl_context.c
	src/osmesa_context.c)

target_compile_definitions(glfw PRIVATE
	_GLFW_WIN32
	_GLFW_USE_HYBRID_HPG
	_GLFW_WNDCLASSNAME=L"Anker")
target_include_directories(glfw SYSTEM PUBLIC include)
set_target_properties(glfw PROPERTIES FOLDER external)
