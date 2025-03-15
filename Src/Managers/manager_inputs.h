// Managers/manager_inputs.h

#pragma once
#include <SDL3/SDL_events.h>
#include <list>
#include "manager_base.h"
/**
 * @brief Менеджер отвечающий за обработку ввода
 *
 */
namespace EDD::Managers {

class Inputs : public Managers::Base, public Tools::Interface<SDL_Event> {
 private:
  std::list<SDL_Event> events_;

 public:
  Inputs() {}
  ~Inputs() {}

 public:
  /**
   * @brief Отслеживание нажатий клавиш
   *
   */
  virtual void Update() override {
    SDL_Event L_event;
    while (SDL_PollEvent(&L_event)) {
      events_.push_front(L_event);
    }
  }
  // Реализация метода интерфейса
  virtual std::optional<SDL_Event> Send() const override {
    if (!events_.empty()) {
      return events_.front();
    }
    return std::nullopt;
  }

    virtual void Init() override {}
  virtual void FreeResources() override {}

 private:
};

}  // namespace EDD
