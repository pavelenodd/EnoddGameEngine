#pragma onse
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "src\Libs\nlohmann_json\include\nlohmann\json.hpp"

using namespace std;
using json = nlohmann::json;

class JsonParser {
 private:
  ifstream curr_file_;
  const string adress_ = "src/GameEngine/SettingsEngine/settings.json";
  // инициализация
  void OpenFile(const string& adress) {
    curr_file_.open(adress);
    if (curr_file_.is_open()) {
      cerr << "Settings file open!" << endl;
    } else {
      cerr << "Settings file Not open!" << endl;
      abort();
    }
  }
  void CloseFile() {
    if (curr_file_.is_open()) {
      curr_file_.close();
    }
  }

  vector<string> ParsingJsonFile(const string& variable_name) {
    json json_file;
    curr_file_ >> json_file;
    vector<string> cash = json_file[variable_name];
  }

 public:
  JsonParser() { OpenFile(adress_); }
  ~JsonParser() { CloseFile(); }

  // функции получения данных из файла
  vector<string> Get_Info(const string& variable_name) {
    return ParsingJsonFile(variable_name);
  }
};
