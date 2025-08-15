#pragma once

#include <bgfx/bgfx.h>

#include <SFML/Graphics.hpp>

#include "EngineError/engine_logging.h"
#include "Managers/manager_entity.h"

namespace EDD::Managers {
class Render2D {
 private:
  // Пойнтеры оставляем, но пока не используем для привязки окна
  sf::RenderWindow* window_{nullptr};
  Managers::Entity* entity_manager_{nullptr};

  // Локальное состояние bgfx
  // TODO: перенести в структуру или в EngineData
  bool is_bgfx_inited_{false};
  uint16_t view_id_{1};
  uint32_t reset_flags_{BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4};
  uint32_t width_{1280};  // дефолтный размер бэкбуфера без привязки к окну
  uint32_t height_{720};

 public:
  Render2D() {}
  ~Render2D() {
    Shutdown();
  }

  // === Инициализация bgfx без привязки к окну ===
  void Init() {
    if (is_bgfx_inited_) return;

    bgfx::Init L_init;
    L_init.type = bgfx::RendererType::Count;  // авто выбор рендера
    L_init.vendorId = BGFX_PCI_ID_NONE;       // авто
    L_init.resolution.width = width_;
    L_init.resolution.height = height_;
    L_init.resolution.reset = reset_flags_;

    if (!bgfx::init(L_init)) {
      LOG::Fatal(__FILE__, __LINE__) << "Render2D: bgfx init failed (no window bind)";
      return;
    }
    is_bgfx_inited_ = true;
  }

  // Мягкое выключение bgfx
  void Shutdown() {
    if (is_bgfx_inited_) {
      bgfx::shutdown();
      is_bgfx_inited_ = false;
    }
  }

  // Ресайз бэкбуфера без привязки к окну
  void On_WindowResized(uint32_t w, uint32_t h) {
    width_ = w;
    height_ = h;
    if (is_bgfx_inited_) bgfx::reset(width_, height_, reset_flags_);
  }

  // Настроить view id (по умолчанию 1)
  void Set_ViewId(uint16_t id = 1) {
    view_id_ = id;
  }

  // Обновить флаги reset и применить, если уже инициализировано
  void Set_ResetFlags(uint32_t flags) {
    reset_flags_ = flags;
    if (is_bgfx_inited_) bgfx::reset(width_, height_, reset_flags_);
  }
  // Установить менеджер ресурсов
  void Set_EntityManager(Managers::Entity* entity_manager) {
    entity_manager_ = entity_manager;
  }
  // Установить ссылку на окно SFML
  void Set_WindowRef(sf::RenderWindow* window) {
    window_ = window;
    BindWindow();
  }

  // Рисуем все 2D — пока пусто (логика пойдёт позже)
  void Render_All(Managers::Entity& entity_manager) {
    (void)entity_manager;
    if (!is_bgfx_inited_) return;
    // Здесь позже появится настройка view/proj и сабмиты
  }

 private:
  // Привязка рендера к окну SFML
  void BindWindow() {
    if (!window_) {
      LOG::Fatal(__FILE__, __LINE__) << "Render2D: window pointer is null";
      return;
    }
  }
};
}  // namespace EDD::Managers
#pragma once
//
// Компоненты для 3D рендера (bgfx)

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace EDD::Managers {
class Render2D {};

}  // namespace EDD::Managers
