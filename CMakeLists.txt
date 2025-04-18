cmake_minimum_required(VERSION 3.5)
project (NoobBoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${EMSDK_PATH}/upstream/emscripten/cache/sysroot/lib/pkgconfig")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_FREETYPE=1 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} -s EXPORTED_RUNTIME_METHODS='[\"FS\",\"callMain\"]' -lembind")
else()
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
endif()

include_directories("/usr/local/include")
include_directories("/opt/homebrew/include")
include_directories("/opt/homebrew/lib")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/")

include_directories(${SOURCE_DIR})
link_directories(${SOURCE_DIR})

file (GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX "main(.*)\\.cpp")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(MAIN_FILE "${SOURCE_DIR}/main_emscripten.cpp")
else ()
    set(MAIN_FILE "${SOURCE_DIR}/main.cpp")
endif ()

list(APPEND SOURCE_FILES ${MAIN_FILE})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} SDL3::SDL3 SDL3_ttf::SDL3_ttf)