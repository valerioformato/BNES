//
// Created by vformato on 12/23/25.
//

#include "SDLBind/Graphics/WindowCreateProperties.h"

#include <SDL3/SDL_video.h>

#include <utility>

namespace BNES::SDL {
constexpr std::string_view ToSDLValue(WindowCreateProperty prop) {
  switch (prop) {
  case WindowCreateProperty::AlwaysOnTop:
    return SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN;
  case WindowCreateProperty::Borderless:
    return SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN;
  case WindowCreateProperty::ConstrainPopup:
    return SDL_PROP_WINDOW_CREATE_CONSTRAIN_POPUP_BOOLEAN;
  case WindowCreateProperty::Focusable:
    return SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN;
  case WindowCreateProperty::ExternalGraphicsContext:
    return SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN;
  case WindowCreateProperty::Flags:
    return SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER;
  case WindowCreateProperty::Fullscreen:
    return SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN;
  case WindowCreateProperty::Height:
    return SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER;
  case WindowCreateProperty::Hidden:
    return SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN;
  case WindowCreateProperty::HighPixelDensity:
    return SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN;
  case WindowCreateProperty::Maximized:
    return SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN;
  case WindowCreateProperty::Menu:
    return SDL_PROP_WINDOW_CREATE_MENU_BOOLEAN;
  case WindowCreateProperty::Metal:
    return SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN;
  case WindowCreateProperty::Minimized:
    return SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN;
  case WindowCreateProperty::Modal:
    return SDL_PROP_WINDOW_CREATE_MODAL_BOOLEAN;
  case WindowCreateProperty::MouseGrabbed:
    return SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN;
  case WindowCreateProperty::Opengl:
    return SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN;
  case WindowCreateProperty::Parent:
    return SDL_PROP_WINDOW_CREATE_PARENT_POINTER;
  case WindowCreateProperty::Resizable:
    return SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN;
  case WindowCreateProperty::Title:
    return SDL_PROP_WINDOW_CREATE_TITLE_STRING;
  case WindowCreateProperty::Transparent:
    return SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN;
  case WindowCreateProperty::Tooltip:
    return SDL_PROP_WINDOW_CREATE_TOOLTIP_BOOLEAN;
  case WindowCreateProperty::Utility:
    return SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN;
  case WindowCreateProperty::Vulkan:
    return SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN;
  case WindowCreateProperty::Width:
    return SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER;
  case WindowCreateProperty::X:
    return SDL_PROP_WINDOW_CREATE_X_NUMBER;
  case WindowCreateProperty::Y:
    return SDL_PROP_WINDOW_CREATE_Y_NUMBER;
  case WindowCreateProperty::CocoaWindow:
    return SDL_PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER;
  case WindowCreateProperty::CocoaView:
    return SDL_PROP_WINDOW_CREATE_COCOA_VIEW_POINTER;
  case WindowCreateProperty::WaylandSurfaceRoleCustom:
    return SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN;
  case WindowCreateProperty::WaylandCreateEglWindow:
    return SDL_PROP_WINDOW_CREATE_WAYLAND_CREATE_EGL_WINDOW_BOOLEAN;
  case WindowCreateProperty::WaylandWlSurface:
    return SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER;
  case WindowCreateProperty::Win32Hwnd:
    return SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER;
  case WindowCreateProperty::Win32PixelFormatHwnd:
    return SDL_PROP_WINDOW_CREATE_WIN32_PIXEL_FORMAT_HWND_POINTER;
  case WindowCreateProperty::X11Window:
    return SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER;
  }

  std::unreachable();
}
} // namespace BNES::SDL
