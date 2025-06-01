// Managers/manager_physics.h

#pragma once

#include "manager_base.h"
namespace EDD::Managers {

/**
 * @brief Менеджер отвечающий за обработку физики
 *
 */
class Physics : public Managers::Base {
 private:
 public:
  Physics() {}
  ~Physics() {}

 public:
  virtual void Update() override {}

  virtual void Init() override {}
  virtual void FreeResources() override {}
};

}  // namespace EDD