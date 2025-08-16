#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "EngineError/engine_logging.h"
#include "Managers/manager_inputs.h"
#include "Tools/delegate.h"
#include "Tools/interface.h"

namespace EDD {
using InterfaceKeyEvent = Tools::Interface<EDD::Tools::EventTypes::KeyEvent>;
class TestManagerInputs : public InterfaceKeyEvent {
  Managers::Inputs *input_manager_ = nullptr;
  bool is_do_once = false;  // запуск тестов только один раз

 public:
  TestManagerInputs(Managers::Inputs *input_manager) : input_manager_(input_manager) {
    if (input_manager_ == nullptr) {
      TEST_LOG::Failed("Attempting to create TestManagerInputs with null input_manager");
    }
  }

  void RunTests() {
    TestInputManagerInitialization();
    TestSceneManagerHandlesInputEvent();
    if (is_do_once == false) {
      is_do_once = true;
    }
  }

 private:
  // тест инициализации менеджера ввода
  void TestInputManagerInitialization();
  // тест проверки что данные получаются от эвента а не от input_manager
  void TestSceneManagerHandlesInputEvent();
  // тест проверки что данные получаются от делегата
  void TestSceneManagerHandlesDelegateEvent();
  // тест обработки события нажатия клавиши
  void TestSceneManagerHandlesKeyPressEvent();
  // тест обработки события закрытия окна
  void TestSceneManagerHandlesCloseEvent();
  // тест нажатия группы клавиш на протяжении 5 минут
  void TestSceneManagerHandlesKeyPressEventLongDuration();
  // тест обработки события движения мыши
  void TestSceneManagerHandlesMouseMoveEvent();
  // тест обработки события изменения размера окна
  void TestSceneManagerHandlesWindowResizeEvent();
};
}  // namespace EDD