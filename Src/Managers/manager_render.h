#pragma once
#include <any>
#include <cstdint>
#include <vector>

#include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"
#include "manager_base.h"
#include "manager_entity.h"

// Include GLFW and its native access for window handles
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>
#include <bgfx/platform.h>

#include <cstdio>

#include "bgfx/bgfx.h"

namespace Managers {
class Entity;
}

namespace EDD {
namespace Managers {

// Supported rendering types
enum class RenderType { RENDER_2D = 0, RENDER_3D = 1 };

class Render : public Base {
 private:
  std::vector<EDD::Data::Viewport*> viewports_ = {};
  EDD::Managers::Entity* entity_manager_ = nullptr;
  RenderType render_type_ = RenderType::RENDER_2D;

 public:
  Render() = default;
  ~Render() override = default;

  void Init(std::vector<std::any> args) override {
    if (args.size() < 3) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - insufficient parameters";
      return;
    }
    try {
      viewports_ = std::any_cast<std::vector<EDD::Data::Viewport*>>(args[0]);
      entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
      render_type_ = std::any_cast<RenderType>(args[2]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - invalid parameter types";
      return;
    }
    if (viewports_.empty()) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - no viewports provided";
      return;
    }
    if (!entity_manager_) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - entity manager is null";
      return;
    }
    if (!InitBGFX()) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - failed to initialize bgfx";
      return;
    }
    return;
  }

  void Update() override {
    // Очистка экрана
    ClearView(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    SetViewRect(0, 0, 0, viewports_[0]->w, viewports_[0]->h);
    Touch(0);

    // Рендеринг кадра
    Frame();

    // Обмен буферов
    glfwSwapBuffers(viewports_[0]->viewport_window);

    // Обработка событий GLFW
    glfwPollEvents();
  }

  void FreeResources() override {
    bgfx::shutdown();
    glfwDestroyWindow(viewports_[0]->viewport_window);
    glfwTerminate();
  }

 private:
  bool InitBGFX() {
    if (viewports_.empty()) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::InitBGFX - no viewports available";
      return false;
    }
    for (const auto& viewport : viewports_) {
      if (viewport && viewport->viewport_window) {
        bgfx::Init init;
        init.type = bgfx::RendererType::Noop;  // Используем Noop для тестирования
                                               // без рендеринга
        init.resolution.width = viewport->w;
        init.resolution.height = viewport->h;
        init.resolution.reset = BGFX_RESET_VSYNC;
        if (!bgfx::init(init)) {
          printf("bgfx::init failed\n");
          return false;
        }
      }
    }
    return true;
  }
  void ClearView(uint8_t viewId, uint16_t flags, uint32_t rgba, float depth, uint8_t stencil) {
    bgfx::setViewClear(viewId, flags, rgba, depth, stencil);
  }
  void SetViewRect(uint8_t viewId, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    bgfx::setViewRect(viewId, x, y, width, height);
  }
  void Touch(uint8_t viewId) {
    bgfx::touch(viewId);
  }
  void Frame() {
    bgfx::frame();
  }
};

}  // namespace Managers
}  // namespace EDD
