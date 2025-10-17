include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(HW)
add_subdirectory(SDLBind)
add_subdirectory(Tools)

add_executable(BNES main.cpp)
target_link_libraries(BNES PUBLIC SDLBind NESHW)
