#pragma once
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <string>

#include "EngineError/engine_logging.h"
#include "Managers/Engine/manager_inputs.h"
#include "Tools/Engine/delegate.h"
#include "Tools/Engine/interface.h"

namespace EDD {
using InterfaceSFEvent = Tools::Interface<sf::Event>;
class TestManagerInputs : public InterfaceSFEvent {
  Managers::Inputs *input_manager_ = nullptr;
  bool do_it_TestInputManagerInitialization = false;  // что бы не запускать лишний раз

 public:
  TestManagerInputs(Managers::Inputs *input_manager) : input_manager_(input_manager) {
    if (input_manager_ == nullptr) {
      TEST_LOG::Failed("Attempting to create TestManagerInputs with null input_manager");
    }
  }

  void RunTests() {
    TestInputManagerInitialization();
    TestSceneManagerHandlesInputEvent();
  }

 private:
  // тест инициализации менеджера ввода
  void TestInputManagerInitialization() {
    if (do_it_TestInputManagerInitialization) {
      return;
    }
    TEST_LOG::Info("Run Test: TestInputManagerInitialization...");

    // Проверка на null указатель
    if (input_manager_ == nullptr) {
      TEST_LOG::Failed("Input manager is null pointer.");
    }

    // Проверим адрес объекта
    TEST_LOG::Info("Input manager address: " +
                   std::to_string(reinterpret_cast<uintptr_t>(input_manager_)));

    TEST_LOG::Success("Input manager initialized correctly. \n");
    do_it_TestInputManagerInitialization = true;
  }
  // тест проверки что данные получаются от эвента а не от input_manager
  void TestSceneManagerHandlesInputEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesInputEvent...");
  }
  // тест проверки что данные получаются от делегата
  void TestSceneManagerHandlesDelegateEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesDelegateEvent...");
  }
  // тест обработки события нажатия клавиши
  void TestSceneManagerHandlesKeyPressEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesKeyPressEvent...");
  }
  // тест обработки события закрытия окна
  void TestSceneManagerHandlesCloseEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesCloseEvent...");
  }
  // тест нажатия группы клавиш на протяжении 5 минут
  void TestSceneManagerHandlesKeyPressEventLongDuration() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesKeyPressEventLongDuration...");
  }
  // тест обработки события движения мыши
  void TestSceneManagerHandlesMouseMoveEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesMouseMoveEvent...");
  }
  // тест обработки события изменения размера окна
  void TestSceneManagerHandlesWindowResizeEvent() {
    TEST_LOG::Info("Run Test: TestSceneManagerHandlesWindowResizeEvent...");
  }
};
}  // namespace EDD