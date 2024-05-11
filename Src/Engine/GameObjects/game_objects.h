#include <GL/gl.h>

#include <SFML/Graphics/Image.hpp>
#include <string>
namespace {

/**
 * @brief координаты объекта
 */
struct ObjCoord {
  float x;
  float y;
  float z;
};
/**
 * @brief
 * class GameObject основной класс игровых объектов
 * @note
 * представляет собой базовый игровый объект. недоступен для вызова
 * @param color_        цвет объекта
 * @param image_        изображение принадлежащее объекту
 * @param name_         название объекта
 * @param obj_coord_    координаты объекта
 */
class GameObject {
 private:
 protected:
  sf::Color color_;
  sf::Image image_;
  std::string name_;
  ObjCoord obj_coord_;

 public:
};

}  // end namespace

/**
 * @brief
 * class GameEntity основной класс игровых объектов
 * @note
 * все неуправяемые объекты на игровом поле
 * @param direction_    направление движения
 * @param speed_        скорость движения
 *
 *
 */
class GameEntity : public GameObject {
 private:
 public:
  ObjCoord direction_;
  double speed_;
};
/**
 * @brief
 *  класс GameControllableEntity основной класс упраяемых объектов
 * @note
 * управляемые объекты на игровом поле в т.ч. пешки игрока и компьютера
 * @param is_player_    флаг игрока или компьютера
 */
class GameControllableEntity : public GameEntity {
 private:
  bool is_player_;

 public:
};

/**
 * @brief
 * игровая карта
 */
class GameWorldSpace {
 private:
  /* data */
 public:
};
