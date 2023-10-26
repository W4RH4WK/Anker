#include <anker/core/anker_actions.hpp>

#include <anker/platform/anker_platform.hpp>

namespace Anker {

void Action::tick(float dt)
{
	m_value = 0;
	if (bindingMkb1) {
		m_value += Platform::inputValue(*bindingMkb1);
	}
	if (bindingMkb2) {
		m_value += Platform::inputValue(*bindingMkb2);
	}
	if (bindingGamepad1) {
		m_value += Platform::inputValue(*bindingGamepad1);
	}
	if (bindingGamepad2) {
		m_value += Platform::inputValue(*bindingGamepad2);
	}
	m_value = clamp01(m_value);

	if (down()) {
		m_previousTime = m_currentTime;
		m_currentTime += dt;
	} else {
		m_previousTime = 0;
		m_currentTime = 0;
	}

	m_consumed = false;
}

} // namespace Anker
