#pragma once
#include <any>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

#include "../EngineError/engine_logging.h"
#include "manager_base.h"

namespace EDD {
enum SettingsType {
  NONE_TYPE = -1,
  VIEWPORT_SETTINGS = 0,
  AUDIO_SETTINGS = 1,
  GRAPHICS_SETTINGS = 2,
  INPUT_SETTINGS = 3,
  RENDER_SETTINGS = 4

};
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Автоматически проверяет тип данных со схемой, которая содержит тип переменной и её диапазон
      Настройки могут включать параметры окна, аудио и графики.
*/
class Settings : public Managers::Base {
 private:
  std::fstream file_stream_;
  std::map<SettingsType, nlohmann::json> settings_map_;
  std::map<SettingsType, std::string> settings_paths_ = {
      {NONE_TYPE, ""},
      {VIEWPORT_SETTINGS, "Settings/viewport_settings.jsonc"},
      {AUDIO_SETTINGS, "Settings/audio_settings.jsonc"},
      {GRAPHICS_SETTINGS, "Settings/graphics_settings.jsonc"},
      {INPUT_SETTINGS, "Settings/input_settings.jsonc"},
      {RENDER_SETTINGS, "Settings/render_settings.jsonc"}};

 public:
  explicit Settings() {}

  ~Settings() {
    FreeResources();
    LOG::Debug() << "ManagerSettings destroyed";
  }
  // Удаление лишних конструкторов
  Settings(const Settings&) = delete;
  Settings(Settings&&) = delete;
  Settings& operator=(const Settings&) = delete;
  Settings& operator=(Settings&&) = delete;

 public:
  bool LoadSettings(SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot load settings";
      return false;
    }
    if (IsVerifityAndOpeFile(settings_paths_.at(type))) {
      nlohmann::json parsed = nlohmann::json::parse(file_stream_, nullptr, true, true);
      if (parsed.is_array()) {
        if (!parsed.empty() && parsed.front().is_object()) {
          parsed = parsed.front();
        } else {
          LOG::Error() << "Loaded JSON array is empty or not object";
          parsed = nlohmann::json::object();
        }
      }
      settings_map_[type] = std::move(parsed);

      CloseFile();
      LOG::Debug() << "Settings loaded from " << settings_paths_.at(type);
      return true;
    } else {
      LOG::Error() << "Failed to open file: path not found " << settings_paths_.at(type);
      return false;
    }
  }
  bool SaveSettings(SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot save settings";
      return false;
    }

    if (IsVerifityAndOpeFile(settings_paths_.at(type))) {
      file_stream_ << settings_map_.at(type).dump(4);
      CloseFile();
      LOG::Debug() << "Settings saved to " << settings_paths_.at(type);
      return true;
    } else {
      LOG::Error() << "Failed to open file: path not found " << settings_paths_.at(type);
      return false;
    }
    return false;
  }

  bool SetValue(SettingsType type = SettingsType::NONE_TYPE,
                const std::string& key = "",
                const std::any& value = std::any()) {
    return true;
  }

  std::any GetValue(SettingsType type = SettingsType::NONE_TYPE,
                    const std::string& key = "") {
    if (type == NONE_TYPE) {
      LOG::Error() << "GetValue: NONE_TYPE";
      return {};
    }
    auto itType = settings_map_.find(type);
    if (itType == settings_map_.end()) {
      LOG::Error() << "GetValue: settings type not loaded";
      return {};
    }
    auto& root = itType->second;
    if (!root.is_object()) {
      LOG::Error() << "GetValue: root json is not object";
      return {};
    }
    if (!root.contains(key)) {
      LOG::Error() << "GetValue: key not found: " << key;
      return {};
    }
    const nlohmann::json& node = root.at(key);

    std::string expected_type;
    const nlohmann::json* value_json = &node;

    if (node.is_object()) {
      if (node.contains("value")) {
        value_json = &node["value"];
      }
      if (node.contains("type") && node["type"].is_string()) {
        expected_type = node["type"].get<std::string>();
      }
    }

    return JsonToAny(*value_json, expected_type);
  }

  void Update() override {}
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "ManagerSettings initialized";
  }
  void FreeResources() override {}

 private:
  /**
   * @brief Проверяет, можно ли открыть файл по указанному пути и открывает его
   * @param file_path Путь к файлу
   */
  bool IsVerifityAndOpeFile(const std::string& file_path) {
    if (file_stream_.is_open()) {
      file_stream_.close();
    }
    std::fstream file(file_path, std::ios::in | std::ios::out);
    file_stream_ = std::move(file);
    return file_stream_.is_open();
  }
  /**
   * @brief Закрывает файл, если он открыт
   * @param file_path Путь к файлу для закрытия
   */
  void CloseFile() {
    if (file_stream_.is_open()) {
      file_stream_.close();
    }
  }

  nlohmann::json AnyToJson(const std::any& value, const std::string& expected_type) {
    nlohmann::json j = nullptr;
    if (!value.has_value()) return j;
    const auto& et = expected_type;
    const std::type_info& t = value.type();

    // string
    auto to_string_any = [&](std::string& out) -> bool {
      if (t == typeid(std::string)) {
        out = std::any_cast<const std::string&>(value);
        return true;
      }
      if (t == typeid(const char*)) {
        out = std::string(std::any_cast<const char*>(value));
        return true;
      }
      if (t == typeid(char*)) {
        out = std::string(std::any_cast<char*>(value));
        return true;
      }
      if (t == typeid(char)) {
        out = std::string(1, std::any_cast<char>(value));
        return true;
      }
      if (t == typeid(bool)) {
        out = std::any_cast<bool>(value) ? "true" : "false";
        return true;
      }
      if (t == typeid(int)) {
        out = std::to_string(std::any_cast<int>(value));
        return true;
      }
      if (t == typeid(long)) {
        out = std::to_string(std::any_cast<long>(value));
        return true;
      }
      if (t == typeid(long long)) {
        out = std::to_string(std::any_cast<long long>(value));
        return true;
      }
      if (t == typeid(unsigned int)) {
        out = std::to_string(std::any_cast<unsigned int>(value));
        return true;
      }
      if (t == typeid(unsigned long)) {
        out = std::to_string(std::any_cast<unsigned long>(value));
        return true;
      }
      if (t == typeid(unsigned long long)) {
        out = std::to_string(std::any_cast<unsigned long long>(value));
        return true;
      }
      if (t == typeid(float)) {
        out = std::to_string(std::any_cast<float>(value));
        return true;
      }
      if (t == typeid(double)) {
        out = std::to_string(std::any_cast<double>(value));
        return true;
      }
      if (t == typeid(long double)) {
        out = std::to_string(static_cast<double>(std::any_cast<long double>(value)));
        return true;
      }
      return false;
    };

    // int
    auto to_int_any = [&](long long& out) -> bool {
      if (t == typeid(int)) {
        out = std::any_cast<int>(value);
        return true;
      }
      if (t == typeid(short)) {
        out = std::any_cast<short>(value);
        return true;
      }
      if (t == typeid(long)) {
        out = std::any_cast<long>(value);
        return true;
      }
      if (t == typeid(long long)) {
        out = std::any_cast<long long>(value);
        return true;
      }
      if (t == typeid(unsigned int)) {
        out = std::any_cast<unsigned int>(value);
        return true;
      }
      if (t == typeid(unsigned long)) {
        out = std::any_cast<unsigned long>(value);
        return true;
      }
      if (t == typeid(unsigned long long)) {
        out = std::any_cast<unsigned long long>(value);
        return true;
      }
      if (t == typeid(bool)) {
        out = std::any_cast<bool>(value) ? 1 : 0;
        return true;
      }
      if (t == typeid(float)) {
        out = static_cast<long long>(std::any_cast<float>(value));
        return true;
      }
      if (t == typeid(double)) {
        out = static_cast<long long>(std::any_cast<double>(value));
        return true;
      }
      if (t == typeid(long double)) {
        out = static_cast<long long>(std::any_cast<long double>(value));
        return true;
      }
      if (t == typeid(std::string)) {
        const auto& s = std::any_cast<const std::string&>(value);
        try {
          out = std::stoll(s);
          return true;
        } catch (...) {
          return false;
        }
      }
      return false;
    };

    // double
    auto to_double_any = [&](double& out) -> bool {
      if (t == typeid(float)) {
        out = std::any_cast<float>(value);
        return true;
      }
      if (t == typeid(double)) {
        out = std::any_cast<double>(value);
        return true;
      }
      if (t == typeid(long double)) {
        out = static_cast<double>(std::any_cast<long double>(value));
        return true;
      }
      if (t == typeid(int)) {
        out = static_cast<double>(std::any_cast<int>(value));
        return true;
      }
      if (t == typeid(long)) {
        out = static_cast<double>(std::any_cast<long>(value));
        return true;
      }
      if (t == typeid(long long)) {
        out = static_cast<double>(std::any_cast<long long>(value));
        return true;
      }
      if (t == typeid(unsigned int)) {
        out = static_cast<double>(std::any_cast<unsigned int>(value));
        return true;
      }
      if (t == typeid(unsigned long)) {
        out = static_cast<double>(std::any_cast<unsigned long>(value));
        return true;
      }
      if (t == typeid(unsigned long long)) {
        out = static_cast<double>(std::any_cast<unsigned long long>(value));
        return true;
      }
      if (t == typeid(bool)) {
        out = std::any_cast<bool>(value) ? 1.0 : 0.0;
        return true;
      }
      if (t == typeid(std::string)) {
        const auto& s = std::any_cast<const std::string&>(value);
        try {
          out = std::stod(s);
          return true;
        } catch (...) {
          return false;
        }
      }
      return false;
    };

    // bool
    auto to_bool_any = [&](bool& out) -> bool {
      if (t == typeid(bool)) {
        out = std::any_cast<bool>(value);
        return true;
      }
      if (t == typeid(int)) {
        out = std::any_cast<int>(value) != 0;
        return true;
      }
      if (t == typeid(long)) {
        out = std::any_cast<long>(value) != 0;
        return true;
      }
      if (t == typeid(long long)) {
        out = std::any_cast<long long>(value) != 0;
        return true;
      }
      if (t == typeid(unsigned int)) {
        out = std::any_cast<unsigned int>(value) != 0u;
        return true;
      }
      if (t == typeid(unsigned long)) {
        out = std::any_cast<unsigned long>(value) != 0ul;
        return true;
      }
      if (t == typeid(unsigned long long)) {
        out = std::any_cast<unsigned long long>(value) != 0ull;
        return true;
      }
      if (t == typeid(float)) {
        out = std::any_cast<float>(value) != 0.0f;
        return true;
      }
      if (t == typeid(double)) {
        out = std::any_cast<double>(value) != 0.0;
        return true;
      }
      if (t == typeid(long double)) {
        out = std::any_cast<long double>(value) != 0.0L;
        return true;
      }
      if (t == typeid(std::string)) {
        const auto& s = std::any_cast<const std::string&>(value);
        if (s == "true" || s == "1" || s == "TRUE" || s == "True") {
          out = true;
          return true;
        }
        if (s == "false" || s == "0" || s == "FALSE" || s == "False") {
          out = false;
          return true;
        }
        return false;
      }
      return false;
    };

    if (et == "string") {
      std::string s;
      if (to_string_any(s)) j = std::move(s);
    } else if (et == "int") {
      long long v = 0;
      if (to_int_any(v)) j = v;
    } else if (et == "float" || et == "double") {
      double v = 0.0;
      if (to_double_any(v)) j = v;
    } else if (et == "bool") {
      bool v = false;
      if (to_bool_any(v)) j = v;
    }
    return j;
  }
  std::any JsonToAny(const nlohmann::json& j, const std::string& expected_type) const {
    // TODO
    //  !надо переписать тк должен быть стандартный способ использовать To lower в std
    auto to_lower = [](std::string s) {
      for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
      }
      return s;
    };

    std::string et = to_lower(expected_type);

    auto deduce_type = [&]() -> std::string {
      if (!et.empty()) return et;
      if (j.is_string()) return "string";
      if (j.is_boolean()) return "bool";
      if (j.is_number_integer() || j.is_number_unsigned()) return "int";
      if (j.is_number_float()) return "double";
      return "unknown";
    };
    et = deduce_type();

    // Общие маленькие помощники
    auto conv_string = [&]() -> std::any {
      if (j.is_string()) return j.get<std::string>();
      if (j.is_boolean())
        return j.get<bool>() ? std::string("true") : std::string("false");
      if (j.is_number()) return j.dump();  // число в текст
      return {};
    };
    auto conv_bool = [&]() -> std::any {
      if (j.is_boolean()) return j.get<bool>();
      if (j.is_number()) return j.get<double>() != 0.0;
      if (j.is_string()) {
        auto s = to_lower(j.get<std::string>());
        if (s == "true") return true;
        if (s == "false") return false;
      }
      return {};
    };
    auto conv_int = [&]() -> std::any {
      if (j.is_number_integer()) return static_cast<int>(j.get<long long>());
      if (j.is_number_unsigned()) return static_cast<int>(j.get<unsigned long long>());
      if (j.is_number_float()) return static_cast<int>(j.get<double>());
      if (j.is_boolean()) return j.get<bool>() ? 1 : 0;
      if (j.is_string()) {
        try {
          return std::stoi(j.get<std::string>());
        } catch (...) {
          return std::any{};
        }
      }
      return {};
    };
    auto conv_double = [&]() -> std::any {
      if (j.is_number()) return j.get<double>();
      if (j.is_boolean()) return j.get<bool>() ? 1.0 : 0.0;
      if (j.is_string()) {
        try {
          return std::stod(j.get<std::string>());
        } catch (...) {
          return std::any{};
        }
      }
      return {};
    };
    auto conv_fallback = [&]() -> std::any {
      if (j.is_string()) return j.get<std::string>();
      if (j.is_boolean()) return j.get<bool>();
      if (j.is_number_integer()) return static_cast<int>(j.get<long long>());
      if (j.is_number_unsigned()) return static_cast<int>(j.get<unsigned long long>());
      if (j.is_number_float()) return static_cast<int>(j.get<double>());
      return {};
    };

    // Таблица обработчиков
    const static std::unordered_map<std::string, std::function<std::any()>> handlers = {
        {"string", [&]() { return conv_string(); }},
        {"bool", [&]() { return conv_bool(); }},
        {"int", [&]() { return conv_int(); }},
        {"float", [&]() { return conv_double(); }},
        {"double", [&]() { return conv_double(); }}};

    try {
      auto it = handlers.find(et);
      if (it != handlers.end()) {
        return it->second();
      }
      if (j.is_primitive()) {
        return conv_fallback();
      }
    } catch (const std::exception& e) {
      LOG::Error() << "JsonToAny: conversion error: " << e.what();
      return {};
    }
  }
};

}  // namespace EDD