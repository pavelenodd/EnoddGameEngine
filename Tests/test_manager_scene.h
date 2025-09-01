#pragma once
#include <tuple>

#include "EngineError/engine_logging.h"
#include "Managers/manager_scene.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace EDD {
namespace Tests {

// Инспектор приватных полей (см. friend в менеджере)
struct SceneInspector {
  static const EDD::Data::Viewport& Get_Viewport_(Managers::Scene& scene) {
    return *(scene.view_data_);
  }

  static EDD::Data::Viewport* Call_CreateViewport(Managers::Scene& scene,
                                                  const std::string& title,
                                                  int width,
                                                  int height) {
    return scene.CreateViewport(title, width, height);
  }
};

class TestManagerScene {
 private:
  // === Управление прогоном ===
  int test_case_ = -99;
  Managers::Scene* scene_manager_ = nullptr;  // внешний менеджер

 public:
  void SetManager(Managers::Scene* manager) {
    scene_manager_ = manager;
  }

  // Вызов в каждом кадре/тике
  bool RunTests() {
    bool success = true;

    switch (test_case_) {
      // ===== Одноразовые =====
      case -99: {
        scene_manager_ = new Managers::Scene();
        scene_manager_->Init(
            std::vector<std::any>{std::make_any<std::tuple<std::string, int, int>>(
                std::string("MainViewport"), 100, 50)});
        success &= CreateViewport();
        [[fallthrough]];
      }
      case -98: {
        [[fallthrough]];
      }
      case -97: {
        [[fallthrough]];
      }
      case -96: {
        [[fallthrough]];
      }
      case -95: {
        [[fallthrough]];
      }
      case -94: {
        [[fallthrough]];
      }
      case -93: {
        [[fallthrough]];
      }

      // ===== Маяк перехода к рантайму =====
      case 0: {
        DestroyAllViewport();
        FreeResources();

        Test_Assert(success, "All one-time test cases passed");
        test_case_ = 1;
        std::cout << "\n "
                     "------------------------------------------------------------------------"
                     "------------------------------------------------------------------------"
                     " \n";
        break;
      }

      // ===== Рантайм (single viewport) =====
      case 1: {
        [[fallthrough]];
      }
      case 2: {
        [[fallthrough]];
      }
      default: {
        break;
      }
    }
    return success;
  }

 private:
  inline void Test_Assert(bool condition, const std::string& msg) {
    if (condition) {
      TEST_LOG::Success() << "[TEST][Scene] [Result] " << msg << "\n";
    } else {
      TEST_LOG::Failed() << "[TEST][Scene] [Result] " << msg << "\n";
    }
  }
  // Создание вьюпорта со стандартными настройками
  bool CreateViewport() {
    EDD::Data::Viewport* vp = SceneInspector::Call_CreateViewport(
        *scene_manager_, "Main", 800, 600);

    bool condition = (vp != nullptr);
    Test_Assert(condition, "CreateViewport");
    return condition;
  }
  // Уничтожение всех вьюпортов
  void DestroyAllViewport() {
    scene_manager_->DestroyViewport();
  }
  void FreeResources() {
    scene_manager_->FreeResources();
  }
};

}  // namespace Tests
}  // namespace EDD
