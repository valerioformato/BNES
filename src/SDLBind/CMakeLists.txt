set(SDLBind_SRC Graphics/Window.cpp Graphics/Texture.cpp Graphics/Surface.cpp Graphics/Buffer.cpp Graphics/Buffer.h
                Text/Font.cpp
)

add_library(SDLBind STATIC ${SDLBind_SRC})
target_link_libraries(SDLBind PUBLIC SDL3::SDL3-static SDL3_ttf::SDL3_ttf spdlog::spdlog magic_enum)
