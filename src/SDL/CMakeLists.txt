set(SDLBind_SRC Media.cpp WindowHandle.cpp)

add_library(SDLBind STATIC ${SDLBind_SRC})
target_link_libraries(SDLBind PUBLIC SDL3::SDL3 spdlog::spdlog)
