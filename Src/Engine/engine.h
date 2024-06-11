#include <string>
/**/
#include "GameObjects/game_objects.h"
/**/
#include "InputsOutputs/input_output.h"
#include "JsonParser/json_parser.h"
#include "Render/render.h"
#include "engine_data.h"

namespace edd {

class Engine : protected edd::Inputs, protected edd::Outputs {
 private:
  edd::settings::Engine engine_settings_;
  GLFWwindow* game_vieport_ = nullptr;
  bool is_main_loop_active_ = false;
  //^ переменные для работы с вьюпортом
  std::vector<GameObject*> game_objects_;
  //^ список игровых объектов

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

    engine_settings_.major_version =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "OpenGl_MajorVersion");

    engine_settings_.minor_version =
        json_parser_.GetParameterValue<int>("ViewportSettings",
                                            "WindowSettings",  //
                                            "OpenGl_MinorVersion");
  }

 public:
  void MainLoopStart() { is_main_loop_active_ = true; }
  void MainLoopStop() { is_main_loop_active_ = false; }

  Engine() {
    /**/
    game_objects_.push_back(new GameObject);
    std::cerr << "Game Objects size: " << game_objects_.size() << std::endl;

    /*надо потом удалить*/

    DefiningOpenGLContextParameters();
    MainLoopStart();

    game_vieport_ = edd::Outputs::CreateViewport(engine_settings_);

    // Main Loop
    while (!glfwWindowShouldClose(game_vieport_) && is_main_loop_active_) {
      glfwSwapBuffers(game_vieport_);
      glfwPollEvents();
    }
  }
};

}  // namespace edd