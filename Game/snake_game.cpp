#include "snake_game.h"

#include "game_loop.h"
SnakeGame::SnakeGame(EDD::GameLoop &game_loop) : game_loop_(game_loop) {}
void SnakeGame::Run() {
  game_loop_.StartLoop();
}