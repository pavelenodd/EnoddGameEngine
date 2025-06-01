#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class JsonReader {
 private:
  nlohmann::json json_data_;

 public:
  JsonReader() {}
  ~JsonReader() {}
  bool IsLoadJson(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
      return false;
    }
    file >> json_data_;
    return true;
  }
  nlohmann::json GetJsonData() const { return json_data_; }

  nlohmann::json GetJsonData(const std::string& key) const {
    if (json_data_.contains(key)) {
      return json_data_[key];
    }
    return nullptr;
  }
  void ClearJsonData() { json_data_.clear(); }

  void SetJsonData(const nlohmann::json& json_data) { json_data_ = json_data; }
  void SetJsonData(const std::string& key, const nlohmann::json& value) {
    json_data_[key] = value;
  }
};
