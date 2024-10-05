#pragma once
/**
 * \brief Общий объект окна
 */
class Window {
 public:
  /**
   * \brief Cоздание нового активного окна
   */
  virtual void Create() = 0;
  /**
   * \brief Обновление окна
   */
  virtual void Update() = 0;
};
