#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "EngineError/engine_logging.h"
#include "Managers/manager_inputs.h"
#include "Tools/delegate.h"
#include "Tools/interface.h"

namespace EDD::Tests {
using InterfaceKeyEvent = Tools::Interface<EDD::Tools::EventTypes::KeyEvent>;
class TestManagerInputs : public InterfaceKeyEvent {
  Managers::Inputs *input_manager_ = nullptr;
  bool is_do_once = false;  // запуск тестов только один раз

 public:
  void RunTests() {
    //....
    if (is_do_once == false) {
      is_do_once = true;
    }
  }

 private:
};
}  // namespace EDD::Tests