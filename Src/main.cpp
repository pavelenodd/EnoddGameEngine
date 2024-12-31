#include <chrono>
#include <iostream>
#include <thread>
#include "game_loop.h"

int main(int, char**) {
  GameLoop game_loop;
  game_loop.StartLoop();
  std::cout << "Game loop started" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  game_loop.StopLoop();
  std::cout << "Game loop stopped" << std::endl;
  return 0;
}
