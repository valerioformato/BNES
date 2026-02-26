//
// Created by Valerio Formato on 26-Feb-26.
//

#include "HW/Screen.h"
#include "HW/PPU.h"
#include "Tools/PPUPalette.h"
#include "common/ranges_compat.h"

#include <spdlog/fmt/ranges.h>

namespace BNES {
namespace HW {
ErrorOr<void> Screen::Init(const SDL::Window &window) {
  auto buffer = TRY(SDL::Buffer::FromSize(NES_SCREEN_W, NES_SCREEN_H));
  m_texture = TRY(SDL::Texture::FromBuffer(window.Renderer(), std::move(buffer)));

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
  spdlog::trace("vram content: {}", nametable);

  for (const auto &[tile_position_idx, tile] : rv::enumerate(
           nametable | rv::transform([&](uint8_t tile_idx) { return chr_tiles[256 * bank_idx + tile_idx]; }))) {

    TilePixelData tile_pixels;
    std::ranges::copy(PPU::DecodeTile(tile) | rv::transform([](uint8_t value) {
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
                        default:
                          std::unreachable();
                        }
                        return PPUPalette[idx];
                      }),
                      tile_pixels.begin());

    auto starting_pixel_x = (tile_position_idx * tile_width) % buffer.Width();
    auto starting_pixel_y = (tile_position_idx / (buffer.Width() / tile_width)) * tile_height;

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