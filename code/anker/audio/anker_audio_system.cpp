#include <anker/audio/anker_audio_system.hpp>

#include <anker/core/anker_data_loader.hpp>

#include <SDL_mixer.h>

namespace Anker {

AudioSystem::AudioSystem()
{
	int flags = MIX_INIT_OGG | MIX_INIT_OPUS;
	int inited = Mix_Init(flags);
	if (flags != inited) {
		ANKER_ERROR("Mix_Init failed: {}", Mix_GetError());
		return;
	}

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) != 0) {
		ANKER_ERROR("Mix_OpenAudio failed: {}", Mix_GetError());
	}

	int allocatedChannels = Mix_AllocateChannels(EffectChannelCount);
	if (allocatedChannels != EffectChannelCount) {
		ANKER_ERROR("Mix_AllocateChannels: Could not allocate desired number of effect channels allocatedChannels={} "
		            "EffectChannelCount={}",
		            allocatedChannels, EffectChannelCount);
	}
}

AudioSystem::~AudioSystem()
{
	m_music = nullptr;
	for (auto& effect : m_effects) {
		effect = nullptr;
	}
	Mix_Quit();
}

void AudioSystem::playMusic(AssetPtr<AudioStream> music, float fadeTime)
{
	if (!music) {
		stopMusic();
		return;
	}

	if (fadeTime <= 0) {
		Mix_PlayMusic(*music, -1);
	} else {
		Mix_FadeInMusic(*music, -1, int(fadeTime * 1000.0f));
	}

	m_music = music;
}

void AudioSystem::stopMusic(float fadeTime)
{
	if (fadeTime <= 0) {
		Mix_HaltMusic();
	} else {
		Mix_FadeOutMusic(int(fadeTime * 1000.0f));
	}
	m_music = nullptr;
}

void AudioSystem::playEffect(AssetPtr<AudioTrack> effect, float volume)
{
	ANKER_CHECK(effect);

	int channel = Mix_PlayChannel(-1, *effect, 0);
	if (channel < 0) {
		ANKER_WARN("Not enough audio channels available");
		return;
	}

	Mix_Volume(channel, int(std::clamp(volume, 0.0f, 1.0f) * MIX_MAX_VOLUME));

	ANKER_CHECK(channel < m_effects.size());
	m_effects[channel] = effect;
}

} // namespace Anker
