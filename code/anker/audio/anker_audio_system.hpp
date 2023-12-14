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

	float musicVolume();
	void setMusicVolume(float volume);

	void playEffect(AssetPtr<AudioTrack>, float volume = 1);

  private:
	static constexpr int EffectChannelCount = 64;

	AssetPtr<AudioStream> m_music;
	std::array<AssetPtr<AudioTrack>, EffectChannelCount> m_effects = {};
};

} // namespace Anker
