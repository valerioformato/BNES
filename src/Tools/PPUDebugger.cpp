//
// Created by vformato on 10/17/25.
//

#include "Tools/PPUDebugger.h"
#include "SDLBind/Graphics/Texture.h"
#include "Tools/PPUPalette.h"

#include <spdlog/fmt/ranges.h>

#include <algorithm>
#include <numeric>
#include <ranges>

namespace BNES::Tools {

PPUDebugger::Window::Window(SDL::Buffer &&buffer)
    : m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                         .width = buffer.Width(),
                                         .height = buffer.Height(),
                                         .title = "PPU Debugger",
                                         .flags = SDL::WindowFlag::None,
                                     })
                   .value()) {}

ErrorOr<void> PPUDebugger::Window::Update(SDL::Texture &chr_rom_tex) {
  SDL_RenderClear(m_window.Renderer());

  TRY(chr_rom_tex.Render(m_window.Renderer()));

  SDL_RenderPresent(m_window.Renderer());

  return {};
}

ErrorOr<void> PPUDebugger::Update() {
  namespace rv = std::ranges::views;

  SDL::Buffer &chr_rom_buffer = m_chr_rom_texture.Buffer();

  static constexpr unsigned int tile_memory_size = 16;

  const auto &chr_rom = m_ppu->m_bus->Rom().character_rom;
  auto tiles = chr_rom | rv::chunk(tile_memory_size);

  static constexpr unsigned int tile_width = 8;
  static constexpr unsigned int tile_height = 8;

  using TilePixelValues = std::array<uint8_t, tile_width * tile_height>;

  for (const auto &[tile_index, tile] : rv::enumerate(tiles)) {
    TilePixelValues tile_pixels_v{0};
    for (const auto [index, byte] : rv::enumerate(tile)) {
      auto row_index = index % tile_width;
      auto bit_pos = index / tile_width;

      for (size_t x = 0; x < tile_width; ++x) {
        bool value = std::bitset<8>(byte)[tile_width - x - 1];
        tile_pixels_v[row_index * tile_width + x] |= (value << bit_pos);
      }
    }

    using TilePixelData = std::array<SDL::Pixel, tile_width * tile_height>;
    TilePixelData tile_pixels;
    std::ranges::copy(tile_pixels_v | rv::transform([](uint8_t value) {
                        // FIXME: we should actually look into the palette data and choose the right color. For now
                        //        let's make it bright enough to be seen on screen...
                        uint8_t idx{0};
                        switch (value) {
                        case 0:
                          idx = 0x01;
                          break;
                        case 1:
                          idx = 0x23;
                          break;
                        case 2:
                          idx = 0x27;
                          break;
                        case 3:
                          idx = 0x30;
                          break;
                        }
                        return HW::PPUPalette[idx];
                      }),
                      tile_pixels.begin());

    auto starting_pixel_x = (tile_index * tile_width) % chr_rom_buffer.Width();
    auto starting_pixel_y = (tile_index / (chr_rom_buffer.Width() / tile_width)) * tile_height;

    for (const auto [index, pixel] : rv::enumerate(tile_pixels)) {
      auto pixel_x = index % tile_width + starting_pixel_x;
      auto pixel_y = index / tile_width + starting_pixel_y;
      TRY(chr_rom_buffer.WritePixel(pixel_x, pixel_y, pixel));
    }
  }

  m_chr_rom_texture.SetScaleMode(SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
  TRY(m_chr_rom_texture.Update());

  return m_window.Update(m_chr_rom_texture);
}

} // namespace BNES::Tools
