#include "manager_settings.h"
#include <any>

#include <string>

namespace EDD {
namespace Managers {

bool Settings::LoadSettings(SettingsType type) {
  if (type == NONE_TYPE) {
    LOG::Error() << "Settings type is NONE_TYPE, cannot load settings";
    return false;
  }
  if (type == ALL_SETTINGS) {
    bool all_ok = true;
    for (const auto &[key, value] : settings_paths_) {
      if (key == NONE_TYPE || key == ALL_SETTINGS) continue;
      if (!LoadSettings(key)) {
        all_ok = false;
      }
    }
    return all_ok;
  }
  std::ifstream L_ifs(settings_paths_.at(type));
  if (!L_ifs.is_open()) {
    LOG::Error() << "Failed to open file for read: " << settings_paths_.at(type);
    return false;
  }
  try {
    nlohmann::json L_parsed = nlohmann::json::parse(L_ifs, nullptr, true, true);
    settings_map_[type] = std::move(L_parsed);
    LOG::Debug() << "Settings loaded from " << settings_paths_.at(type);
    return true;
  } catch (const std::exception &L_e) {
    LOG::Error() << "Parse error: " << L_e.what();
    return false;
  }
}
bool Settings::SaveSettings(SettingsType type) {
  if (type == NONE_TYPE) {
    LOG::Error() << "Settings type is NONE_TYPE, cannot save settings";
    return false;
  }
  if (type == ALL_SETTINGS) {
    bool L_all_ok = true;
    for (const auto &[L_key, L_value] : settings_map_) {
      if (!SaveSettings(L_key)) {
        L_all_ok = false;
      }
    }
    return L_all_ok;
  }
  auto L_it = settings_map_.find(type);
  if (L_it == settings_map_.end()) {
    LOG::Error() << "No settings loaded for type";
    return false;
  }
  std::ofstream L_ofs(settings_paths_.at(type), std::ios::out | std::ios::trunc);
  if (!L_ofs.is_open()) {
    LOG::Error() << "Failed to open file for write: " << settings_paths_.at(type);
    return false;
  }
  L_ofs << L_it->second.dump(4) << '\n';
  if (!L_ofs.good()) {
    LOG::Error() << "Write error: " << settings_paths_.at(type);
    return false;
  }
  LOG::Debug() << "Settings saved to " << settings_paths_.at(type);
  return true;
}

bool Settings::SetValue(SettingsType type,
              const std::string &key ,
              const std::any &value) {
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
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << " GetValue: node is not object";
    return false;
  }

  if (!node.contains("value")) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << " GetValue: node is not contains \"value\"";
    return false;
  }
  settings_map_.find(type)->second[key]["value"] = AnyToJson(value,
                                                             node["type"].get<std::string>());
  return true;
}

std::any Settings::GetValue(SettingsType type, const std::string &key) {
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

  const nlohmann::json &node = root.at(key);

  if (!node.is_object()) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << " GetValue: node is not object";
  }

  if (!node.contains("value")) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << " GetValue: node is not contains \"value\"";
  }

  return JsonToAny(node["value"], node["type"].get<std::string>());
}
void Settings::Update()  {}
void Settings::Init(std::vector<std::any> args)  {
  LOG::Debug() << "ManagerSettings initialized";
  LoadSettings(SettingsType::ALL_SETTINGS);
}

void Settings::FreeResources()  {
  SaveSettings(SettingsType::ALL_SETTINGS);
}

nlohmann::json Settings::AnyToJson(const std::any &value, const std::string &expected_type) {
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

std::any Settings::JsonToAny(const nlohmann::json &j, const std::string &expected_type) const  {
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
}  // namespace Managers
}  // namespace EDD