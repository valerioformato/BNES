set(SDLBind_SRC Media.cpp WindowHandle.cpp
        TextureHandle.cpp
        Buffer.cpp
        Buffer.h
)

add_library(SDLBind STATIC ${SDLBind_SRC})
target_link_libraries(SDLBind PUBLIC SDL3::SDL3-static spdlog::spdlog)
