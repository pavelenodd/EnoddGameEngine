#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

#include "InputsOutputs/input_output.cpp"
#include "JsonParser/json_parser.cpp"
class Engine {
 private:
  JsonParser parser_;
  Outputs outputs_;
  sf::Font font_;

  void Set_Font(const std::string &L_font_adress = "Data/Fonts/font.ttf") {}

 public:
  Engine() {
    Set_Font();

    outputs_.CreateViewportWindow();
  }
  ~Engine() {}
};
