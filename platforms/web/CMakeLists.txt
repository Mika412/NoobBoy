add_executable(NoobBoy main_emscripten.cpp)
target_link_libraries(NoobBoy PRIVATE NoobBoyCore)

set_target_properties(NoobBoy PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)
