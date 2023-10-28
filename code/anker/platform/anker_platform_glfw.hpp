#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Anker::Platform {

using NativeWindow = HWND;

GLFWwindow* glfwWindow();

} // namespace Anker::Platform
