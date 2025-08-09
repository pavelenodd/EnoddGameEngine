#pragma once
//
// Компоненты для 2D рендера (bgfx)

#include <bgfx/bgfx.h>

#include <cstdint>

namespace EDD::Managers {

// === Transform 2D ===
struct Transform2D {
  float position_[2]{0.0f, 0.0f};  // экранные координаты (px)
  float rotation_rad_{0.0f};       // вокруг Z
  float scale_[2]{1.0f, 1.0f};
  float z_{0.0f};  // порядок рисования (0..1)
};

// === Вершина 2D: Pos(3) + Color + UV ===
struct Vertex2D {
  float pos_[3];   // x,y,z
  uint32_t rgba_;  // ABGR (bgfx формат)
  float uv_[2];

  static bgfx::VertexLayout Get_Layout() {
    bgfx::VertexLayout L;
    L.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true, false)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    return L;
  }
};

// === Спрайт 2D ===
struct Sprite2D {
  bgfx::TextureHandle tex_ = BGFX_INVALID_HANDLE;
  float size_[2]{100.0f, 100.0f};  // ширина/высота в px
  float uv_rect_[4]{0.0f,
                    0.0f,
                    1.0f,
                    1.0f};  // u0, v0, u1, v1 (пока не используем для quad по умолчанию)
  uint32_t color_abgr_{0xffffffff};  // модуль цвет
  bool is_loaded_{false};
};

// === Материал 2D (программа) ===
struct Material2D {
  bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
};

}  // namespace EDD::Managers
