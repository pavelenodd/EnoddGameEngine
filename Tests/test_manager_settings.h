#pragma once

#include <any>
#include <string>

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
      // проверка загрузки не валидных настроек
      case -99: {
        sucsess &= IsLoadInvalidSettingsWithoutType();
        [[fallthrough]];
      }
      // проверка загрузки  валидных настроек
      case -98: {
        sucsess &= IsLoadValidSettings();
        [[fallthrough]];
      }
      // проверка сохранения настроек
      case -97: {
        sucsess &= IsSaveValidSettings(SettingsType::VIEWPORT_SETTINGS);
        [[fallthrough]];
      }
      // проверка на невалидные данные при сохранении
      case -96: {
        sucsess &= IsSaveInValidSettings(SettingsType::NONE_TYPE);
        [[fallthrough]];
      }
      // получение валидных значений
      case -95: {
        sucsess &= IsGetValidValue(SettingsType::VIEWPORT_SETTINGS, "title");
        [[fallthrough]];
      }
      case -94: {
        // получение невалидных значений
        sucsess &= IsGetInValidValue(SettingsType::VIEWPORT_SETTINGS, "invalid_key");
        [[fallthrough]];
      }
      case -93: {
        // получение невалидных значений
        sucsess &= IsGetInValidValue(SettingsType::NONE_TYPE, "title");
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
      TEST_LOG::Info() << "[TEST][Settings] [Result] " << msg << "\n";
    } else {
      TEST_LOG::Failed() << "[TEST][Settings] [Result] " << msg << "\n";
    }
  };

  // чтение настроек (загрузка JSON)
  bool IsLoadValidSettings() {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start loading valid settings";
    bool sucsess = settings_manager_->LoadSettings(SettingsType::VIEWPORT_SETTINGS);
    Test_Assert(sucsess, "Load valid settings");
    return sucsess;
  };
  // проверка загрузки не валидных настроек
  bool IsLoadInvalidSettingsWithoutType() {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start loading invalid settings";
    bool sucsess = settings_manager_->LoadSettings();
    Test_Assert(!sucsess, "Load invalid settings should fail");
    return !sucsess;
  };

  // сохранение настроек
  bool IsSaveValidSettings(SettingsType type = SettingsType::NONE_TYPE) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start saving valid settings";
    bool sucsess = settings_manager_->SaveSettings(type);
    Test_Assert(sucsess, "Save settings");
    return sucsess;
  };
  bool IsSaveInValidSettings(SettingsType type = SettingsType::NONE_TYPE) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start saving invalid settings";
    bool sucsess = (!settings_manager_->SaveSettings(type));
    Test_Assert(sucsess, "Save settings");
    return sucsess;
  };

  bool IsSetValidValue(SettingsType type = SettingsType::NONE_TYPE,
                       std::string key = "",
                       const std::any& value = std::any()) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start setting valid value";
    bool sucsess = settings_manager_->SetValue(type, key, value);
    Test_Assert(sucsess, "Set settings");
    return sucsess;
  }

  bool IsGetValidValue(SettingsType type = SettingsType::NONE_TYPE,
                       std::string key = "") {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start getting valid value";
    bool sucsess = true;
    auto L_value = settings_manager_->GetValue(type, key);
    sucsess = (std::any_cast<std::string>(L_value) == "Main");
    Test_Assert(sucsess, "Get settings");
    return sucsess;
  }

  bool IsGetInValidValue(SettingsType type = SettingsType::NONE_TYPE,
                         std::string key = "") {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start getting invalid value";
    bool sucsess = true;
    auto L_value = settings_manager_->GetValue(type, key);
    sucsess = (!(std::any_cast<std::string>(L_value) == "Main"));
    Test_Assert(sucsess, "Get settings");
    return sucsess;
  }
  // проверка на дубликаты настроек (ожидается политика → минимум ошибка загрузки)
  bool IsDuplicateSettings(const std::string& path) {
    EDD::TEST_LOG::Info()
        << "[TEST][Settings] [Start] Start checking for duplicate settings";
    bool sucsess = 1;
    Test_Assert(!sucsess, "Duplicate settings should be rejected");
    return !sucsess;
  };

  // проверка граничных значений
  bool IsBoundaryValues() {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start checking boundary values";
    bool sucsess = false;

    return sucsess;
  };
  // проверка невалидных граничных значений
  bool IsInvalidBoundaryValues() {
    EDD::TEST_LOG::Info()
        << "[TEST][Settings] [Start] Start checking invalid boundary values";
    bool sucsess = false;

    return sucsess;
  };
};

}  // namespace Tests
}  // namespace EDD
