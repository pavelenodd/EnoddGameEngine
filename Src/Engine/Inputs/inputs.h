#pragma once
#include <map>
#include <string>
#include "../GameEntity/EngineEntity/engine_entity.h"
/**
 * \brief Виртуальный метод для обработки событий ввода
 */
class Inputs {
 private:
 protected:
  std::map<int, std::string> inputs_key_map_;

 public:
  Inputs() {
    inputs_key_map_.insert({GLFW_KEY_ESCAPE, "Exit"});
    // DEBUG ^ Удалить
  };
  ~Inputs() {}

  /**
   * \brief Обработка событий ввода
   * \param key
   * \param scancode
   * \param action
   * \param mods
   */
  virtual void HandlingInputs(GLFWwindow* L_window,
                              int key,
                              int scancode,
                              int action,
                              int mods) = 0;
};
