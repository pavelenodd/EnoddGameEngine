#pragma once

#include <GLFW/glfw3.h>

// Подключаем нативные функции только под нужную платформу
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <any>

#include "EngineData/engine_data.h"
#include "Items/Components/comp_render2D.h"
#include "Items/Components/comp_render3D.h"
#include "engine_logging.h"
#include "manager_base.h"
#include "manager_entity.h"
#include "manager_resource.h"

namespace EDD::Managers {
enum RenderType { NONE, RENDER_2D, RENDER_3D };
/*
  Менеджер рендера отвечает за отрисовку игровых объектов на экране.
      Рендерит в предоставленном окне менеджером сцен объекты
      >принимает объекты от менеджера сущностей (Entity)
      >принимает ресурсы от менеджера ресурсов(Resource)
      >передает объединённые данные в активное окно для отрисовки
      (3D + 2D) на bgfx, окно — GLFW
*/
class Render : public Managers::Base {
 private:
  RenderType render_type_ = RenderType::NONE;
  EDD::Data::Viewport* viewport_ = nullptr;  // ссылка на данные вьюпорта
  Render2D* render_2d_ = nullptr;
  Render3D* render_3d_ = nullptr;
  Managers::Entity* entity_manager_ = nullptr;

 public:
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "Render manager initialized.";
    if (args.size() < 3) {
      LOG::Fatal(__func__, __LINE__) << "Render: not enough arguments";
      abort();
    }

    // [0] Viewport*
    try {
      if (std::any_cast<EDD::Data::Viewport*>(args[0])->viewport_window != nullptr) {
        viewport_ = std::any_cast<EDD::Data::Viewport*>(args[0]);
      } else {
        LOG::Fatal(__func__, __LINE__) << "Viewport window is null";
        abort();
      }
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__func__, __LINE__) << "First argument must be EDD::Data::Viewport*";
      abort();
    }

    // [1] Entity*
    try {
      entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__func__, __LINE__) << "Second argument must be Managers::Entity*";
      abort();
    }

    // [2] RenderType
    try {
      render_type_ = std::any_cast<RenderType>(args[2]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__func__, __LINE__) << "Third argument must be RenderType";
      abort();
    }

    if (render_type_ == RenderType::NONE) {
      LOG::Fatal(__func__, __LINE__) << "Render type NONE is invalid here";
      abort();
    }

    InitBgfx();
    InitRenderer();
  }

  void Update() override {
    if (!viewport_ || !entity_manager_) return;

    switch (render_type_) {
      case RENDER_2D:
        render_2d_->Render_All(*entity_manager_);
        break;
      case RENDER_3D:
        // render_3d_->Render_All(*entity_manager_);
        break;
      default:
        break;
    }

    bgfx::frame();
  }

  void FreeResources() override {
    if (render_type_ == RENDER_2D && render_2d_) {
      delete render_2d_;
      render_2d_ = nullptr;
    } else if (render_type_ == RENDER_3D && render_3d_) {
      delete render_3d_;
      render_3d_ = nullptr;
    }
    bgfx::shutdown();
    LOG::Debug(__func__) << "Render manager resources freed.";
  }

 private:
  void InitBgfx() {
    bgfx::Init init;
    bgfx::PlatformData pd{};

#if defined(_WIN32)
    pd.nwh = glfwGetWin32Window(viewport_->viewport_window);
#elif defined(__linux__)
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(viewport_->viewport_window);
    init.type = bgfx::RendererType::Vulkan;
#elif defined(__APPLE__)
    pd.nwh = glfwGetCocoaWindow(viewport_->viewport_window);
#endif

    init.platformData = pd;
    init.resolution.width = viewport_->w;
    init.resolution.height = viewport_->h;
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
      LOG::Fatal(__func__, __LINE__) << "Failed to init bgfx";
      abort();
    }
  }

  void InitRenderer() {
    switch (render_type_) {
      case RENDER_2D:
        render_2d_ = new Render2D();
        render_2d_->Set_EntityManager(entity_manager_);
        render_2d_->Init();
        break;
      case RENDER_3D:
        // render_3d_ = new Render3D();
        // render_3d_->Init();
        break;
      default:
        break;
    }
  }
};

}  // namespace EDD::Managers
