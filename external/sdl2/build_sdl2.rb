require 'fileutils'
require 'rake'

SDL_VERSION = '2.28.5'
SDL_MIXER_VERSION = '2.6.3'

Dir.chdir __dir__

####################################################################
# SDL

if not File.exist? 'build_sdl2'
  sh "git clone --branch release-#{SDL_VERSION} --depth 1 https://github.com/libsdl-org/SDL build_sdl2"
end

Dir.chdir 'build_sdl2' do
  sh "git checkout release-#{SDL_VERSION}"
  sh "git submodule update --init --recursive"

  sh %W[cmake
    -G "Visual Studio 17 2022"
    -B build_vs
    --install-prefix "#{File.expand_path('install')}"
    -D SDL_STATIC=OFF
  ].join(' ')

  sh 'cmake --build build_vs --config Debug -- /m'
  sh 'cmake --install build_vs --config Debug'

  sh 'cmake --build build_vs --config RelWithDebInfo -- /m'
  sh 'cmake --install build_vs --config RelWithDebInfo'
end

FileUtils.mkdir_p ['lib']
[
  ['build_sdl2/install/include', '.'],
  ['build_sdl2/install/bin/SDL2.dll', 'lib'],
  ['build_sdl2/install/bin/SDL2d.dll', 'lib'],
  ['build_sdl2/install/lib/SDL2.lib', 'lib'],
  ['build_sdl2/install/lib/SDL2d.lib', 'lib'],
].each { |from, to| FileUtils.cp_r(FileList[from], to) }

File.write('sdl2_version.txt', SDL_VERSION)

####################################################################
# SDL Mixer

if not File.exist? 'build_sdl2mixer'
  sh "git clone --branch release-#{SDL_MIXER_VERSION} --depth 1 https://github.com/libsdl-org/SDL_mixer build_sdl2mixer"
end

Dir.chdir 'build_sdl2mixer' do
  sh "git checkout release-#{SDL_MIXER_VERSION}"
  sh "git submodule update --init --recursive"

  sh %W[cmake
    -G "Visual Studio 17 2022"
    -B build_vs
    --install-prefix "#{File.expand_path('install')}"
    -D SDL2_DIR="#{File.expand_path('../build_sdl2/install/cmake')}"
    -D SDL2MIXER_SAMPLES=OFF
    -D SDL2MIXER_DEPS_SHARED=OFF
    -D SDL2MIXER_FLAC=OFF
    -D SDL2MIXER_MOD=OFF
    -D SDL2MIXER_OPUS_SHARED=OFF
  ].join(' ')

  sh 'cmake --build build_vs --config Debug -- /m'
  sh 'cmake --install build_vs --config Debug'

  sh 'cmake --build build_vs --config RelWithDebInfo -- /m'
  sh 'cmake --install build_vs --config RelWithDebInfo'
end

FileUtils.mkdir_p ['lib']
[
  ['build_sdl2mixer/install/include', '.'],
  ['build_sdl2mixer/install/bin/*.dll', 'lib'],
  ['build_sdl2mixer/install/lib/*.lib', 'lib'],
].each { |from, to| FileUtils.cp_r(FileList[from], to) }

File.write('sdl2mixer_version.txt', SDL_MIXER_VERSION)
