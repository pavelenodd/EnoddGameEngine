#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "component_pool.h"

namespace EDD::ECS {

class ComponentStorage {
 private:
  std::unordered_map<std::type_index, std::unique_ptr<void>>
      pools_;  // Пулы компонентов по типу

  /**
   * @brief Хранилище компонентов, реализованное как набор пулов
   *        для каждого типа компонента.
   * @return Указатель на пул компонента T.
   *         Если пула нет, он будет создан.
   */
  template <typename T>
  ComponentPool<T>* GetPool() {
    auto L_it = pools_.find(typeid(T));
    if (L_it == pools_.end()) {
      auto L_pool = std::make_unique<ComponentPool<T>>();
      L_it = pools_.emplace(typeid(T), std::move(L_pool)).first;
    }
    return static_cast<ComponentPool<T>*>(L_it->second.get());
  }

 public:
  /**
   * @brief Добавить новый компонент T для сущности с заданным id.
   * @param id Идентификатор сущности
   * @param args Аргументы для конструктора компонента T
   * @return Ссылка на созданный компонент T.
   */
  template <typename T, typename... Args>
  T& Add(entity_id_t id, Args&&... args) {
    return GetPool<T>()->Emplace(id, std::forward<Args>(args)...);
  }

  /**
   * @brief Получить компонент T по id сущности.
   * @param id Идентификатор сущности
   * @return Указатель на компонент T или nullptr, если нет.
   */
  template <typename T>
  T* Get(entity_id_t id) {
    return GetPool<T>()->Get(id);
  }

  /**
   * @brief Удалить компонент T у сущности с заданным id.
   * @param id Идентификатор сущности
   */
  template <typename T>
  void Remove(entity_id_t id) {
    GetPool<T>()->Remove(id);
  }

  template <typename T>
  bool Has(entity_id_t id) {
    return GetPool<T>()->Has(id);
  }

  /* Для систем: плотная SoA-колонка компонента T */
  template <typename T>
  ComponentPool<T>& View() {
    return *GetPool<T>();
  }
};

}  // namespace EDD::ECS
