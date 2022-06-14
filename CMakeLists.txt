cmake_minimum_required(VERSION 3.5)

project (GameBoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Curses REQUIRED)

include_directories("/usr/local/include")
include_directories("/opt/homebrew/include")
include_directories("/opt/homebrew/lib")
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIR})
include_directories(${CURSES_INCLUDE_DIR})

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Werror -std=c++14")
# set (CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Werror -std=c++14 -lSDL2 -lSDL2_ttf")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")


include_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_SOURCE_DIR}/src)

file (GLOB SOURCE_FILES "${source_dir}/*.cpp")
add_executable(GameBoy ${SOURCE_FILES})
target_link_libraries(GameBoy ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${CURSES_LIBRARIES} SDL2)
