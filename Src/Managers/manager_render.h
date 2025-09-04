#pragma once
#include <any>
#include <cstdint>
#include <vector>

#include "EngineError/engine_logging.h"
#include "manager_base.h"
#include "manager_entity.h"
// Forward declare the Entity manager to avoid heavy includes
namespace Managers {
class Entity;
}

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

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

namespace EDD {
namespace Managers {

// Supported rendering types
enum class RenderType { RENDER_2D = 0, RENDER_3D = 1 };

class Render : public Base {
 private:
  GLFWwindow* window_handle_ = nullptr;  // Window handle from Scene
  EDD::Managers::Entity*
      entity_manager_ = nullptr;  // Entity manager (for future use in rendering)
  RenderType render_type_ = RenderType::RENDER_2D;
  uint32_t window_width_ = 0;
  uint32_t window_height_ = 0;

 public:
  Render() = default;
  ~Render() override = default;

  void Init(std::vector<std::any> args = {}) override {
    // Expect parameters: [GLFWwindow*, Entity*, RenderType]
    if (args.size() < 3) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - insufficient parameters";
      // return false;
    }
    try {
      window_handle_ = std::any_cast<GLFWwindow*>(args[0]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - invalid window handle parameter";
      // return false;
    }
    try {
      entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - invalid entity manager parameter";
      // return false;
    }
    try {
      render_type_ = std::any_cast<RenderType>(args[2]);
    } catch (const std::bad_any_cast&) {
      // If stored as an int, convert to RenderType
      try {
        render_type_ = static_cast<RenderType>(std::any_cast<int>(args[2]));
      } catch (...) {
        LOG::Fatal(__FILE__, __LINE__) << "Render::Init - invalid render type parameter";
        // return false;
      }
    }

    if (!window_handle_) {
      LOG::Fatal(__FILE__, __LINE__) << "Render::Init - window handle is null";
      // return false;
    }

    // Determine initial window dimensions
    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window_handle_, &fbWidth, &fbHeight);
    if (fbWidth <= 0 || fbHeight <= 0) {
      // Fallback to window size or default if framebuffer size is zero (e.g., not yet shown)
      int winWidth = 0, winHeight = 0;
      glfwGetWindowSize(window_handle_, &winWidth, &winHeight);
      fbWidth = (winWidth > 0 ? winWidth : 800);
      fbHeight = (winHeight > 0 ? winHeight : 600);
    }
    window_width_ = static_cast<uint32_t>(fbWidth);
    window_height_ = static_cast<uint32_t>(fbHeight);

    // Setup native platform data for bgfx (for Vulkan rendering)
    bgfx::PlatformData pd;
    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    // On Linux, decide between X11 or Wayland at runtime (GLFW 3.4+)
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
      pd.ndt = glfwGetWaylandDisplay();
      pd.nwh = glfwGetWaylandWindow(window_handle_);
    } else {
      pd.ndt = glfwGetX11Display();
      pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window_handle_);
    }
#else
    // If GLFW platform query not available, assume X11 by default
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window_handle_);
#endif
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = glfwGetCocoaWindow(window_handle_);
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = glfwGetWin32Window(window_handle_);
#else
    pd.ndt = NULL;
    pd.nwh = nullptr;
#endif

    // Initialize bgfx with Vulkan renderer
    bgfx::setPlatformData(pd);
    bgfx::Init init_cfg{};
    init_cfg.type = bgfx::RendererType::Count;
    init_cfg.resolution.width = window_width_;
    init_cfg.resolution.height = window_height_;
    init_cfg.resolution.reset = BGFX_RESET_VSYNC;
    init_cfg.platformData = pd;
    if (!bgfx::init(init_cfg)) {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to initialize bgfx (Vulkan)";
      // return false;
    }

    // Configure default view (id 0) clear color and viewport
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, window_width_, window_height_);

    // return true;
  }

  void Update() override {
    if (!window_handle_) {
      return;
    }
    // Check if window size has changed (e.g., resized)
    int newFbWidth = 0, newFbHeight = 0;
    glfwGetFramebufferSize(window_handle_, &newFbWidth, &newFbHeight);
    if (newFbWidth > 0 && newFbHeight > 0 &&
        (newFbWidth != static_cast<int>(window_width_) ||
         newFbHeight != static_cast<int>(window_height_))) {
      // Update bgfx with new resolution
      window_width_ = static_cast<uint32_t>(newFbWidth);
      window_height_ = static_cast<uint32_t>(newFbHeight);
      bgfx::reset(window_width_, window_height_, BGFX_RESET_VSYNC);
      bgfx::setViewRect(0, 0, 0, window_width_, window_height_);
    }

    // If no other rendering commands submitted, touch the view to trigger clear
    bgfx::touch(0);
    // Advance to the next frame
    bgfx::frame();
  }

  void FreeResources() override {
    // Shut down bgfx and free rendering resources
    bgfx::shutdown();
  }
};

}  // namespace Managers
}  // namespace EDD
