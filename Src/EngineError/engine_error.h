#pragma once
#include <string>
#include <vector>

namespace EDD {
namespace EngineErrorNS {
enum SeverityLevel { INFO = 0, ERROR = 1, CRITICAL = 2 };
std::pair<std::string, int> code0 = {"", 0};
std::pair<std::string, int> code1 = {"Manager initialization error",
                                     CRITICAL};  // manager initialization error

std::pair<std::string, int> code2 = {"", 2};
std::pair<std::string, int> code3 = {"", 3};
std::pair<std::string, int> code4 = {"", 4};
std::pair<std::string, int> code5 = {"", 5};
std::pair<std::string, int> code6 = {"", 6};
std::pair<std::string, int> code7 = {"", 7};
std::pair<std::string, int> code8 = {"", 8};
std::pair<std::string, int> code9 = {"", 9};
}  // namespace EngineErrorNS
// TODO перевести в struct-singleton и добавить логику ограничения
// TODO ...многопоточности, добавить инкапсуляцию логики

std::vector<std::pair<std::string, int>> errors;
}  // namespace EDD