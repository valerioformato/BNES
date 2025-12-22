add_executable(test-text main.cpp)
target_include_directories(test-text PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test-text PRIVATE NESTools SDLBind)
