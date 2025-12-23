//
// Created by vformato on 12/23/25.
//

#ifndef WINDOWCREATEPROPERTIES_H
#define WINDOWCREATEPROPERTIES_H

#include <string_view>

namespace BNES::SDL {
enum class WindowCreateProperty {
  AlwaysOnTop,
  Borderless,
  ConstrainPopup,
  Focusable,
  ExternalGraphicsContext,
  Flags,
  Fullscreen,
  Height,
  Hidden,
  HighPixelDensity,
  Maximized,
  Menu,
  Metal,
  Minimized,
  Modal,
  MouseGrabbed,
  Opengl,
  Parent,
  Resizable,
  Title,
  Transparent,
  Tooltip,
  Utility,
  Vulkan,
  Width,
  X,
  Y,
  CocoaWindow,
  CocoaView,
  WaylandSurfaceRoleCustom,
  WaylandCreateEglWindow,
  WaylandWlSurface,
  Win32Hwnd,
  Win32PixelFormatHwnd,
  X11Window,
};

constexpr std::string_view ToSDLValue(WindowCreateProperty prop);
} // namespace BNES::SDL

#endif // WINDOWCREATEPROPERTIES_H
