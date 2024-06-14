#pragma once

#include <SFML/Graphics/Image.hpp>
#include <string>

#include "../Render/render_data.h"
#include "../Render/shape.h"

/**
 * @brief координаты объекта
 */
struct ObjCoord {
  double x;
  double y;
  double z;
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
  edd::Color color_ = edd::Colors::Red;
  sf::Image image_;
  std::string name_ = "test shape";
  ObjCoord obj_coord_;
  edd::primitives::Circle shape_;

 public:
  void Draw() { this->shape_.draw(color_, 1); }
};

namespace edd {

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

}  // namespace edd