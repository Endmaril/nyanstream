cmake_minimum_required(VERSION 2.8)

project(nyanstream)

find_package(SDL REQUIRED)
#find_package(SDL_image REQUIRED)
#find_package(SDL_sound REQUIRED)

if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!")
endif ( NOT SDL_FOUND )

add_executable(nyanstream
    src/nyanstream.cc
    src/client.cc src/client.hh
    src/server.cc src/server.hh
    )
target_link_libraries(nyanstream
    ${SDL_LIBRARY}
)
