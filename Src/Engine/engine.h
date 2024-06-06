#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

#include "InputsOutputs/input_output.h"
#include "JsonParser/json_parser.h"
#include "Render/render.h"
#include "engine_data.h"

class Engine : protected edd::Inputs, protected edd::Outputs {
 private:
  EngineSettings engine_settings_;
  sf::Window* window_;

  bool is_main_loop_active_ = false;
  //
  void SaveEngineSettings() {
    JsonParser json_parser_("Data/output.json");

    json_parser_.SetParameterValue("ViewportSettings",
                                   "ViewportSize",  //
                                   "Height",
                                   engine_settings_.vieport_size.height);

    json_parser_.SetParameterValue("ViewportSettings",
                                   "ViewportSize",  //
                                   "Width",
                                   engine_settings_.vieport_size.width);
  };
  void DefiningOpenGLContextParameters() {
    // Определение параметров контекста OpenGL
    JsonParser json_parser_("Data/output.json");

    engine_settings_.vieport_size.height =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "ViewportSize",  //
                                            "Height");
    engine_settings_.vieport_size.width =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "ViewportSize",  //
                                            "Width");
    engine_settings_.depth_bits =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "DepthBits");

    engine_settings_.stencil_bits =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "StencilBits");

    engine_settings_.antialiasing_level =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "AntialiasingLevel");

    engine_settings_.major_version =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "OpenGl_MajorVersion");

    engine_settings_.minor_version =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "OpenGl_MinorVersion");
    engine_settings_.is_vertical_sync_enabled =
        json_parser_.GetParameterValue<bool>("ViewportSettings",
                                             "WindowSettings",  //
                                             "IsVerticalSyncEnabled");
  }

 public:
  void MainLoopStart() { is_main_loop_active_ = true; }
  void MainLoopStop() { is_main_loop_active_ = false; }

  Engine() {
    DefiningOpenGLContextParameters();
    MainLoopStart();
    window_ = edd::Outputs::CreateViewport(engine_settings_);

    // window_->setActive(true);

    // Main Loop
    while (is_main_loop_active_) {
      // handle events
      sf::Event event;
      while (window_->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          // end the program
          MainLoopStop();
          SaveEngineSettings();
        } else if (event.type == sf::Event::Resized) {
          // adjust the viewport when the window is resized
          glViewport(0, 0,
                     engine_settings_.vieport_size.width = event.size.height,
                     engine_settings_.vieport_size.height = event.size.width);
          //^умышленно перепутано , проблема в SFML
        }
      }

      // очистка буферов
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // очистка цвета
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

      // Рендеринг
      edd::Outputs::RenderVieport(window_);

      // end the current frame (internally swaps the front and back buffers)
      window_->display();
    }
  }

  ~Engine() {}
};
