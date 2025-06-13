#pragma once
#include "ecs_types.h"

namespace EDD::ECS {

class EntityBase {
 private:
  entity_id_t id_{INVALID_ENTITY_ID};

 public:
  explicit EntityBase(entity_id_t id) : id_{id} {}
  ~EntityBase() = default;

  entity_id_t Get_Id() const { return id_; }
};

}  // namespace EDD::ECS
