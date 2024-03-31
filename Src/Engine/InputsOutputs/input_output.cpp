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
  std::string window_name_;
  sf::ContextSettings window_settings_;  // настройки рендера вьюпорта
  bool is_vertical_sync_enabled_;

  void DefiningOpenGLContextParameters() {
    // Определение параметров контекста OpenGL
    parser_.ParseFile("Data/output.json");

    window_size_.height = parser_.GetParameterValue<int>("ViewportSettings",
                                                         "ViewportSize",  //
                                                         "Height");
    window_size_.width = parser_.GetParameterValue<int>("ViewportSettings",
                                                        "ViewportSize",  //
                                                        "Width");
    window_settings_.depthBits =
        parser_.GetParameterValue<int>("ViewportSettings",
                                       "WindowSettings",  //
                                       "DepthBits");

    window_settings_.stencilBits =
        parser_.GetParameterValue<int>("ViewportSettings",
                                       "WindowSettings",  //
                                       "StencilBits");

    window_settings_.antialiasingLevel =
        parser_.GetParameterValue<int>("ViewportSettings",
                                       "WindowSettings",  //
                                       "AntialiasingLevel");

    window_settings_.majorVersion =
        parser_.GetParameterValue<int>("ViewportSettings",
                                       "WindowSettings",  //
                                       "OpenGl_MajorVersion");

    window_settings_.minorVersion =
        parser_.GetParameterValue<int>("ViewportSettings",
                                       "WindowSettings",  //
                                       "OpenGl_MinorVersion");
    is_vertical_sync_enabled_ =
        parser_.GetParameterValue<bool>("ViewportSettings",
                                        "WindowSettings",  //
                                        "IsVerticalSyncEnabled");
  }
  void PrintToLogWindowSettongs() {
    // ^вывод консоьной информации о рендере и настройках вьюпорта

    // зона подключения модификаторов вывода
    // вывод true или false
    std::cerr << std::boolalpha;
    //
    std::cerr << "\t"
              << "\033[4m"
              << "depth bits"
              << "\033[0m"
              << "\t\t" << window_settings_.depthBits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "stencil bits"
              << "\033[0m"
              << "\t\t" << window_settings_.stencilBits << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "antialiasing level"
              << "\033[0m"
              << "\t" << window_settings_.antialiasingLevel << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "vertical sync"
              << "\033[0m"
              << "\t\t" << is_vertical_sync_enabled_ << "\n";
    std::cerr << "\t"
              << "\033[4m"
              << "OpenGL version"
              << "\033[0m"
              << "\t\t" << window_settings_.majorVersion << "."
              << window_settings_.minorVersion << "\n";
    // зона отключения всех модификаторов
    std::cerr << std::noboolalpha;

    //
    std::cout << std::endl;
  }

 public:
  void CreateViewportWindow(std::string L_window_name = "Vieport") {
    window_name_ = L_window_name;
    DefiningOpenGLContextParameters();
    PrintToLogWindowSettongs();

    // Создание окна
    std::cerr << "Window : " << window_name_ << " open \n";
    sf::Window window(sf::VideoMode(window_size_.height, window_size_.width),
                      window_name_, sf::Style::Default, window_settings_);

    // Включение или выключение вертикальной синхронизации
    window.setVerticalSyncEnabled(is_vertical_sync_enabled_);

    // активировать контекст окна для OpenGL
    window.setActive(true);

    // загружайте ресурсы, инициализируйте состояния OpenGL, ...

    // запуск главного цикла окна
    bool running = true;
    while (running) {
      // обработка эвентов
      sf::Event event;
      // главный цикл окна
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          std::cerr << "Window : " << window_name_ << " closed \n";
          // end the program
          running = false;
        } else if (event.type == sf::Event::Resized) {
          // отрегулируйте область просмотра при изменении размера окна
          glViewport(0, 0, event.size.width, event.size.height);
        }
      }

      // clear the buffers
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw...

      // end the current frame (internally swaps the front and back buffers)
      window.display();
    }
    // освобождение ресурсов
  }
};