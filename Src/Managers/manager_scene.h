#pragma once
// менеджер созданя сцены и управлении окнами
#include <algorithm>
#include <cstdlib>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <any>
#include <initializer_list>
#include <tuple>
#include <vector>

#include "../EngineData/engine_data.h"
#include "../EngineError/engine_logging.h"
#include "../Tools/interface.h"
#include "manager_base.h"
#include "manager_inputs.h"

// Forward-declare инспектора для тестов
namespace EDD {

#ifdef DEBUG
namespace Tests {
struct SceneInspector;
}
#endif

namespace Managers {

/*
  Менеджер сцены отвечает за создание и управление сценами(вьюпортами)
  в игре обеспечивает корректность, шэринг ресурсов.
      принимает настройки вьюпорта (размеры окна, имя вьюпорта, .......)
      принимает интерфейс для обработки событий
      принимает делегат для срочного выполнения
  */

using InterfaceKeyEvent = Tools::Interface<EDD::Tools::EventTypes::KeyEvent>;

class Scene : public Managers::Base, public InterfaceKeyEvent {
 private:
  EDD::Data::Viewport* view_data_ = nullptr;  // данные о вьюпорте

#ifdef DEBUG
  friend struct ::EDD::Tests::SceneInspector;
#endif
 public:
  Scene() {}
  ~Scene() {
    FreeResources();
  }
  virtual void Update() override {}
  /**
   * @brief Init manager scene and create viewport
   *
   * @param args
   *         -args[0] - имя вьюпорта, ширина, высота,
   *         -args[1] - ,
   *         -args[2] -
   */
  virtual void Init(std::vector<std::any> args = {
                        std::tuple<std::string, int, int>("", 0, 0)}) override {
    // Инициализация менеджера сцены
    LOG::Debug() << "Scene manager initialized.";
    if (args.size() < 1) {
      LOG::Fatal() << "Not enough arguments for Scene manager initialization.";
      abort();
    }
    const auto& vp = std::any_cast<std::tuple<std::string, int, int>>(&args[0]);
    if (!vp) {
      LOG::Fatal() << "Init expects args[0] to be std::tuple<std::string,int,int>.";
      abort();
    }
    if (!InitViewport(*vp)) {
      LOG::Fatal() << "Failed to initialize viewport.";
      abort();
    }
  }

  // Освобождение ресурсов
  virtual void FreeResources() override {
    DestroyViewport();
    delete view_data_;
    view_data_ = nullptr;
    LOG::Debug() << "Scene manager resources freed.";
  }
  // Удаление вьюпорта
  void DestroyViewport() {
    if (view_data_) {
      if (view_data_->viewport_window) {
        glfwDestroyWindow(view_data_->viewport_window);
        view_data_->viewport_window = nullptr;
      }
      glfwTerminate();
    }
  }

 private:
  /**
   * @brief Initialize viewport settings
   *
   * @param view_params tuple of (name, width, height)
   * @return true if success, false otherwise
   */
  bool InitViewport(std::tuple<std::string, int, int> view_params) {
    if (std::get<0>(view_params) == "") {
      LOG::Fatal() << "Viewport name cannot be empty.";
      return false;
    } else {
      view_data_ = new EDD::Data::Viewport();
      view_data_->name = std::get<0>(view_params);
    }
    if (std::get<1>(view_params) == 0 || std::get<2>(view_params) == 0) {
      LOG::Fatal() << "Viewport width and height cannot be zero.";
      return false;
    } else {
      view_data_->w = std::get<1>(view_params);
      view_data_->h = std::get<2>(view_params);
    }

    LOG::Debug() << "Viewport name: " << view_data_->name << ", width: " << view_data_->w
                 << ", height: " << view_data_->h;

    return true;
  }
  /**
   * @brief Create a Viewport object using view_data or current settings
   *
   * @return true if success, false otherwise
   */
  EDD::Data::Viewport* CreateViewport(std::string title = "", int width = 0, int height = 0) {
    if (!view_data_) {
      LOG::Fatal() << "Viewport is not initialized.";
      return view_data_;
    }

    if (!glfwInit()) {
      LOG::Fatal() << "Failed to initialize GLFW.";
      return nullptr;
    }

    // Важно: запрещаем GLFW создавать GL-контекст
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!title.empty()) {
      view_data_->name = title;
      if (width > 0 && height > 0) {
        view_data_->viewport_window = glfwCreateWindow(
            width, height, title.c_str(), nullptr, nullptr);
      }
    } else {
      view_data_->viewport_window = glfwCreateWindow(
          view_data_->w, view_data_->h, view_data_->name.c_str(), nullptr, nullptr);
    }

    if (!view_data_->viewport_window) {
      LOG::Fatal() << "Failed to create GLFW window.";
      glfwTerminate();
      return nullptr;
    }

    LOG::Debug() << "Viewport created.";
    return view_data_;
  }
};
}  // namespace Managers

}  // namespace EDD