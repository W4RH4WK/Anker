#pragma once

#include <anker/core/anker_actions.hpp>

namespace Anker {

class InputSystem {
  public:
	InputSystem() = default;
	InputSystem(const InputSystem&) = delete;
	InputSystem& operator=(const InputSystem&) = delete;
	InputSystem(InputSystem&&) noexcept = delete;
	InputSystem& operator=(InputSystem&&) noexcept = delete;

	void tick(float);

	const Actions& actions() const { return m_actions; }

  private:
	Actions m_actions;
};

} // namespace Anker
