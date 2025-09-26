#pragma once
namespace EDD {

class GameLoop;
}
class SnakeGame {
 private:
  EDD::GameLoop &game_loop_;

 public:
  SnakeGame(EDD::GameLoop &game_loop);
  void Run();
};
