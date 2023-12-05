#pragma once

#include <SDL_mixer.h>

namespace Anker {

class AudioTrack {
  public:
	AudioTrack() = default;
	AudioTrack(const AudioTrack&) = delete;
	AudioTrack& operator=(const AudioTrack&) = delete;
	AudioTrack(AudioTrack&&) noexcept = delete;
	AudioTrack& operator=(AudioTrack&&) noexcept = delete;
	~AudioTrack();

	operator Mix_Chunk*() { return m_chunk; }

	Status load(std::string_view identifier);

  private:
	Mix_Chunk* m_chunk = nullptr;
};

} // namespace Anker
