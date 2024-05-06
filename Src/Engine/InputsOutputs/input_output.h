#include <SFML/OpenGL.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/Window.hpp>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../JsonParser/json_parser.h"
#include "../engine_data.h"

class Inputs {
 private:
  InputSettings input_settings_;
  std::map<std::string, std::string> keyboard_inputs;  // инпуты клавиатуры
  std::map<std::string, std::string> mouse_inputs;  // инпуты мыши

 public:
  InputSettings* Get_InputSettings() { return &input_settings_; }

  Inputs() {
    JsonParser parser("Data/input.json");
    keyboard_inputs = parser.Get_ParameterValuesPairs("Keyboard_inputs");
  }
  ~Inputs() {}
};

class Outputs {
 private:
  EngineSettings engine_settings_;  // настройки рендера вьюпорта

  void PrintToLogWindowSettongs(const EngineSettings& L_engine_settings) {
    // ^вывод консоьной информации о рендере и настройках вьюпорта

    // зона подключения модификаторов вывода
    // вывод true или false
    std::cerr << std::boolalpha;
    //
    std::cerr << "\t"
              << "\033[4m"
              << "depth bits"
              << "\033[0m"
              << "\t\t" << L_engine_settings.depth_bits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "stencil bits"
              << "\033[0m"
              << "\t\t" << L_engine_settings.stencil_bits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "antialiasing level"
              << "\033[0m"
              << "\t" << L_engine_settings.antialiasing_level << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "vertical sync"
              << "\033[0m"
              << "\t\t" << L_engine_settings.is_vertical_sync_enabled << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "OpenGL version"
              << "\033[0m"
              << "\t\t" << L_engine_settings.major_version << "."
              << L_engine_settings.minor_version << "\n";
    // зона отключения всех модификаторов
    std::cerr << std::noboolalpha;

    //
    std::cout << std::endl;
  }

 public:
  sf::Window* CreateViewport(const EngineSettings& L_engine_settings) {
    PrintToLogWindowSettongs(L_engine_settings);

    sf::Window* window =
        new sf::Window(sf::VideoMode(L_engine_settings.vieport_size.height,
                                     L_engine_settings.vieport_size.width),
                       L_engine_settings.vieport_name,
                       sf::Style::Default,  //
                       L_engine_settings.Get_ContextSettings());
    return window;
  }
};