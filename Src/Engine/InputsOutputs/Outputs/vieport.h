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
  GLFWwindow* CreateViewport(const edd::settings::Engine& L_engine_settings) {
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
  void TerminateVieport(GLFWwindow* L_vieport) {
    glfwDestroyWindow(L_vieport);
    glfwTerminate();
  }
  void SetContextSettings(const edd::settings::Engine& L_engine_settings) {
    // Версия OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, L_engine_settings.major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, L_engine_settings.minor_version);
    // тип ядра OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, L_engine_settings.core_profile);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    // Дебаг мод контекста OpenGL
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
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
