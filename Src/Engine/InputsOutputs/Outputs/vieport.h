#include "../../engine_data.h"

namespace edd {

class Vieport {
 private:
 protected:
  /**
   * @brief Создает новый объект вьюпорта GLFWwindow*
   * @param struct EngineSettings
   * @return GLFWwindow* указатель на созданое окно вьюпорта
   */
  virtual GLFWwindow* CreateViewport(
      const edd::settings::Engine& L_engine_settings) {
    glfwInit();
    GLFWwindow* window =
        glfwCreateWindow(L_engine_settings.vieport_size.height,
                         L_engine_settings.vieport_size.width,
                         L_engine_settings.vieport_name, nullptr, nullptr);
    if (!window) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
    }
    return window;
  }

  /**
   * @brief Устанавливает активное окно вьюпорта
   * @param GLFWwindow* Ссылка на вьюпорт который надо сделать активным
   */
  void SetVieportActive(GLFWwindow* L_window) {
    glfwMakeContextCurrent(L_window);
  }
};
}  // namespace edd
