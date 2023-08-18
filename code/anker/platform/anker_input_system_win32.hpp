#pragma once

#include <anker/core/anker_actions.hpp>

namespace Anker {

class Window;
class ImguiSystem;

class InputSystem {
  public:
	InputSystem(Window&, ImguiSystem&);
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) noexcept = delete;
	InputSystem& operator=(InputSystem&&) noexcept = delete;

	void tick(float);

	void hideCursor();

	const Actions& actions() const { return m_actions; }

	void onScroll(float xoffset);

  private:
	Window& m_window;
	ImguiSystem& m_imgui;

	Actions m_actions;

	bool m_hideCursor = false;
	Vec2 m_previousCursorPosition = Vec2(0);

	float m_scrollDelta = 0;
};

} // namespace Anker
