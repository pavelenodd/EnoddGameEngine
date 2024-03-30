#include "InputsOutputs/input_output.cpp"
#include "JsonParser/json_parser.cpp"
class Engine {
 private:
  JsonParser parser_;
  Outputs outputs_;

 public:
  Engine() { outputs_.CreateVidget(); }
  ~Engine() {}
};
