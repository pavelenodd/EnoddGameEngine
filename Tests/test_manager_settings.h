#pragma once

#include <any>
#include <string>

#include "Managers/manager_settings.h"
#include "engine_logging.h"

namespace EDD {
namespace Tests {
#if defined(DEBUG)
struct SettingsInspector {
  // Публичные constexpr указатели на приватные поля
  static constexpr auto settings_map = &Managers::Settings::settings_map_;
};

class TestManagerSettings {
 private:
  int test_case_ = -99;
  Managers::Settings* settings_manager_ = nullptr;

 public:
  TestManagerSettings() {
    // Симулируем поведение при загрузке тестов
    settings_manager_ = new Managers::Settings();
  }
  ~TestManagerSettings() {
    if (settings_manager_ != nullptr) {
      delete settings_manager_;
    }
  }
  // получение рефференса на менеджер настроек
  void SetManager(Managers::Settings* manager) {
    if (settings_manager_ != nullptr) {
      delete settings_manager_;
    }
    settings_manager_ = manager;
  };

  bool RunTests() {
    bool sucsess = true;
    auto settings_map_ref = (*settings_manager_).*EDD::Tests::SettingsInspector::settings_map;
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
        sucsess &= IsSaveValidSettings(Managers::SettingsType::VIEWPORT_SETTINGS);
        [[fallthrough]];
      }
      // проверка на невалидные данные при сохранении
      case -96: {
        sucsess &= IsSaveInValidSettings(Managers::SettingsType::NONE_TYPE);
        [[fallthrough]];
      }
      // получение валидных значений
      case -95: {
        auto settings_map_copy = (*settings_manager_).*
                                 EDD::Tests::SettingsInspector::settings_map;
        sucsess &= IsGetValidValue(Managers::SettingsType::VIEWPORT_SETTINGS, "title");
        sucsess &= (settings_map_copy ==
                    (*settings_manager_).*EDD::Tests::SettingsInspector::settings_map);
        sucsess &= IsSaveValidSettings(Managers::SettingsType::VIEWPORT_SETTINGS);
        sucsess &= IsGetValidValue(Managers::SettingsType::VIEWPORT_SETTINGS, "title");
        sucsess &= (settings_map_copy ==
                    (*settings_manager_).*EDD::Tests::SettingsInspector::settings_map);
        if (sucsess) {
          Test_Assert(sucsess, "Settings map was not modified unexpectedly");
        } else {
          Test_Assert(sucsess, "Settings map was modified unexpectedly");
        }
        [[fallthrough]];
      }
      case -94: {
        // получение невалидных значений
        // sucsess &= IsGetInValidValue(SettingsType::VIEWPORT_SETTINGS, "invalid_key");
        [[fallthrough]];
      }
      case -93: {
        // получение невалидных значений
        // sucsess &= IsGetInValidValue(SettingsType::NONE_TYPE, "title");
        [[fallthrough]];
      }
      case 0: {
        Test_Assert(sucsess, "All one-time test cases passed");

        std::cout << "\n "
                     "------------------------------------------------------------------------"
                     "------------------------------------------------------------------------"
                     " \n";
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
      TEST_LOG::SuccessMessage() << "[TEST][Settings] [Result] " << msg << "\n";
    } else {
      TEST_LOG::FailedMessage() << "[TEST][Settings] [Result] " << msg << "\n";
    }
  };

  // чтение настроек (загрузка JSON)
  bool IsLoadValidSettings() {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start loading valid settings";
    bool sucsess = settings_manager_->LoadSettings(Managers::SettingsType::VIEWPORT_SETTINGS);
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
  bool IsSaveValidSettings(Managers::SettingsType type = Managers::SettingsType::NONE_TYPE) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start saving valid settings";
    bool sucsess = settings_manager_->SaveSettings(type);
    Test_Assert(sucsess, "Save settings");
    return sucsess;
  };
  bool IsSaveInValidSettings(Managers::SettingsType type = Managers::SettingsType::NONE_TYPE) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start saving invalid settings";
    bool sucsess = (!settings_manager_->SaveSettings(type));
    Test_Assert(sucsess, "Save settings");
    return sucsess;
  };

  bool IsSetValidValue(Managers::SettingsType type = Managers::SettingsType::NONE_TYPE,
                       std::string key = "",
                       const std::any& value = std::any()) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start setting valid value";
    bool sucsess = settings_manager_->SetValue(type, key, value);
    Test_Assert(sucsess, "Set settings");
    return sucsess;
  }

  bool IsGetValidValue(Managers::SettingsType type = Managers::SettingsType::NONE_TYPE,
                       std::string key = "") {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start getting valid value";
    bool sucsess = true;
    auto L_value = settings_manager_->GetValue(type, key);
    sucsess = (std::any_cast<std::string>(L_value) == "Main");
    Test_Assert(sucsess, "Getting settings");
    return sucsess;
  }

  bool IsGetInValidValue(Managers::SettingsType type = Managers::SettingsType::NONE_TYPE,
                         std::string key = "") {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start getting invalid value";
    bool sucsess = true;
    auto L_value = settings_manager_->GetValue(type, key);

    sucsess = std::any_cast<bool>(L_value);

    Test_Assert(sucsess, "Get settings");
    return sucsess;
  }
  // проверка на дубликаты настроек (ожидается политика → минимум ошибка загрузки)
  bool IsDuplicateSettings(const std::string& path) {
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start checking for duplicate settings";
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
    EDD::TEST_LOG::Info() << "[TEST][Settings] [Start] Start checking invalid boundary values";
    bool sucsess = false;

    return sucsess;
  };
};
#endif

}  // namespace Tests
}  // namespace EDD



