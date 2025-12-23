add_executable(snake_standalone main_standalone.cpp)
target_include_directories(snake_standalone PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(snake_standalone PRIVATE NESHW NESTools SDLBind)

add_executable(snake_rom main_rom.cpp)
target_include_directories(snake_rom PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(snake_rom PRIVATE NESHW NESTools SDLBind)
