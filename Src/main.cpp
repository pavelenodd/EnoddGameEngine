#include <GLFW/glfw3.h>
#include "Engine/engine.h"

int main() {
  Engine engine;
  engine.Init();
  engine.Start();
  engine.Stop();
}