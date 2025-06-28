#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>

class Coord {
 public:
  sf::Vector3f* position_;
  sf::Color* color_;

 public:
  Coord()
      : position_(new sf::Vector3f(0, 0, 0))
      , color_(new sf::Color(sf::Color::Red)) {}

  Coord(float x, float y, float z = 0,  //
        unsigned char r = 255, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
      : position_(new sf::Vector3f(x, y, z))
      , color_(new sf::Color(r, g, b, a)) {}

  virtual ~Coord() {
    delete position_;
    delete color_;
  }
};
