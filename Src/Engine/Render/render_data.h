#pragma once
#include <GL/gl.h>
#include <math.h>

namespace edd {

struct Color {
  double red_ = 100.0;
  double green_ = 100.0;
  double blue_ = 100.0;
  double alpha_ = 100.0;

  Color() = default;
  Color(double L_red,
        double L_green,  //
        double L_blue,   //
        double L_alpha = 255.0) {
    red_ = L_red;
    green_ = L_green;
    blue_ = L_blue;
    alpha_ = L_alpha;
  }
  struct Color Set_Color(double L_red,    //
                         double L_green,  //
                         double L_blue,   //
                         double L_alpha = 255) {
    red_ = L_red;
    green_ = L_green;
    blue_ = L_blue;
    alpha_ = L_alpha;
    return *this;
  };
};
/**
 * Статические цвета
 */
namespace Colors {
static const Color Black(0.0, 0.0, 0.0);
static const Color White(255.0, 255.0, 255.0);
static const Color Red(255.0, 0.0, 0.0);
static const Color Green(0.0, 255.0, 0.0);
static const Color Blue(0.0, 0.0, 255.0);
static const Color Yellow(255.0, 255.0, 0.0);
static const Color Grey(200.0, 200.0, 200.0);

};  // namespace Colors

};  // namespace edd