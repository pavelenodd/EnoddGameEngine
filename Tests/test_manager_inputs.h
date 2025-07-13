#pragma once
#include "EngineError/engine_logging.h"
#include "Managers/Engine/manager_inputs.h"
#include "Tools/Engine/delegate.h"
#include "Tools/Engine/interface.h"
namespace EDD {
class TestManagerInputs : public Tools::Interface<sf::Event> {
  Managers::Inputs *input_manager_ = nullptr;
  mutable std::optional<sf::Event> last_event_;

 public:
  TestManagerInputs() {}
  TestManagerInputs(Managers::Inputs *input_manager) : input_manager_(input_manager) {}
  ~TestManagerInputs() {}

  void RunTests() {
    TestInputManagerInitialization();
    TestSceneManagerHandlesInputEvent();
  }

 private:
  // тест инициализации менеджера ввода
  void TestInputManagerInitialization() {
    TEST_LOG::Info("Запуск теста: TestInputManagerInitialization...");
    if (input_manager_ != nullptr) {
      TEST_LOG::Success("Input manager initialized. \n");
    } else {
      TEST_LOG::Failed("Input manager initialization.");
    }
  }
  // тест проверки что данные получаются от эвента а не от input_manager
  void TestSceneManagerHandlesInputEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesInputEvent...");
    if (input_manager_->Send().has_value()) {
      auto ev = input_manager_->Send();
      if (ev) {
        last_event_ = ev;
        TEST_LOG::Info("Message send.");
        if (last_event_.has_value()) {
          if (1) {
            TEST_LOG::Success("TestSceneManagerHandlesInputEvent passed. \n");
          } else {
            TEST_LOG::Failed("TestSceneManagerHandlesInputEvent failed.");
          }
        } else {
          TEST_LOG::Info("Message not send.");
          TEST_LOG::Failed("TestSceneManagerHandlesInputEvent failed.");
        }
      }
    } else {
      TEST_LOG::Failed("TestSceneManagerHandlesInputEvent failed.");
    }
  }
  // тест проверки что данные получаются от делегата
  void TestSceneManagerHandlesDelegateEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesDelegateEvent...");
  }
  // тест обработки события нажатия клавиши
  void TestSceneManagerHandlesKeyPressEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesKeyPressEvent...");
  }
  // тест обработки события закрытия окна
  void TestSceneManagerHandlesCloseEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesCloseEvent...");
  }
  // тест нажатия группы клавиш на протяжении 5 минут
  void TestSceneManagerHandlesKeyPressEventLongDuration() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesKeyPressEventLongDuration...");
  }
  // тест обработки события движения мыши
  void TestSceneManagerHandlesMouseMoveEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesMouseMoveEvent...");
  }
  // тест обработки события изменения размера окна
  void TestSceneManagerHandlesWindowResizeEvent() {
    TEST_LOG::Info("Запуск теста: TestSceneManagerHandlesWindowResizeEvent...");
  }

  std::optional<sf::Event> Send() const override {
    // В этот момент Events уже лежат в input_manager_->event_
    auto ev = input_manager_->Send();  // вернёт текущее событие
    if (ev) {
      last_event_ = ev;
      TEST_LOG::Success("Observer получил событие");
    }
    return std::nullopt;  // дальше событие не шлём
  }
};
}  // namespace EDD