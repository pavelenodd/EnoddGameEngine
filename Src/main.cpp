// main.cpp
#include "../Game/snake_game.h"  //тестовая игра для проверки функциональности
#include "game_loop.h"

#ifdef _DEBUG
#include "../Tests/test_manager_inputs.h"
#endif

int main(int argc, char** argv) {
#ifdef _DEBUG

#endif
  EDD::GameLoop game_loop;
  SnakeGame test_game(game_loop);

  return 0;
}
