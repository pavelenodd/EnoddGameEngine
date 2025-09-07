#include "manager_inputs.h"

#include "EngineError/engine_logging.h"
using namespace EDD::Managers;

void Inputs::Update() {
  for (auto* observer : observers_) {
    if (observer != nullptr) {
      auto& interface_args = static_cast<InterfaceKeyEvent*>(observer)->interface_args_;
      if (interface_args.has_value()) {
        LOG::Debug() << "Key event set: " << interface_args->key;
        interface_args.reset();
      }
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
