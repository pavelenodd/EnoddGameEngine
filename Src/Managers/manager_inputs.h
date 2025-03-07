// Managers/manager_inputs.h

#pragma once
#include <SDL3/SDL_events.h>
#include <list>
#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за обработку ввода
 *
 */
class ManagerInputs : public ManagerBase {
 private:
  std::list<int> events_;

 public:
  ManagerInputs() {}
  ~ManagerInputs() {}

 public:
  /**
   * @brief Отслеживание нажатий клавиш
   *
   */
  virtual void Update() override {
    SDL_Event L_event;
    while (SDL_PollEvent(&L_event)) {
      events_.push_back(L_event.type);
    }
  }
  virtual void Init() override {}
  virtual void FreeResources() override {}

 private:
};