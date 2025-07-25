#pragma once

#include <sstream>
#include <string>

#include "EngineError/engine_logging.h"
#include "Managers/Engine/manager_inputs.h"
// #include "Tools/Engine/delegate.h"
#include "Tools/Engine/interface.h"

namespace EDD {
using KeyPressedEvent = sf::Event::KeyPressed;
using KeyReleasedEvent = sf::Event::KeyReleased;
static std::string EventToString(const sf::Event &ev) {
  std::ostringstream ss;
  if (ev.is<KeyPressedEvent>()) {
    auto kp = ev.getIf<KeyPressedEvent>();
    auto kr = ev.getIf<KeyReleasedEvent>();
    if (kp) {
      ss << "KeyPressed(code=" << static_cast<int>(kp->code) << ")";
    } else if (ev.is<sf::Event::Closed>()) {
      ss << "Closed";
    }
  } else if (ev.is<KeyReleasedEvent>()) {
    auto kr = ev.getIf<KeyReleasedEvent>();
    ss << "KeyReleased(code=" << static_cast<int>(kr->code) << ")";
  } else if (ev.is<sf::Event::Closed>()) {
    ss << "Closed";
  }
  return ss.str();
}
static bool EventsEqual(const sf::Event &a, const sf::Event &b) {
  if (a.is<KeyPressedEvent>() && b.is<KeyPressedEvent>()) {
    return (a.getIf<KeyPressedEvent>()->code == b.getIf<KeyPressedEvent>()->code);
  } else if (a.is<KeyReleasedEvent>() && b.is<KeyReleasedEvent>()) {
    return (a.getIf<KeyReleasedEvent>()->code == b.getIf<KeyReleasedEvent>()->code);
  } else {
    return false;
  }
}
using InterfaceSFEvent = Tools::Interface<sf::Event>;
class TestManagerInputs : public InterfaceSFEvent {
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