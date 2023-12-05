#include <anker/audio/anker_audio_stream.hpp>

#include <anker/core/anker_data_loader.hpp>

namespace Anker {

AudioStream::~AudioStream()
{
	if (m_music) {
		Mix_FreeMusic(m_music);
	}
	if (m_rwops) {
		SDL_RWclose(m_rwops);
	}
}

Status AudioStream::load(std::string_view identifier)
{
	if (m_music) {
		Mix_FreeMusic(m_music);
		m_music = nullptr;
	}
	if (m_rwops) {
		SDL_RWclose(m_rwops);
		m_rwops = nullptr;
	}

	ANKER_TRY(g_assetDataLoader.load(m_buffer, std::string{identifier} + ".opus"));

	m_rwops = SDL_RWFromConstMem(static_cast<const void*>(m_buffer.data()), int(m_buffer.size()));
	if (!m_rwops) {
		ANKER_ERROR("{}: SDL_RWFromConstMem failed: {}", identifier, SDL_GetError());
		return ReadError;
	}

	m_music = Mix_LoadMUS_RW(m_rwops, 0);
	if (!m_music) {
		ANKER_ERROR("{}: Mix_LoadMUS_RW: failed: {}", identifier, Mix_GetError());
		return ReadError;
	}

	return Ok;
}

} // namespace Anker
