#pragma once

#include <anker/audio/anker_audio_stream.hpp>
#include <anker/audio/anker_audio_track.hpp>

namespace Anker {

class AudioSystem {
  public:
	AudioSystem();
	AudioSystem(const AudioSystem&) = delete;
	AudioSystem& operator=(const AudioSystem&) = delete;
	AudioSystem(AudioSystem&&) noexcept = delete;
	AudioSystem& operator=(AudioSystem&&) noexcept = delete;
	~AudioSystem();

	void playMusic(AudioStream&, float fadeTime = 0.5f);
	void stopMusic(float fadeTime = 0.5f);

	// TODO AudioSystem should take (shared) ownership of currently playing
	// tracks and streams.
};

} // namespace Anker
