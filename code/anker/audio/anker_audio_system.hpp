#pragma once

#include <anker/audio/anker_audio_track.hpp>
#include <anker/audio/anker_audio_stream.hpp>

namespace Anker {

class AudioSystem {
  public:
	AudioSystem();
	AudioSystem(const AudioSystem&) = delete;
	AudioSystem& operator=(const AudioSystem&) = delete;
	AudioSystem(AudioSystem&&) noexcept = delete;
	AudioSystem& operator=(AudioSystem&&) noexcept = delete;
	~AudioSystem();

	void playMusic(AudioStream&);
};

} // namespace Anker
