#pragma once
#include "EngineError/engine_logging.h"
namespace EDD {
namespace Tests {

class TestManagerScene {
 public:
  bool RunTests() {
    // Здесь будут тесты для менеджера сцены
    return true;
  }

 private:
  inline void Test_Assert(bool condition, const std::string& msg) {
    if (condition) {
      TEST_LOG::Success() << "[TEST][Settings] Assert passed: " << msg;
    } else {
      TEST_LOG::Failed(__FILE__, __LINE__) << "[TEST][Settings] Assert failed: " << msg;
    }
  };
};
}  // namespace Tests
}  // namespace EDD