#pragma once

#include <anker/core/anker_actions.hpp>

namespace Anker {

class ImguiSystem;

class InputSystem {
  public:
	InputSystem(ImguiSystem&);
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) noexcept = delete;
	InputSystem& operator=(InputSystem&&) noexcept = delete;

	void tick(float);

	const Actions& actions() const { return m_actions; }

  private:
	ImguiSystem& m_imgui;

	Actions m_actions;
};

} // namespace Anker
