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
  void SetManager(EDD::Settings* manager) {
    settings_manager_ = manager;
  };

  bool RunTests() {
    bool sucsess = true;

    Test_Assert(settings_manager_ != nullptr, "Settings manager is null");
    if (settings_manager_ == nullptr) return false;

    switch (test_case_) {
      case -99: {
        sucsess &= IsLoad_Settings_();
        [[fallthrough]];
      }
      case -98: {
        sucsess &= IsSave_Settings_();
        [[fallthrough]];
      }
      case -97: {
        sucsess &= IsNone_Type_Check_();
        [[fallthrough]];
      }
      case -96: {
        sucsess &= IsDuplicate_Settings_();
        [[fallthrough]];
      }
      case -95: {
        sucsess &= IsBoundary_Values_();
        [[fallthrough]];
      }
      case -94: {
        sucsess &= IsSave_Load_Roundtrip_();
        [[fallthrough]];
      }
      case -93: {
        sucsess &= IsModify_Persist_();
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
      TEST_LOG::Success() << "[TEST][Settings] " << msg;
    } else {
      TEST_LOG::Failed(__FILE__, __LINE__) << "[TEST][Settings] " << msg;
    }
  };

  // -99: чтение настроек (загрузка JSON)
  bool IsLoad_Settings_() {
    bool L_ok = settings_manager_->LoadSettings("Resources/Configs/valid_settings.json");
    Test_Assert(L_ok, "Load valid settings");
    return L_ok;
  };

  // -98: сохранение настроек
  bool IsSave_Settings_() {
    bool L_ok = settings_manager_->SaveSettings("Tmp/test_save.json");
    Test_Assert(L_ok, "Save to Tmp/test_save.json");
    return L_ok;
  };

  // -97: проверка NONE_TYPE реакции (ожидается ошибка → тест успешен, если L_ok == false)
  bool IsNone_Type_Check_() {
    bool L_ok = settings_manager_->LoadSettings("Resources/Configs/none_type.json");
    Test_Assert(!L_ok, "NONE_TYPE should fail to load");
    return !L_ok;
  };

  // -96: проверка на дубликаты настроек (ожидается политика → минимум ошибка загрузки)
  bool IsDuplicate_Settings_() {
    bool L_ok = settings_manager_->LoadSettings("Resources/Configs/dup_keys.json");
    // Если у тебя позже появится Get_Last_Policy_Applied(), тут можно будет уточнить
    // стратегию.
    Test_Assert(!L_ok, "Duplicate settings should be rejected");
    return !L_ok;
  };

  // -95: проверка граничных значений (клампы и типы)
  bool IsBoundary_Values_() {
    bool L_success = true;

    // int: video.target_fps >= 0 (верхнюю границу зафиксируешь позже, тут проверка
    // нижней)
    settings_manager_->SetValue("video.target_fps", -1);
    std::any L_any_fps = settings_manager_->GetValue("video.target_fps");
    bool L_type_ok_fps = (L_any_fps.type() == typeid(int));
    Test_Assert(L_type_ok_fps, "GetValue(video.target_fps) returns int");
    L_success &= L_type_ok_fps;
    if (L_type_ok_fps) {
      int L_fps = std::any_cast<int>(L_any_fps);
      Test_Assert(L_fps >= 0, "FPS is clamped to lower bound");
      L_success &= (L_fps >= 0);
    }

    // float: audio.master_db >= -80.0f (верхний кламп можно добавить позже)
    settings_manager_->SetValue("audio.master_db", -100.0f);
    std::any L_any_db = settings_manager_->GetValue("audio.master_db");
    bool L_type_ok_db = (L_any_db.type() == typeid(float));
    Test_Assert(L_type_ok_db, "GetValue(audio.master_db) returns float");
    L_success &= L_type_ok_db;
    if (L_type_ok_db) {
      float L_db = std::any_cast<float>(L_any_db);
      Test_Assert(L_db >= -80.0f, "Audio dB is clamped to lower bound");
      L_success &= (L_db >= -80.0f);
    }

    return L_success;
  };

  // -94: roundtrip save/load со сверкой значений
  bool IsSave_Load_Roundtrip_() {
    // подготавливаем значения
    settings_manager_->SetValue("video.vsync", true);
    settings_manager_->SetValue("video.target_fps", 60);
    bool L_saved = settings_manager_->SaveSettings("Tmp/test_round.json");
    Test_Assert(L_saved, "Roundtrip: save");

    EDD::Settings L_reload;
    bool L_loaded = L_reload.LoadSettings("Tmp/test_round.json");
    Test_Assert(L_loaded, "Roundtrip: load");
    if (!L_loaded) return false;

    // сверяем ключевые значения
    std::any L_vsync_any = L_reload.GetValue("video.vsync");
    std::any L_fps_any = L_reload.GetValue("video.target_fps");

    bool L_ok = true;
    if (L_vsync_any.type() == typeid(bool)) {
      L_ok &= (std::any_cast<bool>(L_vsync_any) == true);
      Test_Assert(std::any_cast<bool>(L_vsync_any) == true,
                  "Roundtrip: video.vsync == true");
    } else {
      Test_Assert(false, "Roundtrip: video.vsync has wrong type");
      L_ok = false;
    }

    if (L_fps_any.type() == typeid(int)) {
      L_ok &= (std::any_cast<int>(L_fps_any) == 60);
      Test_Assert(std::any_cast<int>(L_fps_any) == 60, "Roundtrip: target_fps == 60");
    } else {
      Test_Assert(false, "Roundtrip: video.target_fps has wrong type");
      L_ok = false;
    }

    return L_saved && L_loaded && L_ok;
  };

  // -93: modify and persist — меняем значение и проверяем, что сохранилось
  bool IsModify_Persist_() {
    settings_manager_->SetValue("video.vsync", false);
    bool L_saved = settings_manager_->SaveSettings("Tmp/test_modify.json");
    Test_Assert(L_saved, "ModifyPersist: save");
    if (!L_saved) return false;

    EDD::Settings L_reload;
    bool L_loaded = L_reload.LoadSettings("Tmp/test_modify.json");
    Test_Assert(L_loaded, "ModifyPersist: load");
    if (!L_loaded) return false;

    std::any L_val = L_reload.GetValue("video.vsync");
    if (L_val.type() != typeid(bool)) {
      Test_Assert(false, "ModifyPersist: video.vsync wrong type");
      return false;
    }

    bool L_flag = std::any_cast<bool>(L_val);
    Test_Assert(L_flag == false, "ModifyPersist: video.vsync == false");
    return (L_loaded && (L_flag == false));
  };
};

}  // namespace Tests
}  // namespace EDD
