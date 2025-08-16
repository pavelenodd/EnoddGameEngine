#pragma once

#include "../EngineData/engine_data.h"
#include "../EngineError/engine_logging.h"
#include "../Tools/interface.h"
#include "Managers/manager_scene.h"

namespace EDD::Tests {

class TestManagerScene {
 private:
  bool is_do_once = false;  // запуск тестов только один раз
  int test_case = -20;  // номер теста, который выполняется
  Managers::Scene *scene_manager_ = nullptr;

 public:
  void RunTests() {
    [[]]switch (test_case) {
      // INFO запуск тестов с проваливанием
      //  все тесты , которые надо выпонить 1 раз заканчиваются на 0
      case 0: {
        TEST_LOG::Info() <<"Test on strarted successfully.";
      }
      case 1: {
      }
      case 2: {
      }
      case 3: {
      }
      case 4: {
      }
      case 5: {
      }
      case 6: {
      }
      case 7: {
      }
      case 8: {
      }
      case 9: {

        break;
      }
      default: {
        TEST_LOG::Success() << "All tests passed successfully.";
        if(is_do_once){
          ++test_case;
        }else{
          test_case = 1;
        }
      }
    }
    //..
    is_do_once = true;
  }

 private:
 /*
 - создать тестовый менеджер сцены
 - проверить, что он инициализируется корректно
  - проверить, что он может добавлять и удалять сцены
  - проверяет ли он наличие активной сцены
  - переключиться на другую сцену
  - закрыть текущую сцену и переключиться на предыдущую
 - передаёт ли он события в менеджер ввода
 - проверяет ли он события клавиатуры
 - проверяет ли он события мыши
 - закрывает ли он сцену и сцены на запрос при закрытии приложения
 - сохраняет ли он состояние сцены при переключении
 - сохраняет ли он состояние сцены при закрытии приложения
 - восстанавливает ли он состояние сцены при открытии приложения
 - проверяет ли он наличие активной сцены при переключении
 */
};

}  // namespace EDD::Tests