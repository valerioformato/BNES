//
// Created by vformato on 10/17/25.
//

#include "Tools/PPUDebugger.h"
#include "SDLBind/Graphics/Buffer.h"
#include "SDLBind/Graphics/Color.h"
#include "SDLBind/Graphics/RenderTarget.h"
#include "SDLBind/Graphics/Texture.h"
#include "Tools/PPUPalette.h"
#include "common/ranges_compat.h"

#include <spdlog/fmt/ranges.h>

#include <algorithm>
#include <bitset>
#include <numeric>

namespace BNES::Tools {

using HW::PPU;

ErrorOr<SDL::Texture> PPUDebugger::BuildPaletteTexture(const SDL::Window &main_window) {
  static constexpr unsigned int padding = 4;
  static constexpr unsigned int PALETTE_BLOCK_WIDTH = 256;
  static constexpr unsigned int PALETTE_BLOCK_HEIGTH = 256;
  static constexpr unsigned int COLOR_BLOCK_WIDTH = PALETTE_BLOCK_WIDTH / 4;

  SDL::Texture target_texture = TRY(
      SDL::Buffer::FromSize(PALETTE_BLOCK_WIDTH, PALETTE_BLOCK_HEIGTH).and_then([&main_window](SDL::Buffer &&buffer) {
        return SDL::Texture::FromBuffer(main_window.Renderer(), std::move(buffer), SDL::Texture::Access::Target);
      }));

  auto at = SDL::ActiveRenderTarget(main_window.Renderer(), target_texture);
  SDL_RenderClear(main_window.Renderer());

  int y_offset = 0;

  for (uint8_t palette_index = 0; palette_index < 4; ++palette_index) {
    const auto palette_data = m_ppu->BackgroundPalette(0);
    const auto palette_data_text = std::format("0x{:02X} 0x{:02X} 0x{:02X} 0x{:02X}", palette_data[0], palette_data[1],
                                               palette_data[2], palette_data[3]);

    SDL::Texture palette_text_texture =
        TRY(SDL::Texture::FromText(main_window.Renderer(), SDL::TextSpec{
                                                               .content = palette_data_text,
                                                               .color = SDL::Color{255, 255, 255, 255},
                                                               .font = m_font,
                                                           }));

    TRY(palette_text_texture.RenderAtPosition(main_window.Renderer(), {padding, y_offset}));

    y_offset += static_cast<int>(palette_text_texture.Buffer().Height()) + padding;

    auto buffer = TRY(SDL::Buffer::FromSize(128, 16));
    rg::generate(buffer.Pixels(), [&, pixel_idx = 0]() mutable {
      auto color_idx = (pixel_idx % PALETTE_BLOCK_WIDTH) / COLOR_BLOCK_WIDTH;
      return color_idx != 0 ? HW::PPUPalette[palette_data[color_idx]] : HW::PPUPalette[m_ppu->BackgroundColor()];
    });

    SDL::Texture color_texture = TRY(SDL::Texture::FromBuffer(main_window.Renderer(), std::move(buffer)));
    color_texture.SetScaleMode(SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
    TRY(color_texture.Update());

    TRY(color_texture.RenderAtPosition(main_window.Renderer(), {padding, y_offset}));

    y_offset += static_cast<int>(color_texture.Buffer().Height()) + 2 * padding;
  }

  return target_texture;
}

ErrorOr<SDL::Texture> PPUDebugger::BuildChrRomTexture(const SDL::Window &main_window) {
  static constexpr unsigned int padding = 4;
  static constexpr unsigned int TABLE_WIDTH = PPU::TILE_WIDTH * 16;
  static constexpr unsigned int BUFFER_WIDTH = TABLE_WIDTH * 2 + padding;
  static constexpr unsigned int BUFFER_HEIGHT = PPU::TILE_HEIGHT * 16;

  constexpr unsigned int SECOND_TABLE_STARTING_X = TABLE_WIDTH + padding;

  SDL::Texture texture =
      TRY(SDL::Buffer::FromSize(BUFFER_WIDTH, BUFFER_HEIGHT).and_then([&main_window](SDL::Buffer &&buffer) {
        return SDL::Texture::FromBuffer(main_window.Renderer(), std::move(buffer));
      }));
  SDL::Buffer &chr_rom_buffer = texture.Buffer();

  static constexpr auto tile_memory_size = PPU::TILE_MEMORY_SIZE;

  const auto &chr_rom = m_ppu->m_bus->Rom().character_rom;
  auto tiles = chr_rom | rv::chunk(tile_memory_size);

  static constexpr auto tile_width = PPU::TILE_WIDTH;
  static constexpr auto tile_height = PPU::TILE_HEIGHT;

  using TilePixelData = std::array<SDL::Pixel, tile_width * tile_height>;

  for (const auto &[tile_index, tile] : rv::enumerate(tiles)) {
    TilePixelData tile_pixels;
    auto tile_data = PPU::DecodeTile(tile);
    std::ranges::copy(tile_data | rv::transform([](uint8_t value) {
                        // FIXME: we should actually look into the palette data and choose the right color. For now
                        //        let's make it bright enough to be seen on screen...
                        switch (value) {
                        case 0:
                          return SDL::Pixel{{0, 0, 0, 255}};
                        case 1:
                          return SDL::Pixel{{85, 85, 85, 255}};
                        case 2:
                          return SDL::Pixel{{170, 170, 170, 255}};
                        case 3:
                          return SDL::Pixel{{255, 255, 255, 255}};
                        default:
                          std::unreachable();
                        }
                        std::unreachable();
                      }),
                      tile_pixels.begin());

    bool second_table = tile_index >= 256;

    auto starting_pixel_x = (tile_index * tile_width) % TABLE_WIDTH + (second_table * SECOND_TABLE_STARTING_X);
    auto starting_pixel_y = ((tile_index % 256) / (TABLE_WIDTH / tile_width)) * tile_height;

    for (const auto [index, pixel] : rv::enumerate(tile_pixels)) {
      auto pixel_x = (index % tile_width) + starting_pixel_x;
      auto pixel_y = (index / tile_width) + starting_pixel_y;
      TRY(chr_rom_buffer.WritePixel(pixel_x, pixel_y, pixel));
    }
  }

  texture.SetScaleMode(SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
  TRY(texture.Update());

  return texture;
}

ErrorOr<SDL::Texture> PPUDebugger::BuildPPURegisterText(const SDL::Window &main_window) {
  using time_resolution = std::chrono::microseconds;
  float fps = 0.0f;
  auto numerator = std::chrono::duration_cast<time_resolution>(std::chrono::seconds(1)).count();
  if (auto denominator = std::chrono::duration_cast<time_resolution>(m_ppu->m_last_frame_time).count();
      denominator > 0) {
    fps = static_cast<decltype(fps)>(numerator) / denominator;
  }

  std::vector<std::string> lines;
  lines.push_back(fmt::format("{:4.2f} PPU FPS", fps));
  lines.push_back(fmt::format("Scanline: {}", m_ppu->m_current_scanline));
  lines.push_back(fmt::format("PPUSTATUS: {:08b}", m_ppu->m_status_register));
  lines.push_back(fmt::format("PPUCTRL:   {:08b}", m_ppu->m_control_register));
  lines.push_back(fmt::format("PPUMASK:   {:08b}", m_ppu->m_mask_register));
  lines.push_back(fmt::format("PPUADDR:   0x{:04X}", m_ppu->m_address_register));

  std::string content = std::ranges::fold_left(
      lines, std::string{}, [](auto &&current, auto &&text) { return fmt::format("{}{}\n", current, text); });

  SDL::TextSpec text_content{
      .content = content,
      .font = m_font,
      .color = SDL::Color{255, 255, 255, 255},
      .wrapping = SDL::TextWrapping::Wrapped,
      .wrap_size = static_cast<unsigned int>(main_window.Size()[1] / 2),
  };

  return SDL::Texture::FromText(main_window.Renderer(), text_content);
}

} // namespace BNES::Tools
