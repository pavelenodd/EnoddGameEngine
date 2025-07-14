// Managers/manager_physics.h

#pragma once

#include "manager_base.h"
namespace EDD::Managers {

/**
 * @brief Менеджер отвечающий за обработку физических взаимодействий в игре.
 *
 */
class Physics : public Managers::Base {
 private:
 public:
  Physics() {}
  ~Physics() {}

 public:
  virtual void Update() override {}

  virtual void Init(std::initializer_list<void*> args = {}) override {}
  virtual void FreeResources() override {}
};

}  // namespace EDD