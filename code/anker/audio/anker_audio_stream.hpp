#pragma once

#include <SDL_mixer.h>

namespace Anker {

class AudioStream {
  public:
	AudioStream() = default;
	AudioStream(const AudioStream&) = delete;
	AudioStream& operator=(const AudioStream&) = delete;
	AudioStream(AudioStream&&) noexcept = delete;
	AudioStream& operator=(AudioStream&&) noexcept = delete;
	~AudioStream();

	operator Mix_Music*() { return m_music; }

	Status load(std::string_view identifier);

  private:
	ByteBuffer m_buffer;
	SDL_RWops* m_rwops = nullptr;
	Mix_Music* m_music = nullptr;
};

} // namespace Anker
