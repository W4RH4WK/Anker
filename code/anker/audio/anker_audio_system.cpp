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
}

AudioSystem::~AudioSystem()
{
	m_music = nullptr;
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

} // namespace Anker
