#pragma once

#include <../../../Libs/nlohmann/json.hpp>
#include <string>
#include <vector>

#include "FileManager/Json_Parser/JsonParser.h"
using json = nlohmann::json;
using namespace std;

struct Windows_Size {
  int height;
  int width;
};

class GameEngine {
 private:
  JsonParser json_parser_;
  Windows_Size windows_size_;

  void Set_WindowsSize() {
    json window_size_json = json_parser_.Get_Info<json>("windows_size")[0];
    windows_size_.height = window_size_json["height"];
    windows_size_.width = window_size_json["width"];
    cout << windows_size_.height << "\n" << windows_size_.width << endl;
  }

 public:
  GameEngine()
      : json_parser_(
            "E:/Projects/CPP_Projects/EnoddGameEngine/src/GameEngine/"
            "SettingsEngine/settings.json") {
    Set_WindowsSize();
  }
  ~GameEngine() {}
};