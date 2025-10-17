add_executable(snake main.cpp)
target_include_directories(snake PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(snake PRIVATE NESHW NESTools SDLBind)
