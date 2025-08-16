#pragma once
#include <GLFW/glfw3.h>

#include <unordered_set>

#include "EngineError/engine_logging.h"
#include "Tools/event_type.h"
#include "Tools/interface.h"
#include "manager_base.h"

namespace EDD {

#ifdef DEBUG

namespace Tests {
struct InputsInspector;
}
#endif

namespace Managers {
class Scene;  // Предварительное объявление класса Scene
/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<KeyEvent>.
  */
using InterfaceKeyEvent = Tools::Interface<EDD::Tools::EventTypes::KeyEvent>;
class Inputs : public Base, public InterfaceKeyEvent {
 private:
  Scene* scene_manager_ = nullptr;  // указатель на менеджер сцены
#ifdef DEBUG

  friend struct ::EDD::Tests::InputsInspector;
#endif
  // Подписчики на события инпута
  std::unordered_set<InterfaceKeyEvent*> observers_;

 public:
  Inputs() {}
  ~Inputs() {
    FreeResources();
  }

  virtual void Update() override {}

  // Подписаться/отписаться на события
  void Subscribe(InterfaceKeyEvent* observers = nullptr) {
    if (observers != nullptr) {
      observers_.insert(observers);
    }
  }
  void Unsubscribe(InterfaceKeyEvent* observers = nullptr) {
    if (observers != nullptr) {
      observers_.erase(observers);
    }
  }

  virtual void Init(std::vector<std::any> args) override {
    // Инициализация обработчика ввода
    LOG::Debug() << "Input manager initialized.";
  }

 private:
  // Освобождение ресурсов
  virtual void FreeResources() override {}
};
}  // namespace Managers
}  // namespace EDD