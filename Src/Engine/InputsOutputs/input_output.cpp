#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>

#include "../JsonParser/json_parser.cpp"
#include "../engine_data.h"
class Inputs {
 private:
  // JsonParser parser_("Data/engine_settings.json");

 public:
};

class Outputs {
 private:
  EngineSettings engine_settings_;  // настройки рендера вьюпорта

  void PrintToLogWindowSettongs(sf::ContextSettings L_window_settings,
                                bool is_vertical_sync_enabled_) {
    // ^вывод консоьной информации о рендере и настройках вьюпорта

    // зона подключения модификаторов вывода
    // вывод true или false
    std::cerr << std::boolalpha;
    //
    std::cerr << "\t"
              << "\033[4m"
              << "depth bits"
              << "\033[0m"
              << "\t\t" << engine_settings_.depth_bits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "stencil bits"
              << "\033[0m"
              << "\t\t" << engine_settings_.stencil_bits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "antialiasing level"
              << "\033[0m"
              << "\t" << engine_settings_.antialiasing_level << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "vertical sync"
              << "\033[0m"
              << "\t\t" << engine_settings_.is_vertical_sync_enabled << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "OpenGL version"
              << "\033[0m"
              << "\t\t" << engine_settings_.major_version << "."
              << engine_settings_.minor_version << "\n";
    // зона отключения всех модификаторов
    std::cerr << std::noboolalpha;

    //
    std::cout << std::endl;
  }

 public:
  Outputs() {}
  ~Outputs() {}
  sf::Window* CreateViewport(const EngineSettings& L_engine_settings) {
    sf::ContextSettings L_const_settings;
    L_const_settings.antialiasingLevel = L_engine_settings.antialiasing_level;
    L_const_settings.attributeFlags = L_engine_settings.attribute_flags;
    L_const_settings.depthBits = L_engine_settings.depth_bits;
    L_const_settings.stencilBits = L_const_settings.stencilBits;

    L_const_settings.majorVersion = L_engine_settings.major_version;
    L_const_settings.minorVersion = L_engine_settings.minor_version;

    L_const_settings.sRgbCapable = L_engine_settings.sRGB_capable;

    sf::Window* window = new sf::Window(
        sf::VideoMode(L_engine_settings.vieport_size.height,
                      L_engine_settings.vieport_size.width),
        L_engine_settings.vieport_name, sf::Style::Default, L_const_settings);
    return window;
  }
};