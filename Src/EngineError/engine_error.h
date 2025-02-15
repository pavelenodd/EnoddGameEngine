#pragma once
#include <string>
#include <vector>
namespace EDD {
namespace EngineErrorNS {
std::pair<std::string, int> code0 = {"", 0};
}
// TODO перевести в struct-singleton и добавить логику ограничения
// TODO ...многопоточности, добавить инкапсуляцию логики
enum EngineError { INIT_ERROR = 1, OK = 0 };
std::vector<std::pair<std::string, int>> errors;
}  // namespace EDD