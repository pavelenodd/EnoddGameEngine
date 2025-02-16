// Managers/manager_scene.h

#pragma once
#include <SFML/Window.hpp>
#include <string>

#include "manager_base.h"
namespace EDD {
namespace ManagerSceneNS {
struct ViewportInfo {
 public:
  std::string name;
  sf::VideoMode mode;
  sf::ContextSettings settings;
  void SetMode(unsigned int width, unsigned int height, unsigned int depth) {
    mode.width = width;
    mode.height = height;
    mode.bitsPerPixel = depth;
  }
  void SetName(const std::string& name) { this->name = name; }
};

}  // namespace ManagerSceneNS

/**
 * @brief Менеджер отвечающий за создание и обновление сцены
 *
 */
class ManagerScene : public ManagerBase {
 private:
  ManagerSceneNS::ViewportInfo viewport_info_;
  sf::Window* window_;

 public:
  ManagerScene() {}
  ~ManagerScene() { FreeResources(); }

 public:
  /**
   * @brief Обновление Вьюпорта
   *
   */
  virtual void Update() override {}
  /**
   * @brief Инициализация менеджера
   *
   */

  virtual void Init() override {
    // Инициализация переменных ......
    viewport_info_.SetMode(800, 600, 32);
    viewport_info_.SetName("Test");
    //

    CreateScene();
  }

  virtual void FreeResources() override { delete window_; }

  sf::Window* GetWindowRef() { return window_; }

 private:
  /**
   * @brief Create a Viewport object
   *
   */
  sf::Window* CreateViewport() {
    return new sf::Window(viewport_info_.mode, viewport_info_.name);
  }
  /**
   * @brief Создание и настройка сцены
   */
  void CreateScene() {
    window_ = CreateViewport();
    window_->setActive(true);
  }
};
}  // namespace EDD