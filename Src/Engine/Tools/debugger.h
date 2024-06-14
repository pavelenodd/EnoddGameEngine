#include <fstream>
#include <iostream>
#include <string>
namespace edd {

class Debugger {
 private:
  /* data */
 protected:
  Debugger() {}
  ~Debugger() {}

  /**
   * @brief вывод дебаг информации в косоль
   * @param debug_log текст дебага
   */
  virtual void PrintConsol(std::string debug_log) {
    std::cerr << debug_log << '\n';
  };
  /**
   * @brief вывод дебаг-информации в лог-файл
   * @param debug_log текст дебага
   */
  virtual void PrintLog(std::string debug_log) {};

 public:
};
/**
 * @brief вывод дебаг-информации в лог-файл
 */
class DebugLOG : protected Debugger {
 private:
  /* data */
 public:
  DebugLOG() = default;
  DebugLOG(std::string debug_log) { Debugger::PrintLog(debug_log); }
  ~DebugLOG(){};
};
class DebugConsole : protected Debugger {
 private:
  /* data */
 public:
  DebugConsole() = default;
  DebugConsole(std::string debug_log) { Debugger::PrintConsol(debug_log); }
  ~DebugConsole(){};
};

}  // namespace edd