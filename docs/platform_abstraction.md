# Platform Abstraction

*Platform* refers to the engine's operating system abstraction layer.
We use [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) along with [SDL mixer](https://github.com/libsdl-org/SDL_mixer/tree/SDL2) for now.
Note that we migrated from [GLFW](https://github.com/glfw/glfw) to SDL2, so there might be some rough edges here and there.

## `build_sdl2.rb`

While SDL2 provides pre-built libraries for Windows, we encountered issues with them and therefore build the libraries from source when needed.
The provided script `build_sdl2.rb` builds both, SDL2 and SDL mixer, and copies the relevant files to the repository.
The relevant files are checked into the repository so you don't have to build SDL2 from source.
