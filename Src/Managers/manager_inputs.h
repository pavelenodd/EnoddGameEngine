#pragma once
#include <GLFW/glfw3.h>

#include <unordered_set>

#include "EngineData/event_type.h"
#include "Tools/interface.h"
#include "Managers/manager_base.h"

namespace EDD {

#ifdef DEBUG

namespace Tests {
struct InputsInspector;
}
#endif

namespace Managers {

/*
  Менеджер ввода отвечает за обработку событий ввода от пользователя.
  все события, которые он принимает передаётся в интерфейс Tools::Interface<KeyEvent>.
  */
using InterfaceKeyEvent = Tools::Interface<EDD::Tools::EventTypes::KeyEvent>;
class Inputs : public Base, public InterfaceKeyEvent {
 private:
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

  virtual void Update() override;

  /**
   * @brief Subscribe to key event notifications.
   *        If observers is nullptr, subscribe the current instance.
   *
   * @param observers Pointer to the observer to subscribe.
   */
  void Subscribe(InterfaceKeyEvent* observers = nullptr);

  /**
   * @brief Unsubscribe from key event notifications.
   *        If observers is nullptr, unsubscribe the current instance.
   *
   * @param observers Pointer to the observer to unsubscribe.
   */
  void Unsubscribe(InterfaceKeyEvent* observers = nullptr);

  virtual void Init(std::vector<std::any> args) override;

 private:
  // Освобождение ресурсов
  virtual void FreeResources() override {}
};
}  // namespace Managers
}  // namespace EDD