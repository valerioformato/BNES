add_executable(test-text main.cpp)
target_include_directories(test-text PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test-text PRIVATE NESTools SDLBind)

add_executable(sdl-tutorial sdl-tutorial.cpp)
target_include_directories(sdl-tutorial PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(sdl-tutorial PRIVATE NESTools SDLBind SDL3)
