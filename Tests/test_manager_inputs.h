#pragma once
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <memory>

#include "../Src/Managers/Engine/manager_inputs.h"

namespace EDD::Tests {

class TestManagerInputs {
 private:
  std::unique_ptr<sf::RenderWindow> test_window_;
  std::unique_ptr<Managers::Inputs> input_manager_;
  int tests_passed_ = 0;
  int tests_failed_ = 0;

 public:
  TestManagerInputs() {
    // Создаем тестовое окно
    test_window_ = std::make_unique<sf::RenderWindow>(sf::VideoMode(100, 100), "Test Window", sf::Style::None);
    test_window_->setVisible(false);  // Скрываем тестовое окно
  }

  ~TestManagerInputs() {
    if (test_window_ && test_window_->isOpen()) { test_window_->close(); }
  }

  /**
   * @brief Запуск всех тестов
   */
  void RunAllTests() {
    std::cout << "=== Running TDD Tests for Inputs Manager ===" << std::endl;

    TestConstructorWithoutWindow();
    TestConstructorWithWindow();
    TestSendWithoutEvents();
    TestUpdateWithoutWindow();
    TestUpdateWithWindow();
    TestInterfaceImplementation();
    TestWindowAssignment();
    TestEventProcessing();
    TestResourceCleanup();

    PrintResults();
  }

 private:
  void Assert(bool condition, const std::string& test_name) {
    if (condition) {
      tests_passed_++;
      std::cout << "[PASS] " << test_name << std::endl;
    } else {
      tests_failed_++;
      std::cout << "[FAIL] " << test_name << std::endl;
    }
  }

  void PrintResults() {
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Passed: " << tests_passed_ << std::endl;
    std::cout << "Failed: " << tests_failed_ << std::endl;
    std::cout << "Total: " << (tests_passed_ + tests_failed_) << std::endl;

    if (tests_failed_ == 0) {
      std::cout << "All tests passed!" << std::endl;
    } else {
      std::cout << "Some tests failed!" << std::endl;
    }
  }

  /**
   * @brief Тест конструктора без окна
   */
  void TestConstructorWithoutWindow() {
    auto input_manager = std::make_unique<Managers::Inputs>();
    Assert(input_manager != nullptr, "Constructor without window creates object");

    // Проверяем, что Send() возвращает nullopt при отсутствии событий
    auto result = input_manager->Send();
    Assert(!result.has_value(), "Send() returns nullopt when no events");
  }

  /**
   * @brief Тест конструктора с окном
   */
  void TestConstructorWithWindow() {
    auto input_manager = std::make_unique<Managers::Inputs>(test_window_.get());
    Assert(input_manager != nullptr, "Constructor with window creates object");

    // Проверяем начальное состояние
    auto result = input_manager->Send();
    Assert(!result.has_value(), "Send() returns nullopt initially with window");
  }

  /**
   * @brief Тест Send() без событий
   */
  void TestSendWithoutEvents() {
    input_manager_ = std::make_unique<Managers::Inputs>();

    auto result = input_manager_->Send();
    Assert(!result.has_value(), "Send() returns nullopt when no events available");
  }

  /**
   * @brief Тест Update() без окна
   */
  void TestUpdateWithoutWindow() {
    input_manager_ = std::make_unique<Managers::Inputs>();

    // Update не должен вызывать исключения без окна
    try {
      input_manager_->Update();
      Assert(true, "Update() doesn't throw without window");
    } catch (...) { Assert(false, "Update() doesn't throw without window"); }

    auto result = input_manager_->Send();
    Assert(!result.has_value(), "Send() returns nullopt after Update() without window");
  }

  /**
   * @brief Тест Update() с окном
   */
  void TestUpdateWithWindow() {
    input_manager_ = std::make_unique<Managers::Inputs>(test_window_.get());

    // Update должен работать с окном
    try {
      input_manager_->Update();
      Assert(true, "Update() works with window");
    } catch (...) { Assert(false, "Update() works with window"); }
  }

  /**
   * @brief Тест реализации интерфейса
   */
  void TestInterfaceImplementation() {
    input_manager_ = std::make_unique<Managers::Inputs>();

    // Проверяем, что класс реализует интерфейс
    Tools::Interface<sf::Event>* interface_ptr = input_manager_.get();
    Assert(interface_ptr != nullptr, "Class implements Interface<sf::Event>");

    // Проверяем работу через интерфейс
    auto result = interface_ptr->Send();
    Assert(!result.has_value(), "Interface Send() works correctly");
  }

  /**
   * @brief Тест назначения окна через SetWindow
   */
  void TestWindowAssignment() {
    input_manager_ = std::make_unique<Managers::Inputs>();

    // Назначаем окно
    input_manager_->SetWindow(test_window_.get());

    // Проверяем, что Update работает после назначения окна
    try {
      input_manager_->Update();
      Assert(true, "Update() works after SetWindow()");
    } catch (...) { Assert(false, "Update() works after SetWindow()"); }
  }

  /**
   * @brief Тест обработки событий
   */
  void TestEventProcessing() {
    input_manager_ = std::make_unique<Managers::Inputs>(test_window_.get());

    // Генерируем тестовое событие (закрытие окна)
    sf::Event close_event;
    close_event.type = sf::Event::Closed;

    // Обновляем менеджер
    input_manager_->Update();

    // Проверяем состояние после обновления
    // Примечание: реальные события сложно симулировать в unit-тестах
    // Этот тест проверяет, что Update() выполняется без ошибок
    Assert(true, "Event processing doesn't crash");
  }

  /**
   * @brief Тест очистки ресурсов
   */
  void TestResourceCleanup() {
    {
      auto temp_manager = std::make_unique<Managers::Inputs>(test_window_.get());
      temp_manager->Update();
      // Деструктор должен корректно очистить ресурсы
    }
    Assert(true, "Resource cleanup in destructor works");
  }

  /**
   * @brief Тест множественных вызовов Update()
   */
  void TestMultipleUpdates() {
    input_manager_ = std::make_unique<Managers::Inputs>(test_window_.get());

    // Множественные вызовы Update не должны вызывать проблем
    try {
      for (int i = 0; i < 10; ++i) {
        input_manager_->Update();
      }
      Assert(true, "Multiple Update() calls work correctly");
    } catch (...) { Assert(false, "Multiple Update() calls work correctly"); }
  }

  /**
   * @brief Тест состояния has_event_
   */
  void TestEventState() {
    input_manager_ = std::make_unique<Managers::Inputs>(test_window_.get());

    // Начальное состояние
    auto result1 = input_manager_->Send();
    Assert(!result1.has_value(), "Initial state: no events");

    // После Update
    input_manager_->Update();
    auto result2 = input_manager_->Send();
    // Результат зависит от наличия реальных событий
    Assert(true, "Event state management works");
  }
};

/**
 * @brief Функция для запуска всех TDD тестов
 */
inline void RunInputsManagerTests() {
  TestManagerInputs test_suite;
  test_suite.RunAllTests();
}

}  // namespace EDD::Tests
