//
// Created by Valerio Formato on 26-Feb-26.
//

#include "HW/Screen.h"
#include "HW/PPU.h"
#include "Tools/PPUPalette.h"
#include "common/ranges_compat.h"

#include <spdlog/fmt/ranges.h>

#include <print>

namespace BNES {
namespace HW {
ErrorOr<void> Screen::Init(const SDL::Window &window) {
  m_texture = TRY(SDL::Buffer::FromSize(NES_SCREEN_W, NES_SCREEN_H).and_then([&window](SDL::Buffer &&buffer) {
    return SDL::Texture::FromBuffer(window.Renderer(), std::move(buffer));
  }));

  return {};
}

ErrorOr<void> Screen::FillFromPPU(const PPU &ppu) {
  static constexpr auto tile_width = PPU::TILE_WIDTH;
  static constexpr auto tile_height = PPU::TILE_HEIGHT;

  auto &buffer = m_texture.Buffer();

  static constexpr auto tile_memory_size = PPU::TILE_MEMORY_SIZE;
  auto chr_tiles = ppu.CharacterRom() | rv::chunk(tile_memory_size);

  using TilePixelData = std::array<SDL::Pixel, tile_width * tile_height>;

  const auto bank_idx = ppu.BankIndex();

  const auto nametable = ppu.ActiveNametable().subspan(0, 960);
  const auto attribute_table = ppu.ActiveNametable().subspan(960, 64);
  const auto bg_palette_indices = attribute_table | rv::transform([](uint8_t value) {
                                    std::array<uint8_t, 4> quad_palettes{0};
                                    for (const auto &[idx, palette_idx] : rv::enumerate(quad_palettes)) {
                                      palette_idx = (value >> (idx * 2)) & 0b11;
                                    }
                                    return quad_palettes;
                                  }) |
                                  rv::join | rg::to<std::vector>();

  for (const auto &[tile_position_idx, tile] : rv::enumerate(
           nametable | rv::transform([&](uint8_t tile_idx) { return chr_tiles[256 * bank_idx + tile_idx]; }))) {

    const auto starting_pixel_x = (tile_position_idx * tile_width) % buffer.Width();
    const auto starting_pixel_y = (tile_position_idx / (buffer.Width() / tile_width)) * tile_height;

    const auto tile_x = tile_position_idx % (buffer.Width() / tile_width);
    const auto tile_y = tile_position_idx / (buffer.Width() / tile_width);

    const auto attr_x = tile_x / 4;
    const auto attr_y = tile_y / 4;
    const auto quad_idx = ((tile_y % 4) / 2) * 2 + ((tile_x % 4) / 2);
    const auto bg_palette_idx = bg_palette_indices[(attr_y * 8 + attr_x) * 4 + quad_idx];

    TilePixelData tile_pixels;

    auto tile_data = PPU::DecodeTile(tile);
    std::ranges::copy(tile_data | rv::transform([&](uint8_t value) {
                        const auto palette = ppu.BackgroundPalette(bg_palette_idx);
                        const auto color_value = value ? palette[value] : ppu.BackgroundColor();
                        return PPUPalette[color_value];
                      }),
                      tile_pixels.begin());

    for (const auto [index, pixel] : rv::enumerate(tile_pixels)) {
      auto pixel_x = (index % tile_width) + starting_pixel_x;
      auto pixel_y = (index / tile_width) + starting_pixel_y;
      TRY(buffer.WritePixel(pixel_x, pixel_y, pixel));
    }
  }

  return {};
}

ErrorOr<void> Screen::DrawScreen(SDL::Window &window, float scale_factor) {
  m_texture.SetScaleMode(SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
  TRY(m_texture.Update());

  TRY(m_texture.RenderAtPositionAndScale(window.Renderer(), {0, 0}, scale_factor));

  return {};
}
} // namespace HW
} // namespace BNES
