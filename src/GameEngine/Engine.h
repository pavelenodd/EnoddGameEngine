#pragma onse

#include <string>
#include <vector>

#include "FileManager/Json_Parser/JsonParser.h"

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
    vector<string> cash = json_parser_.Get_Info("windows_size");
    for (string i : cash) {
      windows_size_.height = stoi(i);
      windows_size_.width = stoi(i);
    }
  }

 public:
  GameEngine(){};
  ~GameEngine(){};
};