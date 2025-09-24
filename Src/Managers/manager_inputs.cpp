#include "manager_inputs.h"

#include "EngineError/engine_logging.h"
using namespace EDD::Managers;

void Inputs::Update() {
  // <- [WARNING] потенциальная утечка памяти при некорректном удалении observer, добавить
  // проверку валидности
  for (auto* L_observer : observers_) {
    if (L_observer != nullptr) {
      auto& L_interface_args = static_cast<InterfaceKeyEvent*>(L_observer)->interface_args_;
      for (const auto& L_event : L_interface_args) {
        LOG::Debug() << "Notifying observer of key event: " << L_event.key
                     << " action: " << L_event.action;
      }
      L_interface_args.clear();  // Clear after notifying
    }
  }
}

void Inputs::Init(std::vector<std::any> args) {
  (void)args;  // подавление предупреждения о неиспользуемом параметре
  LOG::Debug() << "Input manager initialized.";
}

void Inputs::Subscribe(InterfaceKeyEvent* observers) {
  if (observers != nullptr) {
    observers_.insert(observers);
  }
}
void Inputs::Unsubscribe(InterfaceKeyEvent* observers) {
  if (observers != nullptr) {
    observers_.erase(observers);
  }
}
