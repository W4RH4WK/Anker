#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <anker/core/anker_data_loader.hpp>

// The Platform abstracts away various operating system specific parts, like
// window management and mouse cursor handling. It is initialized before the
// engine and can be accessed globally. Certain events (e.g. window resize) are
// forwarded to the global engine instance, given it has been initialized.
namespace Anker::Platform {

using NativeWindow = HWND;

void initialize();
void finalize();

void tick();

bool shouldShutdown();

////////////////////////////////////////////////////////////
// Window

void createMainWindow();
void destroyMainWindow();

Vec2i windowSize();

GLFWwindow* glfwWindow();
NativeWindow nativeWindow();

////////////////////////////////////////////////////////////
// Input / Cursor

Vec2 cursorPosition();

// Hide cursor state is reset on Platform::tick. Any system that wants to hide
// the cursor has to call this function on every tick. If no system calls this
// function, the cursor is shown.
void hideCursor();

////////////////////////////////////////////////////////////
// ImGui

void imguiImplInit();
void imguiImplNewFrame();
void imguiImplShutdown();

} // namespace Anker::Platform
