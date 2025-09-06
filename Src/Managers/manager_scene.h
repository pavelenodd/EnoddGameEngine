#pragma once
#include <any>
#include <string>
#include <tuple>
#include <vector>

// #include "../Tests/test_manager_scene.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include "EngineData/engine_data.h"
#include "EngineError/engine_logging.h"
#include "manager_base.h"

namespace EDD {

#if defined(DEBUG)
namespace Tests {
struct SceneInspector;  // forward declaration
}
#endif

namespace Managers {
class Scene : public Base {
  friend struct EDD::Tests::SceneInspector;

 private:
  std::vector<EDD::Data::Viewport*> viewports_;  // List of created viewports
  inline static bool glfw_initialized_ = false;  // Track if GLFW is initialized

 public:
  Scene() = default;
  ~Scene() override = default;

  /**
   * @brief Initialize the scene with the given parameters
   *
   * @param args A vector of any type containing initialization parameters
   */
  void Init(std::vector<std::any> args = {}) override {
    LOG::Debug() << "Scene::Init called";
    if (args.size() < 1) {
      LOG::Fatal(__FILE__, __LINE__) << "Scene::Init - insufficient parameters";
      return;
    }
    std::tuple<std::string, uint16_t, uint16_t> L_viewport_params;
    try {
      L_viewport_params = std::any_cast<std::tuple<const std::string, uint16_t, uint16_t>>(
          args[0]);
    } catch (const std::bad_any_cast&) {
      LOG::Fatal(__FILE__, __LINE__) << "Scene::Init - invalid viewport parameters";
      return;
    }

    std::string title = std::get<0>(L_viewport_params);
    int width = std::get<1>(L_viewport_params);
    int height = std::get<2>(L_viewport_params);

    auto viewport = CreateViewport(title, width, height);
    if (!viewport) {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to create initial viewport";
      return;
    }
    viewports_.push_back(viewport);
    LOG::Debug() << "Scene::Init completed successfully";
    return;
  }
  /**
   * @brief Not used in the scene manager
   *
   */
  void Update() override {}

  /**
   * @brief Free all resources used by the scene manager
   *
   */
  void FreeResources() override {
    DestroyAllViewport();
  }

  /**
   * @brief Destroy a viewport by its title
   *
   * @param title The title of the viewport to destroy
   */
  void DestroyViewport(const std::string& title) {
    for (auto& it : viewports_) {
      if (it->title == title) {
        delete it;
        viewports_.erase(std::remove(viewports_.begin(), viewports_.end(), it),
                         viewports_.end());
        break;
      }
    }
  }

  /**
   * @brief Destroy all created viewports
   *
   */
  void DestroyAllViewport() {
    for (auto& it : viewports_) {
      delete it;
    }
    viewports_.clear();
  }

  /**
   * @brief Get the Window Ref object
   *
   * @return EDD::Data::Viewport* or nullptr if no viewport exists
   */
  EDD::Data::Viewport* GetViewportRef(const std::string& title) const {
    for (const auto& it : viewports_) {
      if (it->title == title) return it;
    }
    return nullptr;
  }
  /**
   * @brief Get all created viewports
   *
   * @return std::vector<EDD::Data::Viewport*>
   */
  std::vector<EDD::Data::Viewport*> GetAllViewports() const {
    if (viewports_.empty()) {
      LOG::Warning(__FILE__, __LINE__) << "No viewports available";
      return {};
    }
    return viewports_;
  }

  /**
   * @brief Create a Viewport object
   *
   * @param title std::string
   * @param width uint16_t
   * @param height uint16_t
   * @return EDD::Data::Viewport*
   */
  EDD::Data::Viewport* CreateViewport(std::string& title, uint16_t width, uint16_t height) {
    if (!glfw_initialized_) {
      if (!glfwInit()) {
        LOG::Fatal(__FILE__, __LINE__) << "Failed to initialize GLFW";
        return nullptr;
      }
      glfw_initialized_ = true;
    }
    auto existing = GetViewportRef(title);
    if (existing) {
      title += " (copy)";
    }

    EDD::Data::Viewport* viewport = new EDD::Data::Viewport();
    if (!viewport) {
      LOG::Fatal(__FILE__, __LINE__) << "Failed to create viewport";
      return nullptr;
    }

    viewport->title = title;
    viewport->w = width;
    viewport->h = height;

    viewport->viewport_window = glfwCreateWindow(
        viewport->w, viewport->h, viewport->title.c_str(), NULL, NULL);
    if (!viewport->viewport_window) {
      glfwTerminate();
      return nullptr;
    }
    glfwMakeContextCurrent(viewport->viewport_window);
    return viewport;
  }
};

}  // namespace Managers
}  // namespace EDD
