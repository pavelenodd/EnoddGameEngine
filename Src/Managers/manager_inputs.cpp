#include "manager_inputs.h"

#include "EngineError/engine_logging.h"
using namespace EDD::Managers;

void Inputs::Update() {}

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
