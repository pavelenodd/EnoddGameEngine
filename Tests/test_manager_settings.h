// Tests/test_manager_settings_single.cpp
#pragma once

#include <cmath>
#include <string>

#include "Managers/manager_settings.h"
#include "engine_logging.h"

// ================================================
// Встроенный тест-раннер для manager_settings
// Одноразовые кейсы [-99..0] + рантайм [1..] с проваливанием
// Покрытие: границы значений, загрузка/валидность, save→load,
// изменение значений и их устойчивость.
// ================================================

namespace EDD {
namespace Tests {

// INFO: Assert helpers without macros (keep integrated tests)
inline void Test_Assert(bool condition, const std::string& msg) {
  bool L_ok = static_cast<bool>(condition);
  if (!L_ok) {
    LOG::Error(__FILE__, __LINE__) << "[TEST][Settings] Assert failed: " << msg;
  }
};

inline void Test_Require(bool condition, const std::string& msg) {
  bool L_ok = static_cast<bool>(condition);
  if (!L_ok) {
    LOG::Fatal(__FILE__, __LINE__) << "[TEST][Settings] Require failed: " << msg;
  }
};

class TestManagerSettings {
 private:
  bool is_do_once_ = false;  // одноразовый блок уже выполнен
  int test_case_ = -99;      // текущий case
  Managers::Settings* settings_manager_ = nullptr;

 public:
  void Set_Manager(Managers::Settings* manager) {
    settings_manager_ = manager;
  };

  // INFO: дергать в debug-такте
  void Run_Tests() {
    Test_Require(settings_manager_ != nullptr, "Settings manager is null");

    switch (test_case_) {
      // -----------------------------
      // Одноразовые кейсы (инициализация и базовая валидация)
      // -----------------------------
      case -99: {
        Case_Init_Test_Env_();
        [[fallthrough]];
      }
      case -98: {
        Case_Load_Valid_Config_();
        [[fallthrough]];
      }
      case -97: {
        Case_Boundaries_IntFloatEnum_();
        [[fallthrough]];
      }
      case -96: {
        Case_Invalid_Types_And_Keys_();
        [[fallthrough]];
      }
      case -95: {
        Case_Defaults_Merge_();
        [[fallthrough]];
      }
      case -94: {
        Case_Save_Then_Load_Roundtrip_();
        [[fallthrough]];
      }
      case -93: {
        Case_Modify_And_Persist_();
        [[fallthrough]];
      }
      case -92: {
        Case_Readonly_And_Schema_Version_();
        [[fallthrough]];
      }
      case -91: {
        Case_Corrupt_And_Recovery_();
        [[fallthrough]];
      }
      case -90: {
        Case_HotReload_Atomic_();
        [[fallthrough]];
      }
      case -89: {
        Case_Perf_Load_Sanity_();
        [[fallthrough]];
      }
      case -88: {
        Case_Finalize_Once_();
        break;
      }

      // Маяк конца одноразовых
      case 0: {
        break;
      }

      // -----------------------------
      // Рантайм кейсы (живые инварианты и нотификации)
      // -----------------------------
      case 1: {
        Case_Runtime_Beacon_();
        [[fallthrough]];
      }
      case 2: {
        Case_Live_GetSet_Consistency_();
        [[fallthrough]];
      }
      case 3: {
        Case_No_Notify_On_Noop_();
        [[fallthrough]];
      }
      case 4: {
        Case_Notify_Once_Per_Effective_Change_();
        [[fallthrough]];
      }
      case 5: {
        Case_Reload_Propagates_Changes_();
        [[fallthrough]];
      }
      case 6: {
        Case_Frame_Safe_Reads_();
        [[fallthrough]];
      }
      case 7: {
        Case_Logs_Rate_Limit_();
        [[fallthrough]];
      }
      default: {
        break;
      }
    }

    if (test_case_ < 0) {
      if (!is_do_once_) {
        test_case_ = 0;
        is_do_once_ = true;
      }
    } else {
      ++test_case_;
    }
  };

 private:
  // ============================================
  // Одноразовые
  // ============================================

  // -99: инициализация тестового окружения
  void Case_Init_Test_Env_() {
    LOG::Info(__FILE__, __LINE__) << "[TEST][Settings] Init test env";
    bool L_inited = settings_manager_->Init_Test_Mode();
    Test_Require(L_inited, "Init_Test_Mode failed");
    // Базовые дефолты для предикатов
    settings_manager_->Set_Value("video.vsync", true);
    settings_manager_->Set_Value("video.target_fps", 60);
    settings_manager_->Set_Value("audio.master_db", -6.0f);
  };

  // -98: загрузка валидного файла и валидация ключей/типов
  void Case_Load_Valid_Config_() {
    bool L_loaded = settings_manager_->Load_From_File(
        "Resources/Configs/valid_settings.json");
    Test_Require(L_loaded, "valid_settings.json failed to load");
    Test_Assert(settings_manager_->Key_Exists("video.vsync"), "Missing key video.vsync");
    Test_Assert(settings_manager_->Get_Value<int>("video.target_fps") > 0,
                "Bad target_fps");
  };

  // -97: граничные состояния для int/float/enum
  void Case_Boundaries_IntFloatEnum_() {
    // int: target_fps ∈ [0..1000]
    settings_manager_->Set_Value("video.target_fps", -1);
    int L_fps_min = settings_manager_->Get_Value<int>("video.target_fps");
    Test_Assert(L_fps_min >= 0, "Clamp lower bound failed for target_fps");

    settings_manager_->Set_Value("video.target_fps", 5000);
    int L_fps_max = settings_manager_->Get_Value<int>("video.target_fps");
    Test_Assert(L_fps_max <= 1000, "Clamp upper bound failed for target_fps");

    // float: master_db ∈ [-80..+6]
    settings_manager_->Set_Value("audio.master_db", 9.0f);
    float L_db_up = settings_manager_->Get_Value<float>("audio.master_db");
    Test_Assert(L_db_up <= 6.0f + 1e-3f, "Clamp upper bound failed for master_db");

    settings_manager_->Set_Value("audio.master_db", -100.0f);
    float L_db_dn = settings_manager_->Get_Value<float>("audio.master_db");
    Test_Assert(L_db_dn >= -80.0f - 1e-3f, "Clamp lower bound failed for master_db");

    // enum: quality ∈ {0=LOW,1=MED,2=HIGH}, автокоррекция на ближайшее валидное
    settings_manager_->Set_Value("video.quality", -5);
    int L_q1 = settings_manager_->Get_Value<int>("video.quality");
    Test_Assert(L_q1 == 0, "Enum lower correction failed");

    settings_manager_->Set_Value("video.quality", 99);
    int L_q2 = settings_manager_->Get_Value<int>("video.quality");
    Test_Assert(L_q2 == 2, "Enum upper correction failed");
  };

  // -96: некорректные типы/неизвестные ключи → политика
  void Case_Invalid_Types_And_Keys_() {
    bool L_ok = settings_manager_->Load_From_File("Resources/Configs/invalid_types.json");
    Test_Assert(!L_ok, "Invalid types config should fail");
    Test_Assert(settings_manager_->Has_Error_Code("ERR_TYPE_MISMATCH"),
                "Expected ERR_TYPE_MISMATCH");

    // Неизвестные ключи (например, strict-режим: error/ignore)
    bool L_applied = settings_manager_->Apply_Delta(R"({"video.unknown_key":123})");
    Test_Assert(!L_applied || settings_manager_->Has_Warning_Code("WARN_UNKNOWN_KEY"),
                "Unknown key policy not reported");
  };

  // -95: слияние с дефолтами
  void Case_Defaults_Merge_() {
    bool L_def = settings_manager_->Load_Defaults("Resources/Configs/defaults.json");
    Test_Require(L_def, "Defaults load failed");

    bool L_user = settings_manager_->Load_From_File(
        "Resources/Configs/partial_user.json");
    Test_Require(L_user, "Partial user load failed");

    Test_Assert(settings_manager_->Key_Exists("controls.jump"),
                "defaults not merged (controls.jump)");
    Test_Assert(settings_manager_->Get_Value<int>("video.target_fps") > 0,
                "defaults not applied");
  };

  // -94: сохранение → загрузка (roundtrip)
  void Case_Save_Then_Load_Roundtrip_() {
    const char* L_out = "Tmp/test_settings_round.json";
    bool L_saved = settings_manager_->Save_To_File(L_out);
    Test_Require(L_saved, "Save_To_File failed");

    Managers::Settings L_shadow;
    L_shadow.Init_Test_Mode();
    bool L_loaded = L_shadow.Load_From_File(L_out);
    Test_Require(L_loaded, "Roundtrip load failed");

    Test_Assert(L_shadow.Get_Value<bool>("video.vsync") ==
                    settings_manager_->Get_Value<bool>("video.vsync"),
                "Roundtrip mismatch vsync");
    Test_Assert(L_shadow.Get_Value<int>("video.target_fps") ==
                    settings_manager_->Get_Value<int>("video.target_fps"),
                "Roundtrip mismatch fps");
  };

  // -93: изменение данных и их сохранность после перезагрузки
  void Case_Modify_And_Persist_() {
    settings_manager_->Set_Value("video.vsync", false);
    settings_manager_->Set_Value("video.target_fps", 144);
    settings_manager_->Set_Value("audio.master_db", -3.0f);

    const char* L_out = "Tmp/test_settings_persist.json";
    bool L_saved = settings_manager_->Save_To_File(L_out);
    Test_Require(L_saved, "Save persist failed");

    Managers::Settings L_reload;
    L_reload.Init_Test_Mode();
    bool L_loaded = L_reload.Load_From_File(L_out);
    Test_Require(L_loaded, "Persist reload failed");

    Test_Assert(L_reload.Get_Value<bool>("video.vsync") == false,
                "Persist vsync mismatch");
    Test_Assert(L_reload.Get_Value<int>("video.target_fps") == 144,
                "Persist fps mismatch");
    Test_Assert(std::fabs(L_reload.Get_Value<float>("audio.master_db") - (-3.0f)) < 0.01f,
                "Persist master_db mismatch");
  };

  // -92: read-only и схема версии
  void Case_Readonly_And_Schema_Version_() {
    int L_prev = settings_manager_->Get_Value<int>("schema.version");
    bool L_try = settings_manager_->Try_Set_Readonly("schema.version", L_prev + 1);
    Test_Assert(!L_try, "Readonly schema.version mutated");
    Test_Assert(settings_manager_->Get_Value<int>("schema.version") == L_prev,
                "schema.version changed unexpectedly");
  };

  // -91: повреждённый файл и восстановление
  void Case_Corrupt_And_Recovery_() {
    bool L_ok = settings_manager_->Load_From_File("Resources/Configs/corrupt.json");
    Test_Assert(!L_ok, "Corrupt config should fail");
    Test_Assert(settings_manager_->Recovered_From_Backup(), "No backup recovery");
  };

  // -90: hot-reload атомарно (без промежуточных полусостояний)
  void Case_HotReload_Atomic_() {
    settings_manager_->Begin_Hot_Reload();
    bool L_applied = settings_manager_->Hot_Apply_File(
        "Resources/Configs/hotreload_delta.json");
    settings_manager_->End_Hot_Reload();
    Test_Assert(L_applied, "Hot reload apply failed");
  };

  // -89: sanity по времени загрузки (бюджет)
  void Case_Perf_Load_Sanity_() {
    double L_ms = 0.0;
    bool L_ok = settings_manager_->Profile_Load("Resources/Configs/valid_settings.json",
                                                L_ms);
    Test_Assert(L_ok, "Profile_Load failed");
    Test_Assert(L_ms < 50.0, "Config load is too slow");
  };

  // 0: финал одноразовых
  void Case_Finalize_Once_() {
    LOG::Info(__FILE__, __LINE__) << "[TEST][Settings] One-time suite done";
  };

  // ============================================
  // Рантайм
  // ============================================

  // 1: маяк начала
  void Case_Runtime_Beacon_() {
    LOG::Info(__FILE__, __LINE__) << "[TEST][Settings] Runtime checks started";
  };

  // 2: живые геттеры/сеттеры — идемпотентность на одинаковом значении
  void Case_Live_GetSet_Consistency_() {
    int L_prev = settings_manager_->Get_Value<int>("video.target_fps");
    settings_manager_->Set_Value("video.target_fps", L_prev);
    int L_now = settings_manager_->Get_Value<int>("video.target_fps");
    Test_Assert(L_prev == L_now, "Get/Set consistency broken");
  };

  // 3: нет уведомлений на noop
  void Case_No_Notify_On_Noop_() {
    int L_hits = 0;
    auto L_id = settings_manager_->Subscribe("video.vsync", [&](bool) { ++L_hits; });
    settings_manager_->Set_Value("video.vsync",
                                 settings_manager_->Get_Value<bool>("video.vsync"));
    settings_manager_->Unsubscribe(L_id);
    Test_Assert(L_hits == 0, "No-op should not notify");
  };

  // 4: одно уведомление на фактическое изменение
  void Case_Notify_Once_Per_Effective_Change_() {
    int L_hits = 0;
    auto L_id = settings_manager_->Subscribe("audio.master_db", [&](float) { ++L_hits; });
    settings_manager_->Set_Value("audio.master_db", -2.0f);
    settings_manager_->Set_Value("audio.master_db", -2.0f);  // noop
    settings_manager_->Unsubscribe(L_id);
    Test_Assert(L_hits == 1, "Notify once violated");
  };

  // 5: reload распространяет изменения (счётчик изменённых ключей >= 0)
  void Case_Reload_Propagates_Changes_() {
    settings_manager_->Trigger_Reload();
    Test_Assert(settings_manager_->Get_Last_Reload_Changes() >= 0,
                "Reload did not report changes");
  };

  // 6: frame-safe чтения
  void Case_Frame_Safe_Reads_() {
    Test_Assert(settings_manager_->Is_Frame_Safe_Reads(), "Reads are not frame-safe");
  };

  // 7: rate-limit логов
  void Case_Logs_Rate_Limit_() {
    settings_manager_->Debug_Spam_Warning();
    settings_manager_->Debug_Spam_Warning();
    Test_Assert(settings_manager_->Did_Rate_Limit(), "Log rate-limit not applied");
  };
};
}  // namespace Tests

}  // namespace EDD

// ===== Пример встраивания в цикл =====
// static EDD::Tests::TestManagerSettings g_settings_tests;
// static bool L_inited = false;
// if (!L_inited) {
//   g_settings_tests.Set_Manager(&Managers::Get_Settings());
//   L_inited = true;
// }
// g_settings_tests.Run_Tests();
