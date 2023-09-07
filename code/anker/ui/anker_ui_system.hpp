#pragma once

#include <anker/ui/anker_font.hpp>

namespace Anker {

struct Font;
class DataLoader;
class RenderDevice;

class UISystem {
  public:
	UISystem(DataLoader&, RenderDevice&);
	UISystem(const UISystem&) = delete;
	UISystem& operator=(const UISystem&) = delete;
	UISystem(UISystem&&) noexcept = delete;
	UISystem& operator=(UISystem&&) noexcept = delete;

	Status loadFont(Font&, std::string_view identifier);

	const Font& systemFont() const { return m_systemFont; }

  private:
	DataLoader& m_dataLoader;
	RenderDevice& m_renderDevice;

	Font m_systemFont;
};

} // namespace Anker
