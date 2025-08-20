#pragma once
#include "../EngineError/engine_logging.h"
namespace EDD {
namespace Tests {
class TestManagerResources {
 public:
  bool RunTests() {
    // Здесь будут тесты для менеджера ресурсов
    return true;
  }

 private:
  inline void Test_Assert(bool condition, const std::string& msg) {
    if (condition) {
      TEST_LOG::Success() << "[TEST][Resources] Assert passed: " << msg;
    } else {
      TEST_LOG::Failed(__FILE__, __LINE__) << "[TEST][Resources] Assert failed: " << msg;
    }
  };
};
}  // namespace Tests

}  // namespace EDD