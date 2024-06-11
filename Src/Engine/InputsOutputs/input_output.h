#include <GLFW/glfw3.h>

#include <SFML/OpenGL.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/Window.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../JsonParser/json_parser.h"
#include "../Render/render.h"
#include "../engine_data.h"
#include "Inputs/keyboard_inputs.h"
#include "Inputs/mouse_inputs.h"
#include "Outputs/vieport.h"
namespace edd {
/**
 * @brief Класс для работы с вводом из переферии
 */
class Inputs : protected edd::KeyboardInputs, protected edd::MouseInputs {
 private:
  edd::settings::InputSettings input_settings_;

 public:
  edd::settings::InputSettings* Get_InputSettings() { return &input_settings_; }

  Inputs() {
    JsonParser parser("Data/input.json");

  }
  ~Inputs() {}
};
}  // namespace edd

namespace edd {

/**
 * @brief Класс для работы с вьюпортом, звуком, редером
 */
class Outputs : private edd::Vieport, protected edd::Render {
 private:
  edd::settings::Engine engine_settings_;  // настройки рендера вьюпорта

  /**
   * @brief вывод консольной информации о рендере и настройках вьюпорта
   * @param struct EngineSettings
   */
  void PrintToLogWindowSettongs(
      const edd::settings::Engine& L_engine_settings) {
    // зона подключения модификаторов вывода
    // вывод true или false
    std::cerr << std::boolalpha;
    //
    std::cerr << "\t" << "\033[4m" << "depth bits" << "\033[0m" << "\t\t"
              << L_engine_settings.depth_bits << "\n";
    std::cerr << "\t" << "\033[4m" << "stencil bits" << "\033[0m" << "\t\t"
              << L_engine_settings.stencil_bits << "\n";
    std::cerr << "\t" << "\033[4m" << "antialiasing level" << "\033[0m" << "\t"
              << L_engine_settings.antialiasing_level << "\n";
    std::cerr << "\t" << "\033[4m" << "vertical sync" << "\033[0m" << "\t\t"
              << L_engine_settings.is_vertical_sync_enabled << "\n";
    std::cerr << "\t" << "\033[4m" << "OpenGL version" << "\033[0m" << "\t\t"
              << L_engine_settings.major_version << "."
              << L_engine_settings.minor_version << "\n";
    // зона отключения всех модификаторов
    std::cerr << std::noboolalpha;

    //
    std::cout << std::endl;
  }

 public:
  /**
   * @brief Создает новый объект вьюпорта GLFWwindow*
   * @param struct EngineSettings
   * @return GLFWwindow* указатель на созданое окно вьюпорта
   */
  GLFWwindow* CreateViewport(const edd::settings::Engine& L_engine_settings) {
    PrintToLogWindowSettongs(L_engine_settings);
    return Vieport::CreateViewport(L_engine_settings);
  }
  void SetVieportContextSettings(
      const edd::settings::Engine& L_engine_settings) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, L_engine_settings.major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, L_engine_settings.minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, L_engine_settings.TYPE_PROFILE);
  }
  /**
   * @brief Рендер вьюпорта
   * @param GLFWwindow* вьюпорт для отрисовки
   * @param std::vector<GameObject*> объекты для отрисовки
   */
  void RenderVieport(GLFWwindow* L_window, std::vector<GameObject*> objects) {
    // L_window->setActive(true);  // установка текущего вьюпорта как
    // основного
    edd::Render::DrawObjects(objects);
  }
};
}  // namespace edd