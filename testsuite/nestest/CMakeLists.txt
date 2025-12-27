add_executable(nestest main.cpp)
target_include_directories(nestest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(nestest PRIVATE NESHW NESTools SDLBind)
