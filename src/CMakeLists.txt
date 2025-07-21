include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(HW)
add_subdirectory(SDL)

add_executable(BNES main.cpp)
target_link_libraries(BNES PRIVATE SDLBind)
