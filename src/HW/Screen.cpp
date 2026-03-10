//
// Created by Valerio Formato on 26-Feb-26.
//

#include "HW/Screen.h"
#include "HW/PPU.h"
#include "Tools/PPUPalette.h"
#include "common/ranges_compat.h"

#include <spdlog/fmt/ranges.h>

namespace BNES::HW {
ErrorOr<void> Screen::Init(const SDL::Window &window) {
  m_texture = TRY(SDL::Buffer::FromSize(NES_SCREEN_W, NES_SCREEN_H).and_then([&window](SDL::Buffer &&buffer) {
    return SDL::Texture::FromBuffer(window.Renderer(), std::move(buffer));
  }));

  return {};
}

ErrorOr<void> Screen::FillFromPPU(const PPU &ppu) {
  if (ppu.RenderBackground()) {
    TRY(FillBackground(ppu));
  }

  if (ppu.RenderSprites()) {
    TRY(FillSprites(ppu));
  }
  return {};
}

static constexpr auto tile_width = PPU::TILE_WIDTH;
static constexpr auto tile_height = PPU::TILE_HEIGHT;
static constexpr auto tile_memory_size = PPU::TILE_MEMORY_SIZE;
using TilePixelData = std::array<SDL::Pixel, tile_width * tile_height>;

TilePixelData RenderBkgTile(std::span<const uint8_t> tile, uint8_t palette_idx, const PPU &ppu) {
  TilePixelData tile_pixels;

  auto tile_data = PPU::DecodeTile(tile);
  std::ranges::copy(tile_data | rv::transform([&](uint8_t value) {
                      const auto palette = ppu.BackgroundPalette(palette_idx);
                      const auto color_value = value ? palette[value] : ppu.BackgroundColor();
                      return PPUPalette[color_value];
                    }),
                    tile_pixels.begin());
  return tile_pixels;
}

TilePixelData RenderSprTile(std::span<const uint8_t> tile, uint8_t palette_idx, const PPU &ppu) {
  TilePixelData tile_pixels;

  auto tile_data = PPU::DecodeTile(tile);
  std::ranges::copy(tile_data | rv::transform([&](uint8_t value) {
                      const auto palette = ppu.SpritePalette(palette_idx);
                      const auto color_value = palette[value];
                      return value ? PPUPalette[color_value] : SDL::Pixel{{0, 0, 0, 0}};
                    }),
                    tile_pixels.begin());
  return tile_pixels;
}

ErrorOr<void> Screen::FillBackground(const PPU &ppu) {

  auto &buffer = m_texture.Buffer();

  auto chr_tiles = ppu.CharacterRom() | rv::chunk(tile_memory_size);

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

    TilePixelData tile_pixels = RenderBkgTile(tile, bg_palette_idx, ppu);

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

ErrorOr<void> Screen::FillSprites(const PPU &ppu) {
  auto &buffer = m_texture.Buffer();

  auto sprite_ppu_data = ppu.SpriteOAMData();

  auto chr_tiles = ppu.CharacterRom() | rv::chunk(tile_memory_size);

  const auto bank_idx = ppu.SpritePatternTableAddress();

  // TODO: we should check if there is a specific ordering for sprites to be rendered. The PPU doc is pretty long and we
  //       might be doing something different with this simple approach.
  for (const auto &sprite_data : sprite_ppu_data) {
    // FIXME: in case of 8x16 tiles the bank_idx is encoded in sprite_data.tile_index
    const auto sprite_raw = chr_tiles[256 * bank_idx + sprite_data.tile_index];

    TilePixelData tile_pixels = RenderSprTile(sprite_raw, sprite_data.palette_idx, ppu);

    for (const auto [index, pixel] : rv::enumerate(tile_pixels)) {
      auto lookup_pixel_x = (index % tile_width);
      if (sprite_data.flip_horizontal) {
        lookup_pixel_x = tile_width - lookup_pixel_x - 1;
      }
      auto lookup_pixel_y = (index / tile_width);
      if (sprite_data.flip_vertical) {
        lookup_pixel_y = tile_height - lookup_pixel_y - 1;
      }

      auto pixel_x = lookup_pixel_x + sprite_data.pos_x;
      auto pixel_y = lookup_pixel_y + sprite_data.pos_y;

      if (pixel_x >= buffer.Width() || pixel_y >= buffer.Height())
        continue;

      if (pixel.color.a == 0)
        continue;

      TRY(buffer.WritePixel(pixel_x, pixel_y, pixel));
    }
  }

  return {};
}
} // namespace BNES::HW
