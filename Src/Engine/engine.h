#include <GLFW/glfw3.h>

#include <string>
#include <vector>

/**/
#include "GameObjects/game_objects.h"
/**/
#include "InputsOutputs/input_output.h"
#include "JsonParser/json_parser.h"
#include "Render/render.h"
#include "Tools/debugger.h"
#include "engine_data.h"
namespace edd {
/**
 * @brief класс основного цикла игры
 */
class MainLoop : edd::Render {
 private:
  bool is_main_loop_active_ = false;

  void GameLoop(GLFWwindow* vieport, std::vector<GameObject*> game_objects) {
    while (is_main_loop_active_) {
      /*

      обработка эвентов ввода


      */

      while (!glfwWindowShouldClose(vieport)) {
        edd::Render::DrawObjects(game_objects);
        glfwSwapBuffers(vieport);
        glfwPollEvents();
      }
      if (glfwWindowShouldClose(vieport)) {
        MainLoopStop(vieport);
      }
    }
    if (!vieport) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      MainLoopStop(vieport);
    }
  }

 protected:
 public:
  void MainLoopStart(GLFWwindow* vieport,
                     std::vector<GameObject*> render_objects) {
    is_main_loop_active_ = true;
    GameLoop(vieport, render_objects);
  };
  void MainLoopStop(GLFWwindow* vieport) {
    is_main_loop_active_ = false;
    glfwDestroyWindow(vieport);
    glfwTerminate();
  };
};
class Engine : private edd::Inputs, private edd::Outputs {
 private:
  edd::settings::Engine engine_settings_;
  GLFWwindow* game_vieport_ = nullptr;
  //^ переменные для работы с вьюпортом
  std::vector<GameObject*> game_objects_;
  //^ список игровых объектов

  MainLoop game_loop;
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
  /**
   * @brief получение данных контекста OpenGL из json
   */
  void DefiningOpenGLContextParameters() {
    JsonParser json_parser_("Data/output.json");

    {  // установка размера  вьюпорта

      engine_settings_.vieport_size.height =
          json_parser_.GetParameterValue<int>("ViewportSettings",
                                              "ViewportSize",  //
                                              "Height");
      engine_settings_.vieport_size.width =
          json_parser_.GetParameterValue<int>("ViewportSettings",
                                              "ViewportSize",  //
                                              "Width");
    }
    {  // установка версии OpenGL

      engine_settings_.major_version =
          json_parser_.GetParameterValue<int>("ViewportSettings",
                                              "WindowSettings",  //
                                              "OpenGL_MajorVersion");

      engine_settings_.minor_version =
          json_parser_.GetParameterValue<int>("ViewportSettings",
                                              "WindowSettings",  //
                                              "OpenGL_MinorVersion");
    }
    {  // установка профиля ядра
      if (json_parser_.GetParameterValue<bool>("ViewportSettings",
                                               "OpenGL_Settings",  //
                                               "OpenGL_Profile",   //
                                               "ANY_PROFILE")) {
        engine_settings_.core_profile =
            json_parser_.GetParameterValue<bool>("ViewportSettings",
                                                 "OpenGL_Settings",  //
                                                 "OpenGL_Profile",   //
                                                 "ANY_PROFILE_CODE");
      }
      if (json_parser_.GetParameterValue<bool>("ViewportSettings",
                                               "OpenGL_Settings",  //
                                               "OpenGL_Profile",   //
                                               "CORE_PROFILE")) {
        engine_settings_.core_profile =
            json_parser_.GetParameterValue<bool>("ViewportSettings",
                                                 "OpenGL_Settings",  //
                                                 "OpenGL_Profile",   //
                                                 "CORE_PROFILE_CODE");
      }
      if (json_parser_.GetParameterValue<bool>("ViewportSettings",
                                               "OpenGL_Settings",  //
                                               "OpenGL_Profile",   //
                                               "COMPAT_PROFILE")) {
        engine_settings_.core_profile =
            json_parser_.GetParameterValue<bool>("ViewportSettings",
                                                 "OpenGL_Settings",  //
                                                 "OpenGL_Profile",   //
                                                 "COMPAT_PROFILE_CODE");
      }
    }
    {  // установка Debug Mode OpenGL
      engine_settings_.debug_mode =
          json_parser_.GetParameterValue<bool>("ViewportSettings",
                                               "OpenGL_Settings",  //
                                               "OpenGL_DebugMode"  //
          );
    }
  }

 public:
  Engine() {
    DefiningOpenGLContextParameters();
    game_vieport_ = Outputs::CreateViewport(engine_settings_);
    Outputs::SetVieportActive(game_vieport_);
    Outputs::SetVieportContextSettings(engine_settings_);
    auto test_obj = new GameObject;     //<=== TODO удалить
    game_objects_.push_back(test_obj);  //<=== TODO удалить
  }
  ~Engine() { SaveEngineSettings(); }
  void GameLoopStart() {
    game_loop.MainLoopStart(game_vieport_,  //
                            game_objects_);
  }
  void GameLoopStop() { game_loop.MainLoopStop(game_vieport_); }
};

}  // namespace edd