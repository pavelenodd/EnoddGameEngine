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
#ifdef DEBUG
namespace Tests {
struct SettingsInspector;
}
#endif

enum SettingsType {
  NONE_TYPE = -1,
  ALL_SETTINGS = 0,
  VIEWPORT_SETTINGS = 1,
  AUDIO_SETTINGS = 2,
  GRAPHICS_SETTINGS = 3,
  INPUT_SETTINGS = 4,
  RENDER_SETTINGS = 5

};
/*
  Менеджер настроек отвечает за загрузку и сохранение настроек игры.
      Он может загружать настройки из файла JSON и сохранять их обратно в файл.
      Автоматически проверяет тип данных со схемой, которая содержит тип
  переменной и её диапазон Настройки могут включать параметры окна, аудио и
  графики.
*/
class Settings : public Managers::Base {
#ifdef DEBUG
  friend struct Tests::SettingsInspector;
#endif

 private:
  std::fstream file_stream_;
  std::map<SettingsType, nlohmann::json> settings_map_;
  std::map<SettingsType, std::string> settings_paths_ = {
      {NONE_TYPE, ""},
      {ALL_SETTINGS, ""},
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
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;

 public:
  /**
   * @brief Load settings from a JSON file.
   *
   * @param type The enum type of settings to load.
   * @return bool True if settings were loaded successfully, false otherwise.
   */
  bool LoadSettings(SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot load settings";
      return false;
    }
    std::ifstream ifs(settings_paths_.at(type));
    if (!ifs.is_open()) {
      LOG::Error() << "Failed to open file for read: " << settings_paths_.at(type);
      return false;
    }
    try {
      nlohmann::json parsed = nlohmann::json::parse(ifs, nullptr, true, true);
      settings_map_[type] = std::move(parsed);
      LOG::Debug() << "Settings loaded from " << settings_paths_.at(type);
      return true;
    } catch (const std::exception &e) {
      LOG::Error() << "Parse error: " << e.what();
      return false;
    }
  }
  /**
   * @brief Save settings to a JSON file.
   *
   * @param type The enum type of settings to save.
   * @return bool True if settings were saved successfully, false otherwise.
   */
  bool SaveSettings(SettingsType type = NONE_TYPE) {
    if (type == NONE_TYPE) {
      LOG::Error() << "Settings type is NONE_TYPE, cannot save settings";
      return false;
    }
    auto it = settings_map_.find(type);
    if (it == settings_map_.end()) {
      LOG::Error() << "No settings loaded for type";
      return false;
    }
    std::ofstream ofs(settings_paths_.at(type), std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
      LOG::Error() << "Failed to open file for write: " << settings_paths_.at(type);
      return false;
    }
    ofs << it->second.dump(4) << '\n';
    if (!ofs.good()) {
      LOG::Error() << "Write error: " << settings_paths_.at(type);
      return false;
    }
    LOG::Debug() << "Settings saved to " << settings_paths_.at(type);
    return true;
  }
  /**
   * @brief Set the Value object
   *
   * @param type The enum type of settings to modify.
   * @param key The key of the setting to modify.
   * @param value The new value to set.
   * @return true if the value was set successfully, false otherwise.
   */
  bool SetValue(SettingsType type = SettingsType::NONE_TYPE,
                const std::string &key = "",
                const std::any &value = std::any()) {
    if (type == NONE_TYPE) {
      LOG::Error() << "GetValue: NONE_TYPE";
      return false;
    }
    if (key.empty()) {
      LOG::Error() << "GetValue: key is not bee empty";
      return false;
    }

    auto &root = settings_map_.find(type)->second;
    if (!root.is_object()) {
      LOG::Error() << "GetValue: root json is not object";
      return false;
    }
    if (!root.contains(key)) {
      LOG::Error() << "GetValue: key not found: " << key;
      return false;
    }
    const nlohmann::json &node = root.at(key);
    if (!node.is_object()) {
      LOG::Error(__func__, __LINE__) << " GetValue: node is not object";
      return false;
    }

    if (!node.contains("value")) {
      LOG::Error(__func__, __LINE__) << " GetValue: node is not contains \"value\"";
      return false;
    }
    settings_map_.find(type)->second[key]["value"] = AnyToJson(value, node["type"].get<std::string>());
    return true;
  }
  /**
   * @brief Get the Value object
   *
   * @param type The enum type of settings to retrieve.
   * @param key The key of the setting to retrieve.
   * @return std::any The value of the setting, or an empty std::any if not
   * found.
   */
  std::any GetValue(SettingsType type = SettingsType::NONE_TYPE, const std::string &key = "") {
    if (type == NONE_TYPE) {
      LOG::Error() << "GetValue: NONE_TYPE";
      return {};
    }
    if (key.empty()) {
      LOG::Error() << "GetValue: key is not bee empty";
      return {};
    }
    auto it_type = settings_map_.find(type);
    auto &root = it_type->second;
    if (!root.is_object()) {
      LOG::Error() << "GetValue: root json is not object";
      return {};
    }
    if (!root.contains(key)) {
      LOG::Error() << "GetValue: key not found: " << key;
      return {};
    }
    const nlohmann::json &node = root.at(key);
    if (!node.is_object()) {
      LOG::Error(__func__, __LINE__) << " GetValue: node is not object";
    }

    if (!node.contains("value")) {
      LOG::Error(__func__, __LINE__) << " GetValue: node is not contains \"value\"";
    }

    return JsonToAny(node["value"], node["type"].get<std::string>());
  }

  void Update() override {}
  void Init(std::vector<std::any> args) override {
    LOG::Debug() << "ManagerSettings initialized";
    LoadSettings(SettingsType::ALL_SETTINGS);
  }
  void FreeResources() override {
    SaveSettings(SettingsType::ALL_SETTINGS);
  }

 private:
  /**
   * @brief Convert any value to JSON
   *
   * @param value The value to convert
   * @param expected_type The expected type of the value
   * @return nlohmann::json The JSON representation of the value
   */
  nlohmann::json AnyToJson(const std::any &value, const std::string &expected_type) {
    nlohmann::json j = nullptr;
    if (!value.has_value()) return j;
    const auto &et = expected_type;
    const std::type_info &t = value.type();

    // string
    auto to_string_any = [&](std::string &out) -> bool {
      if (t == typeid(std::string)) {
        out = std::any_cast<const std::string &>(value);
        return true;
      }
      if (t == typeid(const char *)) {
        out = std::string(std::any_cast<const char *>(value));
        return true;
      }
      if (t == typeid(char *)) {
        out = std::string(std::any_cast<char *>(value));
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
    auto to_int_any = [&](long long &out) -> bool {
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
        const auto &s = std::any_cast<const std::string &>(value);
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
    auto to_double_any = [&](double &out) -> bool {
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
        const auto &s = std::any_cast<const std::string &>(value);
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
    auto to_bool_any = [&](bool &out) -> bool {
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
        const auto &s = std::any_cast<const std::string &>(value);
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
  /**
   * @brief Convert JSON to std::any
   *
   * @param j The JSON value to convert
   * @param expected_type The expected type of the value
   * @return std::any The converted value
   */
  std::any JsonToAny(const nlohmann::json &j, const std::string &expected_type) const {
    // TODO
    // !надо переписать тк должен быть стандартный способ использовать To lower в std
    auto to_lower = [](std::string s) {
      for (char &c : s) {
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

    auto conv_string = [&]() -> std::any {
      if (j.is_string()) return j.get<std::string>();
      if (j.is_boolean()) return j.get<bool>() ? std::string("true") : std::string("false");
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
    } catch (const std::exception &e) {
      LOG::Error() << "JsonToAny: conversion error: " << e.what();
      return {};
    }
    return {};
  }
};

}  // namespace EDD