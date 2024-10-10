#pragma once
#include <string>
struct WindowSettings {
  unsigned int width;        //< Ширина окна
  unsigned int height;       //< Высота окна
  std::string window_name_;  //< Название окна
};

/**
 * \brief Общий объект окна
 */
class Window {
 private:
 protected:
  WindowSettings window_settings_;  //< Настройки окна

 public:
 private:
 protected:
 public:
  /**
   * \brief Cоздание нового активного окна
   */
  virtual void Create(const WindowSettings& L_winow_settings) = 0;
  /**
   * \brief Обновление окна
   */
  virtual void Update() = 0;
};
