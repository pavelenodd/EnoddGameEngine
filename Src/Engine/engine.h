#pragma once
#include <vector>
#include "GameEntity/game_entity.h"
#include "GameLoop/game_loop.h"
#include "Inputs/inputs.h"
#include "Render/render.h"

class Engine {
 private:
  GameLoop* game_loop_;
  Render* render_;
  std::vector<GameEntity*> game_entities_;
  Inputs* inputs_;

 public:
 public:
  ~Engine();

  /**
   * \brief Инициализация движка (подключение субклассов)
   */
  void Init();
  /**
   * \brief Запуск движка
   */
  void Start();
  /**
   * \brief Остановка движка
   */
  void Stop();
};

Engine::~Engine() {
  delete game_loop_;
  delete render_;
}

void Engine::Init() {
  game_loop_ = new GameLoop();
  render_ = new Render();

  // *DEBUG удалить потом
  for (int i = i = 0; i < 10; i++) {
    GameEntity* L_GameEntity = new GameEntity();
    game_entities_.push_back(L_GameEntity);
  }
}
void Engine::Start() {
  game_loop_->Start();
  game_loop_->Loop(game_entities_, render_);
}

void Engine::Stop() {
  game_loop_->Stop();
}