// Tests/test_manager_inputs.cpp
// Автоматические тесты менеджера ввода
//------------------------------------------------------------------------------
#include "test_manager_inputs.h"

#include <SFML/Window/Event.hpp>
#include <string>

#include "engine_logging.h"
namespace EDD {

void TestManagerInputs::TestInputManagerInitialization() {
  if (is_do_once) {
    return;
  }
  // Тест инициализации менеджера ввода
  TEST_LOG::Info() << "Testing Input Manager Initialization";
  if (input_manager_ == nullptr) {
    TEST_LOG::Failed(__FUNCTION__, __LINE__) << "Input Manager is not initialized";
  } else {
    TEST_LOG::Success() << "Input Manager is initialized";
  }
}

void TestManagerInputs::TestSceneManagerHandlesInputEvent() {
  TEST_LOG::Info() << "Testing Scene Manager Handles Input Event";
  if ((input_manager_->interface_args_.has_value()) &&
      (input_manager_->interface_args_->is<KeyPressedEvent>() ||
       input_manager_->interface_args_->is<KeyReleasedEvent>())) {
    if (EventsEqual(input_manager_->interface_args_.value(),
                    this->interface_args_.value())) {
      TEST_LOG::Success(EventToString(input_manager_->interface_args_.value()) +
                        " == " + EventToString(this->interface_args_.value()));

    } else {
      TEST_LOG::Failed(EventToString(input_manager_->interface_args_.value()) +
                       " != " + EventToString(this->interface_args_.value()));
    }
  }
}

}  // namespace EDD
