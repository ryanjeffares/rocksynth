cmake_minimum_required(VERSION 3.16)
project(rocksynth)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(fmt)

find_package(RtAudio REQUIRED)

add_executable(rocksynth src/main.cpp src/Audio/AudioBuffer.cpp src/Synth/Oscillator.cpp)
set_property(TARGET rocksynth PROPERTY CXX_STANDARD 20)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(rocksynth PRIVATE ROCKSYNTH_DEBUG)
endif()

target_compile_options(rocksynth PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(rocksynth PRIVATE fmt::fmt RtAudio::rtaudio)
