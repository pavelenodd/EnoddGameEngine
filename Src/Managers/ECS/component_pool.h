#pragma once
#include <cstdint>
#include <limits>
#include <vector>

#include "ecs_types.h"

namespace EDD::ECS {

template <typename T>
class ComponentPool {
 private:
  static constexpr entity_id_t INVALID =
      std::numeric_limits<entity_id_t>::max();  // Невалидный id для sparse

  std::vector<T> data_;              // SoA-колонка компонентов
  std::vector<entity_id_t> dense_;   // dense[index] → id
  std::vector<std::size_t> sparse_;  // sparse[id]  → index (или INVALID)

 public:
  explicit ComponentPool(std::size_t maxEntities = 1024)
      : sparse_(maxEntities, INVALID) {}

  /**
   * @brief Проверить, есть ли компонент у сущности
   * @param id Идентификатор сущности
   * @return true, если есть компонент, иначе false
   */
  [[nodiscard]] bool Has(entity_id_t id) const {
    return id < sparse_.size() && sparse_[id] != INVALID;
  }

  /**
   * @brief Получить компонент по id
   * @param id Идентификатор сущности
   * @return Указатель на компонент или nullptr, если нет
   */
  T* Get(entity_id_t id) {
    if (Has(id)) {
      return &data_[sparse_[id]];
    } else {
      return nullptr;
    }
  }

  /**
   * @brief Создать новый компонент для сущности с заданным id
   * @param id Идентификатор сущности
   * @param args Аргументы для конструктора компонента
   * @return Ссылка на созданный компонент
   */
  template <typename... Args>
  T& Emplace(entity_id_t id, Args&&... args) {
    if (id >= sparse_.size()) {
      sparse_.resize(id + 1, INVALID);
    }

    std::size_t L_index = data_.size();
    data_.emplace_back(std::forward<Args>(args)...);
    dense_.push_back(id);
    sparse_[id] = L_index;
    return data_.back();
  }

  /**
   * @brief Удалить компонент сущности по id
   * @param id Идентификатор сущности
   * @return true, если компонент был удалён, иначе false
   */
  bool Remove(entity_id_t id) {
    if (!Has(id)) {
      return false;  // нет компонента
    }
    std::size_t L_index = sparse_[id];
    std::size_t L_lastIdx = data_.size() - 1;
    entity_id_t L_lastId = dense_[L_lastIdx];

    // swap-pop
    data_[L_index] = std::move(data_[L_lastIdx]);
    dense_[L_index] = L_lastId;
    sparse_[L_lastId] = L_index;

    data_.pop_back();
    dense_.pop_back();
    sparse_[id] = INVALID;

    return true;
  }

  /* === итераторы для систем === */
  /**
   * @brief Итератор для обхода компонентов
   * @return Итератор на начало данных
   */
  auto begin() { return data_.begin(); }

  /**
   * @brief Итератор для обхода компонентов
   * @return Итератор на конец данных
   */
  auto end() { return data_.end(); }
  /**
   * @brief Получить плотную колонку компонентов
   * @return Ссылка на вектор плотных индексов
   */
  const std::vector<entity_id_t>& Dense() const { return dense_; }
};

}  // namespace EDD::ECS
