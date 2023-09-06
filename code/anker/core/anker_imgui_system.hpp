#pragma once

namespace Anker {

class RenderDevice;

class ImguiSystem {
  public:
	ImguiSystem(RenderDevice&);
	ImguiSystem(const ImguiSystem&) = delete;
	ImguiSystem& operator=(const ImguiSystem&) = delete;
	ImguiSystem(ImguiSystem&&) noexcept = delete;
	ImguiSystem& operator=(ImguiSystem&&) noexcept = delete;
	~ImguiSystem() noexcept;

	void newFrame();

	void draw();

private:
	RenderDevice& m_renderDevice;
};

} // namespace Anker
