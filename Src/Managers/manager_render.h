#pragma once
//
// Менеджер рендера (3D + 2D) на bgfx, окно — SFML

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <type_traits>

#include "../Tools/utils_shader_bgfx.h"
#include "../Tools/utils_texture_bgfx.h"
#include "Items/components_2d.h"
#include "Items/components_3d.h"
#include "manager_base.h"
#include "manager_entity.h"

namespace EDD::Managers {

class Render : public Managers::Base {
 private:
 sf::RenderWindow* window_{nullptr};
 Entity* entity_manager_{nullptr};

 bool is_bgfx_inited_{false};
 uint16_t view3d_id_{0};
 uint16_t view2d_id_{1};

 // 2D: общий quad
 bgfx::VertexBufferHandle vbh_quad2d_ = BGFX_INVALID_HANDLE;
 bgfx::IndexBufferHandle ibh_quad2d_ = BGFX_INVALID_HANDLE;

 // Uniforms
 bgfx::UniformHandle s_tex0_ = BGFX_INVALID_HANDLE;   // sampler
 bgfx::UniformHandle u_color_ = BGFX_INVALID_HANDLE;  // vec4 (для 2D-модуляции цветом)

 // Программы по умолчанию (если у материала нет своей)
 bgfx::ProgramHandle prog_3d_ = BGFX_INVALID_HANDLE;
 bgfx::ProgramHandle prog_2d_ = BGFX_INVALID_HANDLE;

 // Текстура «белая 1x1» для цветных прямоугольников
 bgfx::TextureHandle tex_white_ = BGFX_INVALID_HANDLE;

 public:
  Render() {}
  ~Render() {
    FreeResources();
  }

  // === Update: один кадр ===
  void Update() override {
    if (!window_ || !entity_manager_ || !is_bgfx_inited_) return;

    const sf::Vector2u L_sz = window_->getSize();

    // ===== 3D =====
    float L_view3d[16];
    float L_proj3d[16];
    bool L_has_cam = false;

    entity_manager_->ForEach<Camera3D>([&](entt::entity, Camera3D& cam) {
      bx::mtxLookAt(L_view3d,
                    bx::Vec3{cam.eye_[0], cam.eye_[1], cam.eye_[2]},
                    bx::Vec3{cam.at_[0], cam.at_[1], cam.at_[2]},
                    bx::Vec3{cam.up_[0], cam.up_[1], cam.up_[2]});
      const float L_aspect = (float)L_sz.x / (float)(L_sz.y ? L_sz.y : 1);
      bx::mtxProj(L_proj3d,
                  cam.fov_y_deg_,
                  L_aspect,
                  cam.z_near_,
                  cam.z_far_,
                  bgfx::getCaps()->homogeneousDepth);
      L_has_cam = true;
    });
    if (!L_has_cam) {
      bx::mtxLookAt(L_view3d,
                    bx::Vec3{0.0f, 0.0f, -5.0f},
                    bx::Vec3{0.0f, 0.0f, 0.0f},
                    bx::Vec3{0.0f, 1.0f, 0.0f});
      const float L_aspect = (float)L_sz.x / (float)(L_sz.y ? L_sz.y : 1);
      bx::mtxProj(
          L_proj3d, 60.0f, L_aspect, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    }

    bgfx::setViewRect(view3d_id_, 0, 0, (uint16_t)L_sz.x, (uint16_t)L_sz.y);
    bgfx::setViewTransform(view3d_id_, L_view3d, L_proj3d);
    bgfx::setViewClear(
        view3d_id_, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x102030ff, 1.0f, 0);
    bgfx::touch(view3d_id_);

    entity_manager_->ForEach<Mesh3D, Transform3D, Material3D>(
        [&](entt::entity, Mesh3D& mesh, Transform3D& tr, Material3D& mat) {
          if (!mesh.is_loaded_ || !bgfx::isValid(mesh.vbh_) || !bgfx::isValid(mesh.ibh_))
            return;
          auto L_prog = bgfx::isValid(mat.program_) ? mat.program_ : prog_3d_;
          if (!bgfx::isValid(L_prog)) return;

          float L_mtx[16];
          bx::mtxSRT(L_mtx,
                     tr.scale_[0],
                     tr.scale_[1],
                     tr.scale_[2],
                     tr.rotation_[0],
                     tr.rotation_[1],
                     tr.rotation_[2],
                     tr.position_[0],
                     tr.position_[1],
                     tr.position_[2]);

          bgfx::setTransform(L_mtx);
          bgfx::setVertexBuffer(0, mesh.vbh_);
          bgfx::setIndexBuffer(mesh.ibh_);
          const uint64_t L_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                                   BGFX_STATE_CULL_CW | BGFX_STATE_MSAA;
          bgfx::setState(L_state);
          bgfx::submit(view3d_id_, L_prog);
        });

    // ===== 2D =====
    float L_view2d[16];
    bx::mtxIdentity(L_view2d);
    float L_proj2d[16];
    bx::mtxOrtho(L_proj2d,
                 0.0f,
                 (float)L_sz.x,
                 (float)L_sz.y,
                 0.0f,
                 0.0f,
                 1.0f,
                 0.0f,
                 bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewRect(view2d_id_, 0, 0, (uint16_t)L_sz.x, (uint16_t)L_sz.y);
    bgfx::setViewTransform(view2d_id_, L_view2d, L_proj2d);
    bgfx::touch(view2d_id_);

    if (bgfx::isValid(vbh_quad2d_) && bgfx::isValid(ibh_quad2d_)) {
      entity_manager_->ForEach<Sprite2D, Transform2D, Material2D>(
          [&](entt::entity, Sprite2D& spr, Transform2D& tr, Material2D& mat) {
            auto L_prog = bgfx::isValid(mat.program_) ? mat.program_ : prog_2d_;
            if (!bgfx::isValid(L_prog)) return;

            bgfx::TextureHandle L_tex = bgfx::isValid(spr.tex_) ? spr.tex_ : tex_white_;
            if (!bgfx::isValid(L_tex)) L_tex = tex_white_;

            // Матрица спрайта
            float L_mtxS[16], L_mtxR[16], L_mtxT[16], L_mtxSR[16], L_mtx[16];
            bx::mtxScale(
                L_mtxS, spr.size_[0] * tr.scale_[0], spr.size_[1] * tr.scale_[1], 1.0f);
            bx::mtxRotateZ(L_mtxR, tr.rotation_rad_);
            bx::mtxMul(L_mtxSR, L_mtxS, L_mtxR);
            bx::mtxTranslate(L_mtxT, tr.position_[0], tr.position_[1], tr.z_);
            bx::mtxMul(L_mtx, L_mtxSR, L_mtxT);

            bgfx::setTransform(L_mtx);
            bgfx::setVertexBuffer(0, vbh_quad2d_);
            bgfx::setIndexBuffer(ibh_quad2d_);

            // Текстура и цвет
            bgfx::setTexture(0, s_tex0_, L_tex);

            // u_color из ABGR в RGBA float
            const uint8_t r = (spr.color_abgr_ & 0x000000FF);
            const uint8_t g = (spr.color_abgr_ & 0x0000FF00) >> 8;
            const uint8_t b = (spr.color_abgr_ & 0x00FF0000) >> 16;
            const uint8_t a = (spr.color_abgr_ & 0xFF000000) >> 24;
            float L_col[4]{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
            if (bgfx::isValid(u_color_)) bgfx::setUniform(u_color_, L_col);

            const uint64_t L_state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                                     BGFX_STATE_BLEND_ALPHA | BGFX_STATE_MSAA;
            bgfx::setState(L_state);
            bgfx::submit(view2d_id_, L_prog);
          });
    }

    // Завершение кадра
    bgfx::frame();
  }

  // === Init ===
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Render manager initialized.";

    for (const auto& L_arg : args) {
      if (!L_arg.has_value()) continue;

      if (L_arg.type() == typeid(sf::RenderWindow*)) {
        Set_WindowRef(std::any_cast<sf::RenderWindow*>(L_arg));
      } else if (!entity_manager_ && L_arg.type() == typeid(Entity*)) {
        Set_EntityManager(std::any_cast<Entity*>(L_arg));
      }
    }

    if (!window_) {
      LOG::Fatal(__FILE__, __LINE__)
          << "Window pointer is null or not provided in Render manager";
    }
    if (!entity_manager_) {
      LOG::Fatal(__FILE__, __LINE__)
          << "Entity manager pointer is null or not provided in Render manager";
    }

    Init_Bgfx_();
    Init_Programs_();
    Init_2DQuad_();
    Init_Defaults_();
    Init_RenderEntity_();
  }

  // === Resize hook ===
  void On_WindowResized(uint32_t w, uint32_t h) {
    if (!is_bgfx_inited_) return;
    uint32_t L_flags = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;
    bgfx::reset(w, h, L_flags);
  }

 private:
 void FreeResources() override {
   if (bgfx::isValid(vbh_quad2d_)) bgfx::destroy(vbh_quad2d_);
   if (bgfx::isValid(ibh_quad2d_)) bgfx::destroy(ibh_quad2d_);
   if (bgfx::isValid(s_tex0_)) bgfx::destroy(s_tex0_);
   if (bgfx::isValid(u_color_)) bgfx::destroy(u_color_);
   if (bgfx::isValid(tex_white_)) bgfx::destroy(tex_white_);
   if (bgfx::isValid(prog_2d_)) bgfx::destroy(prog_2d_);
   if (bgfx::isValid(prog_3d_)) bgfx::destroy(prog_3d_);

   if (is_bgfx_inited_) {
     bgfx::shutdown();
     is_bgfx_inited_ = false;
   }
   window_ = nullptr;
   entity_manager_ = nullptr;
   LOG::Debug() << "Render manager resources freed.";
 }

  void Init_RenderEntity_() {
    LOG::Debug() << "Initializing Render Entity";
    // при желании — создать дефолтную камеру/тестовые спрайты здесь
  }

  // ===== Platform bind =====
  struct NativeHandle {
    void* ndt_{nullptr};
    void* nwh_{nullptr};
  };

  template <class T>
  static void* ToVoidHandle_(T h) {
    if constexpr (std::is_integral_v<T>)
      return reinterpret_cast<void*>(static_cast<uintptr_t>(h));
    else
      return reinterpret_cast<void*>(h);
  }

  NativeHandle Get_NativeHandle_FromSFML_(sf::RenderWindow* wnd) {
    NativeHandle L{};
    // SFML 3: WindowBase::getNativeHandle()
    auto L_wh = wnd->getNativeHandle();  // sf::WindowHandle (платформо-зависимый тип)
    L.nwh_ = ToVoidHandle_(L_wh);
    // L.ndt_ оставляем nullptr — большинству backend-ов этого достаточно.
    return L;
  }

  void Init_Bgfx_() {
    if (is_bgfx_inited_) return;

    bgfx::Init L_init;
    L_init.type = bgfx::RendererType::Count;  // auto
    L_init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;

    // Привязка к SFML-окну
    NativeHandle L_hwnd = Get_NativeHandle_FromSFML_(window_);
    bgfx::PlatformData L_pd{};
    L_pd.ndt = L_hwnd.ndt_;
    L_pd.nwh = L_hwnd.nwh_;
    L_pd.context = nullptr;
    L_pd.backBuffer = nullptr;
    L_pd.backBufferDS = nullptr;
    bgfx::setPlatformData(L_pd);

    const auto L_sz = window_->getSize();
    L_init.resolution.width = (uint32_t)L_sz.x;
    L_init.resolution.height = (uint32_t)L_sz.y;

    if (!bgfx::init(L_init)) {
      LOG::Fatal(__FILE__, __LINE__) << "bgfx initialization failed";
      return;
    }

    is_bgfx_inited_ = true;
    view3d_id_ = 0;
    view2d_id_ = 1;

    LOG::Info() << "bgfx initialized and bound to SFML window";
  }

  void Init_Programs_() {
    // Пути к предсобранным бинарникам шейдеров (.bin)
    prog_3d_ = EDD::Tools::Create_Program("Resources/shaders/vs_basic.bin",
                                          "Resources/shaders/fs_basic.bin");
    prog_2d_ = EDD::Tools::Create_Program("Resources/shaders/vs_2d.bin",
                                          "Resources/shaders/fs_2d.bin");
    if (!bgfx::isValid(prog_3d_))
      LOG::Fatal(__FILE__, __LINE__) << "Program 3D not loaded";
    if (!bgfx::isValid(prog_2d_))
      LOG::Fatal(__FILE__, __LINE__) << "Program 2D not loaded";

    if (!bgfx::isValid(s_tex0_)) {
      s_tex0_ = bgfx::createUniform("s_tex0", bgfx::UniformType::Sampler);
    }
    if (!bgfx::isValid(u_color_)) {
      u_color_ = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    }
  }

  void Init_2DQuad_() {
    // Единичный quad (0..1) — цвет в вершинах белый (модуляция цветом через u_color)
    Vertex2D L_verts[4]{
        {{0.0f, 0.0f, 0.0f}, 0xffffffff, {0.0f, 0.0f}},
        {{1.0f, 0.0f, 0.0f}, 0xffffffff, {1.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, 0xffffffff, {1.0f, 1.0f}},
        {{0.0f, 1.0f, 0.0f}, 0xffffffff, {0.0f, 1.0f}},
    };
    uint16_t L_indices[6]{0, 1, 2, 0, 2, 3};

    auto L_layout = Vertex2D::Get_Layout();
    const bgfx::Memory* L_vmem = bgfx::copy(L_verts, sizeof(L_verts));
    const bgfx::Memory* L_imem = bgfx::copy(L_indices, sizeof(L_indices));

    vbh_quad2d_ = bgfx::createVertexBuffer(L_vmem, L_layout);
    ibh_quad2d_ = bgfx::createIndexBuffer(L_imem);

    if (!bgfx::isValid(vbh_quad2d_) || !bgfx::isValid(ibh_quad2d_)) {
      LOG::Fatal(__FILE__, __LINE__) << "2D quad buffers create failed";
    }
  }

  void Init_Defaults_() {
    tex_white_ = EDD::Tools::Create_White_Texture();
    if (!bgfx::isValid(tex_white_)) {
      LOG::Fatal(__FILE__, __LINE__) << "white texture create failed";
    }
  }

  void Set_WindowRef(sf::RenderWindow* window = nullptr) {
    if (window)
      window_ = window;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Window pointer is null";
  }

  void Set_EntityManager(Entity* entity_manager = nullptr) {
    if (entity_manager)
      entity_manager_ = entity_manager;
    else
      LOG::Fatal(__FILE__, __LINE__) << "Entity manager pointer is null";
  }
};

}  // namespace EDD::Managers
