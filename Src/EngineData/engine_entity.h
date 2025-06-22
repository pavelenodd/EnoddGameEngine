#pragma once

#include <vulkan/vulkan.h>

#include <SFML/Graphics.hpp>
#include <vector>

namespace EDD {
namespace Entity {

// Базовый класс-хранилище данных для всех примитивов игровых объектов
class Primitive {
 public:
  sf::Vector3f* position_;
  sf::Color* color_;

 public:
  Primitive() = delete;
  Primitive(const Primitive&) = delete;
  Primitive& operator=(const Primitive&) = delete;
  Primitive& operator=(Primitive&&) = delete;
  Primitive(Primitive&&) = delete;
  Primitive(float x, float y, float z = 0,  //
            unsigned char r, unsigned char g, unsigned char b, unsigned char a)
      : position_(new sf::Vector3f(x, y, z)),
        color_(new sf::Color(r, g, b, a)) {}
  // Запрещаю копирование и перемещение оставляю только создание

  virtual ~Primitive() {
    delete position_;
    delete color_;
  }
};
// Точка объект для 3D/2D-пространства
class Point {
 private:
  Primitive* primitive_;

 public:
  /**
   * @brief Construct a new Point object
   *
   * @param x `x` coordinate
   * @param y `y` coordinate
   * @param z `z` coordinate
   * @param r `red` color
   * @param g `green` color
   * @param b `blue` color
   * @param a `alpha` opacity
   */
  Point(float x, float y, float z = 0,  //
        unsigned char r, unsigned char g, unsigned char b, unsigned char a)
      : primitive_(new Primitive(x, y, z, r, g, b, a)) {}

  ~Point() { delete primitive_; }
};

// Линия объект для 3D/2D-пространства имеет две точки начало и конец
class Line {
 private:
  Primitive* start_;  // Начальная точка
  Primitive* end_;    // Конечная точка
  Primitive* pivot_;  // Опорная точка(центр линии)

 public:
  /**
   * @brief Construct a new Line object
   *
   * @param x1 `x` start coordinate
   * @param y1 `y` start coordinate
   * @param z1 `z` start coordinate
   * @param x2 `x` end coordinate
   * @param y2 `y` end coordinate
   * @param z2 `z` end coordinate
   * @param r1 `red` start color
   * @param g1 `green` start color
   * @param b1 `blue` start color
   * @param a1 `alpha` start opacity
   * @param r2 `red` end color
   * @param g2 `green` end color
   * @param b2 `blue` end color
   * @param a2 `alpha` end opacity
   */
  Line(float x1, float y1, float z1,  //
       float x2, float y2, float z2,  //
       unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1,
       unsigned char r2, unsigned char g2, unsigned char b2, unsigned char a2)
      : start_(new Primitive(x1, y1, z1, r1, g1, b1, a1)),
        end_(new Primitive(x2, y2, z2, r2, g2, b2, a2)),
        pivot_(new Primitive((x1 + x2) / 2, (y1 + y2) / 2, (z1 + z2) / 2,
                             (r1 + r2) / 2, (g1 + g2) / 2, (b1 + b2) / 2,
                             (a1 + a2) / 2)) {}
  ~Line() {
    delete start_;
    delete end_;
  }
};
//
class Triangle {
 private:
  Primitive* vertex1_;  // Вершина 1
  Primitive* vertex2_;  // Вершина 2
  Primitive* vertex3_;  // Вершина 3
  Primitive* pivot_;    // Опорная точка (центр треугольника)

 public:
  Triangle(float x1, float y1, float z1,  //
           float x2, float y2, float z2,  //
           float x3, float y3, float z3,  //
           unsigned char r1, unsigned char g1, unsigned char b1,
           unsigned char a1, unsigned char r2, unsigned char g2,
           unsigned char b2, unsigned char a2, unsigned char r3,
           unsigned char g3, unsigned char b3, unsigned char a3)
      : vertex1_(new Primitive(x1, y1, z1, r1, g1, b1, a1)),
        vertex2_(new Primitive(x2, y2, z2, r2, g2, b2, a2)),
        vertex3_(new Primitive(x3, y3, z3, r3, g3, b3, a3)),
        pivot_(new Primitive((x1 + x2 + x3) / 3, (y1 + y2 + y3) / 3,
                             (z1 + z2 + z3) / 3, (r1 + r2 + r3) / 3,
                             (g1 + g2 + g3) / 3, (b1 + b2 + b3) / 3,
                             (a1 + a2 + a3) / 3)) {}
};

}  // namespace Entity
}  // namespace EDD