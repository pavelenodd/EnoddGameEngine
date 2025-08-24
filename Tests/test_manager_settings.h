#pragma once

#include <any>
#include <string>
#include <typeinfo>

#include "Managers/manager_settings.h"
#include "engine_logging.h"

namespace EDD {
namespace Tests {

class TestManagerSettings {
 private:
  int test_case_ = -99;
  EDD::Settings* settings_manager_ = nullptr;

 public:
  TestManagerSettings() {
    // Симулируем поведение при загрузке тестов
    settings_manager_ = new EDD::Settings();
  }
  ~TestManagerSettings() {
    if (settings_manager_ != nullptr) {
      delete settings_manager_;
    }
  }
  // получение рефференса на менеджер настроек
  void SetManager(EDD::Settings* manager) {
    if (settings_manager_ != nullptr) {
      delete settings_manager_;
    }
    settings_manager_ = manager;
  };

  bool RunTests() {
    bool sucsess = true;

    switch (test_case_) {
      case -99: {
        // проверка загрузки не валидных настроек
        sucsess &= IsLoadInvalidSettingsWithoutType();
        [[fallthrough]];
      }
      case -98: {
        // проверка загрузки  валидных настроек
        sucsess &= IsLoadValidSettings();
        [[fallthrough]];
      }
      case -97: {
        // проверка сохранения настроек
        sucsess &= IsSaveSettings("Settings/Test/viewport_settings.jsonc");
        [[fallthrough]];
      }
      case 0: {
        if (sucsess) {
          TEST_LOG::Success() << "[TEST][Settings] All test cases passed";
        } else {
          TEST_LOG::Failed() << "[TEST][Settings] cases FAILED";
        }
        test_case_ = 1;
        break;  // INFO: пока break потом будет проваливание если нужно
      }
      default: {
        break;
      }
    }

    return sucsess;
  };

 private:
  inline void Test_Assert(bool condition, const std::string& msg) {
    if (condition) {
      TEST_LOG::Info() << "[TEST][Settings] " << msg;
    } else {
      TEST_LOG::Failed() << "[TEST][Settings] " << msg;
    }
  };

  // чтение настроек (загрузка JSON)
  bool IsLoadValidSettings() {
    bool L_ok = settings_manager_->LoadSettings(SettingsType::VIEWPORT_SETTINGS);
    Test_Assert(L_ok, "Load valid settings");
    return L_ok;
  };
  // проверка загрузки не валидных настроек
  bool IsLoadInvalidSettingsWithoutType() {
    bool L_ok = settings_manager_->LoadSettings();
    Test_Assert(!L_ok, "Load invalid settings should fail");
    return !L_ok;
  };

  // сохранение настроек
  bool IsSaveSettings(const std::string& path) {
    bool L_ok = settings_manager_->SaveSettings(SettingsType::VIEWPORT_SETTINGS);
    Test_Assert(L_ok, "Save to " + path);
    return L_ok;
  };

  // проверка на дубликаты настроек (ожидается политика → минимум ошибка загрузки)
  bool IsDuplicateSettings(const std::string& path) {
    bool L_ok = 1;
    Test_Assert(!L_ok, "Duplicate settings should be rejected");
    return !L_ok;
  };

  // проверка граничных значений
  bool IsBoundaryValues() {
    bool L_success = false;

    return L_success;
  };
  // проверка невалидных граничных значений
  bool IsInvalidBoundaryValues() {
    bool L_success = false;

    return L_success;
  };
};

}  // namespace Tests
}  // namespace EDD
