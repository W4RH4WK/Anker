#include "anker_image_utils.hpp"

#include <stb_image.h>

namespace Anker {

Image::Image(std::span<const uint8_t> imageData, int desiredChannels) : m_pixelStride(desiredChannels)
{
	m_pixels = stbi_load_from_memory(imageData.data(), int(imageData.size()), //
	                                 &m_width, &m_height, &m_channels, desiredChannels);
}

Image::Image(const fs::path& filepath, int desiredChannels) : m_pixelStride(desiredChannels)
{
	m_pixels = stbi_load(filepath.string().c_str(), &m_width, &m_height, &m_channels, desiredChannels);
}

Image::~Image()
{
	stbi_image_free((void*)m_pixels);
}

} // namespace Anker
