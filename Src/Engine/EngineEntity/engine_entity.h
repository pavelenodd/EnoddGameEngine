#pragma once

/**
 * \brief Общий класс для сущностей, подписывающихся на события
 */
class ObserverObject {
 private:
  /* data */
 public:
  ObserverObject(/* args */) {}
  ~ObserverObject() {}

  // Реакция на события
  // Обновление сущности
  virtual void UpdateEvent() = 0;
  // Обновление рендера сущности если нужно
  virtual void UpdateRender() = 0;
};
