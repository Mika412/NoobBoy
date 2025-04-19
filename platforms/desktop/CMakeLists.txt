add_executable(NoobBoy main.cpp)
target_link_libraries(NoobBoy PRIVATE NoobBoyCore)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_libraries(NoobBoy PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
endif()

set_target_properties(NoobBoy PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)
