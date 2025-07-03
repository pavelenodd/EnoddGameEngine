// main.cpp
#include "game_loop.h"

// Опционально: включить тесты в debug режиме
#ifdef _DEBUG
#include "../Tests/test_manager_inputs.h"
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG
  // Запуск тестов в debug режиме
  if (argc > 1 && std::string(argv[1]) == "--test") {
    EDD::Tests::RunInputsManagerTests();
    return 0;
  }
#endif

  EDD::GameLoop game_loop;
  game_loop.StartLoop();

  return 0;
}
