#include "manager_inputs.h"

#include "EngineError/engine_logging.h"
using namespace EDD::Managers;

void Inputs::Update() {
  for (auto* observer : observers_) {
    if (observer != nullptr) {
      auto& interface_args = static_cast<InterfaceKeyEvent*>(observer)->interface_args_;
      for (const auto& event : interface_args) {
        LOG::Debug() << "Notifying observer of key event: " << event.key
                     << " action: " << event.action;
      }
      interface_args.clear();  // Clear after notifying
    }
  }
}

void Inputs::Init(std::vector<std::any> args) {
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
