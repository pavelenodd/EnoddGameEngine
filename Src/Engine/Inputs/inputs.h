#pragma once
#include <map>
#include <string>

/**
 * \brief Виртуальный метод для обработки событий ввода
 */
class Inputs {
 private:
 protected:
  std::map<int, std::string> inputs_key_map_;

 public:
  Inputs() {
    //?
    inputs_key_map_.insert({GLFW_KEY_ESCAPE, "Exit"});
    inputs_key_map_.insert({GLFW_KEY_LEFT, "Left"});
    inputs_key_map_.insert({GLFW_KEY_RIGHT, "Right"});
    // DEBUG ^ Удалить
    // INFO  введено для тестирования
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
                              const int& key,
                              const int& scancode,
                              const int& action,
                              const int& mods) = 0;
};
