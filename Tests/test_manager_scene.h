#pragma once
#include <tuple>

#include "EngineError/engine_logging.h"
#include "Managers/manager_scene.h"
#include "EngineData/engine_data.h"

namespace EDD {
namespace Tests {
#if defined(DEBUG)
// Инспектор приватных полей (см. friend в менеджере)
struct SceneInspector {
  static const std::vector<EDD::Data::Viewport*>& Get_Viewport_(Managers::Scene& scene) {
    return scene.viewports_;
  }

  static EDD::Data::Viewport* Call_CreateViewport(Managers::Scene& scene,
                                                  std::string title,
                                                  uint16_t width,
                                                  uint16_t height) {
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
        scene_manager_->Init(std::vector<std::any>{
            std::make_any<std::tuple<const std::string, uint16_t, uint16_t>>(
                std::string("TestMainViewport"), 100, 50)});
        scene_manager_->DestroyViewport("TestMainViewport");
        success &= scene_manager_->GetAllViewports().empty();

        Test_Assert(success, "Create and Destroy  Test Viewport to title");
        [[fallthrough]];
      }
      case -98: {
        success &= CreateViewport("TestViewport", 640, 480);
        scene_manager_->DestroyAllViewport();
        success &= scene_manager_->GetAllViewports().empty();

        Test_Assert(success, "Create and Destroy  all Viewport`s");
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
      TEST_LOG::SuccessMessage() << "[TEST][Scene] [Result] " << msg << "\n";
    } else {
      TEST_LOG::FailedMessage() << "[TEST][Scene] [Result] " << msg << "\n";
    }
  }
  // Создание вьюпорта со стандартными настройками
  bool CreateViewport(const std::string& name, uint16_t width, uint16_t height) {
    EDD::Data::Viewport* vp = SceneInspector::Call_CreateViewport(
        *scene_manager_, name, width, height);

    bool condition = (vp != nullptr);
    return condition;
  }
  // Уничтожение всех вьюпортов
  void DestroyAllViewport() {
    scene_manager_->DestroyAllViewport();
  }
  void FreeResources() {
    scene_manager_->FreeResources();
  }
};
#endif

}  // namespace Tests
}  // namespace EDD
