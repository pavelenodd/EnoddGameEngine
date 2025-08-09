#pragma once
//
// Создание текстур bgfx из sf::Image

#include <bgfx/bgfx.h>

#include <SFML/Graphics/Image.hpp>
#include <cstdint>

namespace EDD::Tools {

inline bgfx::TextureHandle Create_Texture2D_FromSFML(const char* path,
                                                     bool is_rgba = true) {
  sf::Image L_img;
  if (!L_img.loadFromFile(path)) return BGFX_INVALID_HANDLE;

  const auto* L_pixels = L_img.getPixelsPtr();  // RGBA8
  const uint32_t L_w = L_img.getSize().x;
  const uint32_t L_h = L_img.getSize().y;

  const bgfx::Memory* L_mem = bgfx::copy(L_pixels, L_w * L_h * 4);
  uint64_t L_flags = 0;  // мипы подготовим оффлайн (shaderc/texturec), чтобы не усложнять

  auto L_th = bgfx::createTexture2D((uint16_t)L_w,
                                    (uint16_t)L_h,
                                    false /*hasMips*/,
                                    1 /*layers*/,
                                    is_rgba ? bgfx::TextureFormat::RGBA8
                                            : bgfx::TextureFormat::BGRA8,
                                    L_flags,
                                    L_mem);

  return L_th;
}

// 1x1 белая текстура
inline bgfx::TextureHandle Create_White_Texture() {
  uint32_t L_white = 0xffffffff;
  const bgfx::Memory* L_mem = bgfx::copy(&L_white, sizeof(uint32_t));
  return bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, L_mem);
}

}  // namespace EDD::Tools
