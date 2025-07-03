// Engine/entity_manager.h

#pragma once

#include <algorithm>
#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "../../EngineData/Entity/coord.h"
#include "manager_base.h"

namespace EDD::Managers {

/**
  Менеджер сущностей (Entity Manager) 
     Отвечает за создание, удаление, поиск и управление компонентами сущностей.
 */
class Entity : public Base {
 private:
  entt::registry registry_;  ///< Основной реестр EnTT

  // Опционально: кэш для быстрого поиска сущностей по именам
  std::unordered_map<std::string, entt::entity> named_entities_;

 public:
  Entity() { Init(); }
  ~Entity() { FreeResources(); }

  // Удаление конструкторов копирования и перемещения
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;
  Entity(Entity&&) = delete;
  Entity& operator=(Entity&&) = delete;

  // ================================================================
  // Основные методы менеджера
  // ================================================================

  virtual void Update() override {}
  virtual void Init() override {
    // Инициализация, если нужна
    registry_.clear();
    named_entities_.clear();
  }

  virtual void FreeResources() override {
    registry_.clear();
    named_entities_.clear();
  }

  // ================================================================
  // Методы для работы с сущностями
  // ================================================================

  /**
   * @brief Создать новую сущность
   * @return Идентификатор созданной сущности
   */
  entt::entity CreateEntity() { return registry_.create(); }

  /**
   * @brief Создать именованную сущность
   * @param name Имя сущности
   * @return Идентификатор созданной сущности
   */
  entt::entity CreateEntity(const std::string& name) {
    auto entity = registry_.create();
    named_entities_[name] = entity;
    return entity;
  }

  /**
   * @brief Уничтожить сущность
   * @param entity Идентификатор сущности
   */
  void DestroyEntity(entt::entity entity) {
    // Удалить из кэша имен, если есть
    auto it = std::find_if(
        named_entities_.begin(), named_entities_.end(),
        [entity](const auto& pair) { return pair.second == entity; });
    if (it != named_entities_.end()) {
      named_entities_.erase(it);
    }

    registry_.destroy(entity);
  }

  /**
   * @brief Найти сущность по имени
   * @param name Имя сущности
   * @return Идентификатор сущности или entt::null если не найдена
   */
  entt::entity GetEntityByName(const std::string& name) const {
    auto it = named_entities_.find(name);
    return (it != named_entities_.end()) ? it->second : entt::null;
  }

  /**
   * @brief Проверить, существует ли сущность
   * @param entity Идентификатор сущности
   * @return true если существует
   */
  bool IsValid(entt::entity entity) const { return registry_.valid(entity); }

  // ================================================================
  // Методы для работы с компонентами
  // ================================================================

  /**
   * @brief Добавить компонент к сущности
   * @tparam T Тип компонента
   * @tparam Args Типы аргументов конструктора
   * @param entity Идентификатор сущности
   * @param args Аргументы для конструктора компонента
   * @return Ссылка на добавленный компонент
   */
  template <typename T, typename... Args>
  T& AddComponent(entt::entity entity, Args&&... args) {
    return registry_.emplace<T>(entity, std::forward<Args>(args)...);
  }

  /**
   * @brief Получить компонент сущности
   * @tparam T Тип компонента
   * @param entity Идентификатор сущности
   * @return Ссылка на компонент
   */
  template <typename T>
  T& GetComponent(entt::entity entity) {
    return registry_.get<T>(entity);
  }

  /**
   * @brief Получить компонент сущности (константная версия)
   * @tparam T Тип компонента
   * @param entity Идентификатор сущности
   * @return Константная ссылка на компонент
   */
  template <typename T>
  const T& GetComponent(entt::entity entity) const {
    return registry_.get<T>(entity);
  }

  /**
   * @brief Проверить, есть ли компонент у сущности
   * @tparam T Тип компонента
   * @param entity Идентификатор сущности
   * @return true если компонент есть
   */
  template <typename T>
  bool HasComponent(entt::entity entity) const {
    return registry_.all_of<T>(entity);
  }

  /**
   * @brief Удалить компонент у сущности
   * @tparam T Тип компонента
   * @param entity Идентификатор сущности
   */
  template <typename T>
  void RemoveComponent(entt::entity entity) {
    registry_.remove<T>(entity);
  }

  /**
   * @brief Проверить, есть ли все указанные компоненты у сущности
   * @tparam Components Типы компонентов
   * @param entity Идентификатор сущности
   * @return true если все компоненты есть
   */
  template <typename... Components>
  bool HasAllComponents(entt::entity entity) const {
    return registry_.all_of<Components...>(entity);
  }

  /**
   * @brief Проверить, есть ли хотя бы один из указанных компонентов
   * @tparam Components Типы компонентов
   * @param entity Идентификатор сущности
   * @return true если есть хотя бы один компонент
   */
  template <typename... Components>
  bool HasAnyComponent(entt::entity entity) const {
    return registry_.any_of<Components...>(entity);
  }

  // ================================================================
  // Методы для систем (представления/views)
  // ================================================================

  /**
   * @brief Получить представление сущностей с определенными компонентами
   * @tparam Components Типы компонентов
   * @return Представление для итерации
   */
  template <typename... Components>
  auto GetView() {
    return registry_.view<Components...>();
  }

  /**
   * @brief Получить представление сущностей с определенными компонентами
   * (константная версия)
   * @tparam Components Типы компонентов
   * @return Константное представление для итерации
   */
  template <typename... Components>
  auto GetView() const {
    return registry_.view<Components...>();
  }

  /**
   * @brief Получить группу сущностей (более оптимизированная версия view)
   * @tparam Owned Компоненты, которыми владеет группа
   * @tparam Get Дополнительные компоненты для получения
   * @tparam Exclude Исключаемые компоненты
   * @return Группа для итерации
   */
  template <typename... Owned, typename... Get, typename... Exclude>
  auto GetGroup() {
    return registry_.group<Owned...>(entt::get<Get...>,
                                     entt::exclude<Exclude...>);
  }

  /**
   * @brief Выполнить функцию для каждой сущности с определенными компонентами
   * @tparam Components Типы компонентов
   * @tparam Func Тип функции
   * @param func Функция для выполнения
   */
  template <typename... Components, typename Func>
  void ForEach(Func&& func) {
    auto view = registry_.view<Components...>();
    view.each(std::forward<Func>(func));
  }
  // ! TEST
  /**
   * @brief Создать именованную сущность с компонентом координат
   * @param name Имя сущности
   * @param x X координата
   * @param y Y координата
   * @param z Z координата (по умолчанию 0)
   * @return Идентификатор созданной сущности
   */
  entt::entity CreateEntityWithCoord(const std::string& name, float x, float y, float z = 0) {
    auto entity = CreateEntity(name);
    AddComponent<Coord>(entity, x, y, z);
    return entity;
  }

  /**
   * @brief Получить все сущности с компонентом координат для рендеринга
   * @return Представление сущностей с координатами
   */
  auto GetRenderableEntities() {
    return GetView<Coord>();
  }

  /**
   * @brief Выполнить функцию для каждой сущности с координатами
   * @tparam Func Тип функции
   * @param func Функция для выполнения (принимает entt::entity и Coord&)
   */
  template <typename Func>
  void ForEachCoordEntity(Func&& func) {
    ForEach<Coord>(std::forward<Func>(func));
  }

  // ================================================================
  // Доступ к реестру (для продвинутого использования)
  // ================================================================

  /**
   * @brief Получить прямой доступ к реестру EnTT
   * @return Ссылка на реестр
   */
  entt::registry& GetRegistry() { return registry_; }

  /**
   * @brief Получить прямой доступ к реестру EnTT (константная версия)
   * @return Константная ссылка на реестр
   */
  const entt::registry& GetRegistry() const { return registry_; }

  // ================================================================
  // Утилиты
  // ================================================================

  /**
   * @brief Получить количество активных сущностей
   * @return Количество сущностей
   */
  std::size_t GetEntityCount() const {
    // Подсчет через итерацию всех сущностей
    std::size_t count = 0;
    for (auto entity : registry_.view<entt::entity>()) {
      ++count;
    }
    return count;
  }

  /**
   * @brief Очистить все сущности и компоненты
   */
  void Clear() {
    registry_.clear();
    named_entities_.clear();
  }
};

}  // namespace EDD::Managers