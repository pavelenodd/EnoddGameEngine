#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
class Coord {
 public:
  sf::Vector3f position_;
  sf::Color color_;

 public:
  Coord()
      : position_(0, 0, 0)
      , color_(sf::Color::Red) {}

  Coord(float x, float y, float z = 0,  //
        unsigned char r = 255, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
      : position_(x, y, z)
      , color_(r, g, b, a) {}

  Coord(const Coord& other) = default;
  Coord& operator=(const Coord& other) = default;
  Coord(Coord&& other) = default;
  Coord& operator=(Coord&& other) = default;

  virtual ~Coord() = default;

 public:
  /**
   * @brief Get the Position object
   * @return sf::Vector3f
   */
  sf::Vector3f GetPosition() const {
    return sf::Vector3f(position_.x, position_.y, position_.z);
  }

  /**
   * @brief Set the Position object 
   * @param x,y,z X, Y, Z coordinates
   */
  void SetPosition(float x, float y, float z = 0) {
    position_ = sf::Vector3f(x, y, z);
  }
  /**
 * @brief Set the Color object
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha component (0-255)
 */
  void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
    color_ = sf::Color(r, g, b, a);
  }
};
