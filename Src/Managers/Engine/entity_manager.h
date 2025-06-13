// Managers/manager_physics.h

#pragma once

#include "ECS/component_pool.h"
#include "ECS/entity_base.h"
#include "ECS/entity_registr.h"
//
#include "manager_base.h"

namespace EDD::Managers {

/**
 * @brief Менеджер отвечающий за обработку физики
 *
 */
class Entity : public Managers::Base {
 private:
  EDD::ECS::EntityRegistr entity_registr_;        ///< Регистратор сущностей
  EDD::ECS::ComponentStorage component_storage_;  ///< Хранилище компонентов
 public:
  Entity() {}
  ~Entity() {}

 public:
  virtual void Update() override {}

  virtual void Init() override {}
  virtual void FreeResources() override {}
};

}  // namespace EDD::Managers