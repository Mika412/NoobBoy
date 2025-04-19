cmake_minimum_required(VERSION 3.10)
project(NoobBoy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_DESKTOP "Build the desktop application" OFF)
option(BUILD_WEB "Build the WebAssembly target" OFF)
option(BUILD_TERMINAL "Build the terminal version" OFF)
option(BUILD_TESTS "Build tests" OFF)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${EMSDK_PATH}/upstream/emscripten/cache/sysroot/lib/pkgconfig")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_FREETYPE=1 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[\"FS\",\"callMain\"]' -lembind")
else()
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
endif()

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

file(GLOB CORE_SOURCES "${SRC_DIR}/*.cpp")
list(FILTER CORE_SOURCES EXCLUDE REGEX "main.*\\.cpp")

add_library(NoobBoyCore STATIC ${CORE_SOURCES})
target_include_directories(NoobBoyCore PUBLIC ${INCLUDE_DIR})

if(BUILD_DESKTOP)
    add_subdirectory(platforms/desktop)
endif()
