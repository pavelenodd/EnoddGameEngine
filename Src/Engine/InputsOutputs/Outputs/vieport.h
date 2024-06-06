#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

#include "../../engine_data.h"

namespace edd {

class Vieport {
 private:
 protected:
 public:
  /**
   * @brief Создает новый объект вьюпорта sf::Window
   * @param struct EngineSettings
   * @return sf::Window* указатель на созданое окно вьюпорта
   */
  virtual sf::Window* CreateViewport(const EngineSettings& L_engine_settings) {
    sf::Window* window =
        new sf::Window(sf::VideoMode(L_engine_settings.vieport_size.height,
                                     L_engine_settings.vieport_size.width),
                       L_engine_settings.vieport_name,
                       sf::Style::Default,  //
                       L_engine_settings.Get_ContextSettings());
    return window;
  }
};
}  // namespace edd
