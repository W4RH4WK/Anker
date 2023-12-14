#pragma once

#include <anker/audio/anker_audio_stream.hpp>
#include <anker/audio/anker_audio_track.hpp>
#include <anker/core/anker_asset.hpp>

namespace Anker {

class AudioSystem {
  public:
	AudioSystem();
	AudioSystem(const AudioSystem&) = delete;
	AudioSystem& operator=(const AudioSystem&) = delete;
	AudioSystem(AudioSystem&&) noexcept = delete;
	AudioSystem& operator=(AudioSystem&&) noexcept = delete;
	~AudioSystem();

	void playMusic(AssetPtr<AudioStream>, float fadeTime = 0.5f);
	void stopMusic(float fadeTime = 0.5f);

  private:
	AssetPtr<AudioStream> m_music;
};

} // namespace Anker
