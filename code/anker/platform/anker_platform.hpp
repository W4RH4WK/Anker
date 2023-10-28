#pragma once

#include <anker/core/anker_inputs.hpp>

#if ANKER_PLATFORM_WINDOWS
#include <anker/platform/anker_platform_glfw.hpp>
#endif

// The Platform abstracts away various operating system specific parts, like
// window management and mouse cursor handling. It is initialized before the
// engine and can be accessed globally. Certain events (e.g. window resize) are
// forwarded to the global engine instance, given it has been initialized.
namespace Anker::Platform {

void initialize();
void finalize();

void tick();

bool shouldShutdown();

////////////////////////////////////////////////////////////
// Window

void createMainWindow();
void destroyMainWindow();

Vec2i windowSize();
bool windowHasFocus();

NativeWindow nativeWindow();

////////////////////////////////////////////////////////////
// Input / Cursor

float inputValue(MkbInput);
float inputValue(GamepadInput);

Vec2 cursorPosition();
Vec2 cursorDelta();
Vec2 scrollDelta();

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
