#pragma once

#include "anker_file_utils.hpp"

namespace Anker {

// An image reader utility that allows easy access to pixel data of PNG and JPG
// images.
class Image {
  public:
	explicit Image(std::span<const uint8_t>, int desiredChannels = 4);
	explicit Image(const fs::path&, int desiredChannels = 4);
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&&) noexcept = delete;
	Image& operator=(Image&&) noexcept = delete;
	~Image() noexcept;

	explicit operator bool() const { return m_pixels; }
	const uint8_t* pixels() const { return m_pixels; }

	template <typename T>
	const T& pixel(int index) const
	{
		return reinterpret_cast<const T&>(m_pixels + m_pixelStride * index);
	}

	template <typename T>
	const T& pixel(int x, int y) const
	{
		return pixel(y * m_width + x);
	}

	int width() const { return m_width; }
	int height() const { return m_height; }
	int channels() const { return m_channels; }
	int pixelStride() const { return m_pixelStride; }
	int rowPitch() const { return m_width * m_pixelStride; }

  private:
	const uint8_t* m_pixels = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	int m_pixelStride = 0;
};

} // namespace Anker
