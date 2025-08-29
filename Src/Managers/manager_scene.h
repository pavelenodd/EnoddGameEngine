#pragma once
// менеджер созданя сцены и управлении окнами
#include <cstdlib>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <any>
#include <initializer_list>
#include <tuple>

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
  EDD::Data::Viewport viewport_;  // данные о вьюпорте
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
   * @brief Init manager scene
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

  virtual void FreeResources() override {
    // Освобождение ресурсов
    LOG::Debug() << "Scene manager resources freed.";
  }

 private:
  bool InitViewport(std::tuple<std::string, int, int> view_params) {
    if (std::get<0>(view_params) == "") {
      LOG::Fatal() << "Viewport name cannot be empty.";
      return false;
    } else {
      viewport_.name = std::get<0>(view_params);
    }
    if (std::get<1>(view_params) == 0 || std::get<2>(view_params) == 0) {
      LOG::Fatal() << "Viewport width and height cannot be zero.";
      return false;
    } else {
      viewport_.w = std::get<1>(view_params);
      viewport_.h = std::get<2>(view_params);
    }

    LOG::Debug() << "Viewport name: " << viewport_.name << ", width: " << viewport_.w
                 << ", height: " << viewport_.h;

    return true;
  }
};
}  // namespace Managers

}  // namespace EDD