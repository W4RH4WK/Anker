#pragma once

namespace Anker {

class Window;
class RenderDevice;

class ImguiSystem {
  public:
	ImguiSystem(Window&, RenderDevice&);
	ImguiSystem(const ImguiSystem&) = delete;
	ImguiSystem& operator=(const ImguiSystem&) = delete;
	ImguiSystem(ImguiSystem&&) noexcept = delete;
	ImguiSystem& operator=(ImguiSystem&&) noexcept = delete;
	~ImguiSystem();

	void newFrame();

	void draw();
};

} // namespace Anker
