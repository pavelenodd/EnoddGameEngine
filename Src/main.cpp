// main.cpp
#include <chrono>
#include <iostream>
#include <thread>
#include "game_loop.h"

int main(int, char**) {
  GameLoop game_loop;
  game_loop.StartLoop();

  return 0;
}
