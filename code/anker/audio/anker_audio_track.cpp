#include <anker/audio/anker_audio_track.hpp>

#include <anker/core/anker_data_loader.hpp>

namespace Anker {

AudioTrack::~AudioTrack()
{
	if (m_chunk) {
		Mix_FreeChunk(m_chunk);
	}
}

Status AudioTrack::load(std::string_view identifier)
{
	if (m_chunk) {
		Mix_FreeChunk(m_chunk);
		m_chunk = nullptr;
	}

	ByteBuffer buffer;
	ANKER_TRY(g_assetDataLoader.load(buffer, std::string{identifier} + ".opus"));

	SDL_RWops* src = SDL_RWFromConstMem(static_cast<const void*>(buffer.data()), int(buffer.size()));
	if (!src) {
		ANKER_ERROR("{}: SDL_RWFromConstMem failed: {}", identifier, SDL_GetError());
		return ReadError;
	}

	m_chunk = Mix_LoadWAV_RW(src, 1);
	if (!m_chunk) {
		ANKER_ERROR("{}: Mix_LoadWAV_RW failed: {}", identifier, Mix_GetError());
		return ReadError;
	}

	return Ok;
}

} // namespace Anker
