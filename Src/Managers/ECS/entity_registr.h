#pragma once
#include <functional>
#include <memory>
#include <unordered_map>

#include "component_storage.h"
#include "entity_base.h"

namespace EDD::ECS {

class EntityRegistr {
 private:
  std::unordered_map<entity_id_t, std::unique_ptr<EntityBase>> entities_;
  entity_id_t next_id_{1};
  ComponentStorage storage_;  ///< главный SparseSet-склад

 public:
  /* === Создание / удаление сущностей === */
  entity_id_t Create_Entity() {
    entity_id_t L_id = next_id_++;
    entities_.emplace(L_id, std::make_unique<EntityBase>(L_id));
    return L_id;
  }

  void Destroy_Entity(entity_id_t id) {
    /* Удаляем все компоненты, которые могли быть у сущности (опционально)
       — в примере опускаем; обычно системы сами проверяют Has() */
    entities_.erase(id);
  }

  /* === Доступ === */
  EntityBase* Get_Entity(entity_id_t id) {
    auto L_it = entities_.find(id);
    return (L_it != entities_.end()) ? L_it->second.get() : nullptr;
  }

  ComponentStorage& Components() { return storage_; }

  /* Массовые операции (по-прежнему возможны) */
  void For_Each(const std::function<void(EntityBase&)>& fn) {
    for (auto& [_, L_ent] : entities_) fn(*L_ent);
  }
};

}  // namespace EDD::ECS
