#pragma once

namespace Anker {

struct PlayerController {};

class InputSystem;
class Scene;

class PlayerControllerSystem {
  public:
	PlayerControllerSystem(InputSystem&);
	PlayerControllerSystem(const PlayerControllerSystem&) = delete;
	PlayerControllerSystem& operator=(const PlayerControllerSystem&) = delete;
	PlayerControllerSystem(PlayerControllerSystem&&) noexcept = delete;
	PlayerControllerSystem& operator=(PlayerControllerSystem&&) noexcept = delete;

	void tick(float, Scene&);

  private:
	InputSystem& m_inputSystem;
};

} // namespace Anker
