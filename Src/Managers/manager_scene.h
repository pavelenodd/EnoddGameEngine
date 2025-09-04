#pragma once
#include <any>
#include <string>
#include <tuple>
#include <vector>

// #include "../Tests/test_manager_scene.h"
#include "EngineError/engine_logging.h"
#include "manager_base.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "EngineData/engine_data.h"  // defines EDD::Data::Viewport

namespace EDD {
namespace Managers {

class Scene : public Base {
  // friend struct EDD::Tests::SceneInspector;

 private:
  EDD::Data::Viewport* view_data_ = nullptr;
  std::vector<EDD::Data::Viewport*> viewports_;
  inline static bool glfw_initialized_ = false;

  // Create a new viewport (window) with given title and dimensions
  EDD::Data::Viewport* CreateViewport(const std::string& title, int width, int height) {
    if (!glfw_initialized_) {
      if (!glfwInit()) {
        LOG::Fatal(__FILE__, __LINE__) << "GLFW initialization failed";
        return nullptr;
      }
      glfw_initialized_ = true;
    }
    // Ensure no OpenGL context is created, as rendering will be handled by bgfx
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to create GLFW window";
      return nullptr;
    }
    // Allocate and populate viewport data
    EDD::Data::Viewport* vp = new EDD::Data::Viewport();
    vp->viewport_window = window;
    vp->name = title;
    vp->w = width;
    vp->h = height;
    // If this is the first viewport, mark it as the main viewport
    if (viewports_.empty()) {
      view_data_ = vp;
    }
    viewports_.push_back(vp);
    return vp;
  }

 public:
  Scene() = default;
  ~Scene() override = default;

  void Init(std::vector<std::any> args = {}) override {
    // Initialize the scene by creating the main viewport (window)
    if (!args.empty()) {
      for (auto& arg : args) {
        if (arg.type() == typeid(std::tuple<std::string, int, int>)) {
          auto cfg = std::any_cast<std::tuple<std::string, int, int>>(arg);
          std::string title = std::get<0>(cfg);
          int width = std::get<1>(cfg);
          int height = std::get<2>(cfg);
          EDD::Data::Viewport* vp = CreateViewport(title, width, height);
          if (!vp) {
            // return false;  // Window creation failed
          }
          // Only create the first viewport from params (ignore additional if provided)
          break;
        }
      }
    } else {
      // No parameters provided, create a default main viewport
      EDD::Data::Viewport* vp = CreateViewport("MainViewport", 800, 600);
      if (!vp) {
        // return false;
      }
    }
    // return true;
  }

  void Update() override {
    // Poll OS events for the window every frame
    glfwPollEvents();
    // Optionally handle window close event
    if (view_data_ && glfwWindowShouldClose(view_data_->viewport_window)) {
      // Main window is requested to close (could signal GameLoop to stop here if needed)
      LOG::Info(__FILE__) << "Main window close requested";
    }
  }

  void FreeResources() override {
    // Destroy all remaining viewports and terminate GLFW
    DestroyAllViewport();
    if (glfw_initialized_) {
      glfwTerminate();
      glfw_initialized_ = false;
    }
  }

  // Destroy the most recently created viewport (if any)
  void DestroyViewport() {
    if (!viewports_.empty()) {
      EDD::Data::Viewport* vp = viewports_.back();
      glfwDestroyWindow(vp->viewport_window);
      delete vp;
      viewports_.pop_back();
      // Update main viewport pointer if needed
      if (viewports_.empty()) {
        view_data_ = nullptr;
      } else {
        // Keep the first created viewport as main
        view_data_ = viewports_.front();
      }
    }
  }

  // Destroy all created viewports
  void DestroyAllViewport() {
    for (EDD::Data::Viewport* vp : viewports_) {
      if (vp->viewport_window) {
        glfwDestroyWindow(vp->viewport_window);
      }
      delete vp;
    }
    viewports_.clear();
    view_data_ = nullptr;
  }

  // Get the GLFW window handle of the main viewport
  GLFWwindow* GetWindowRef() const {
    return view_data_ ? view_data_->viewport_window : nullptr;
  }
};

}  // namespace Managers
}  // namespace EDD
