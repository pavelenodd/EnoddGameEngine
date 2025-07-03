#include <iostream>

#include "test_manager_inputs.h"

int main() {
  std::cout << "Starting EnoddGameEngine Tests..." << std::endl;

  try {
    // Запуск тестов менеджера ввода
    EDD::Tests::RunInputsManagerTests();

    std::cout << "\nAll test suites completed!" << std::endl;
    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Test execution failed: " << e.what() << std::endl;
    return 1;
  }
}
