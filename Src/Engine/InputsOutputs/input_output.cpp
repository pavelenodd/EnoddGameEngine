#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>

#include "../JsonParser/json_parser.cpp"
class Inputs {
 private:
  // JsonParser parser_("Data/engine_settings.json");

 public:
};
namespace OutputsNamSps {
struct WindowSize {
  int height;
  int width;
};

}  // namespace OutputsNamSps
class Outputs {
 private:
  OutputsNamSps::WindowSize window_size_;  // размер вьюпорта
  JsonParser parser_;
  const std::string window_name_ = "Vieport";
  sf::ContextSettings window_settings_;  // настройки рендера вьюпорта

  // Определение параметров контекста OpenGL
  void DefiningOpenGLContextParameters() {
    parser_.ParseFile("Data/engine_settings.json");
    window_settings_.depthBits = 24;
    window_settings_.stencilBits = 8;
    window_settings_.antialiasingLevel = 4;  // Уровень сглаживания
    window_settings_.majorVersion = 3;
    window_settings_.minorVersion = 3;
    window_size_.height =
        parser_.GetParameterValue<int>("ViewportSize", "Height");
    window_size_.width =
        parser_.GetParameterValue<int>("ViewportSize", "Width");
  }

 public:
  void CreateVidget() {
    DefiningOpenGLContextParameters();
    // Создание окна
    std::cerr << "Window : " << window_name_ << " open \n";
    sf::Window window(sf::VideoMode(window_size_.height, window_size_.width),
                      window_name_, sf::Style::Default, window_settings_);

    // Главный цикл программы
    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
      }

      // Очистка буфера кадра
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Здесь будет код рендеринга с использованием OpenGL...

      // Обновление окна
      window.display();
    }
  }
};