#pragma once
//
// Компоненты для 3D рендера (bgfx)

#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <cstdint>

namespace EDD::Managers {

// === Transform 3D ===
struct Transform3D {
  float position_[3]{0.0f, 0.0f, 0.0f};
  float rotation_[3]{0.0f, 0.0f, 0.0f};  // yaw, pitch, roll (или XYZ радианы)
  float scale_[3]{1.0f, 1.0f, 1.0f};
};

// === Вершина PNUV ===
struct VertexPNUTB {
  float pos_[3];
  float normal_[3];
  float uv_[2];

  static bgfx::VertexLayout Get_Layout() {
    bgfx::VertexLayout L;
    L.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    return L;
  }
};

// === Меш ===
struct Mesh3D {
  bgfx::VertexBufferHandle vbh_ = BGFX_INVALID_HANDLE;
  bgfx::IndexBufferHandle ibh_ = BGFX_INVALID_HANDLE;
  uint32_t index_count_{0};
  bool is_loaded_{false};
};

// === Материал/программа ===
struct Material3D {
  bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
};

// === Камера ===
struct Camera3D {
  float eye_[3]{0.0f, 0.0f, -5.0f};
  float at_[3]{0.0f, 0.0f, 0.0f};
  float up_[3]{0.0f, 1.0f, 0.0f};
  float fov_y_deg_{60.0f};
  float z_near_{0.1f};
  float z_far_{100.0f};
};

}  // namespace EDD::Managers
