// manager_scene.h
#pragma once
#include <GLFW/glfw3.h>

#include <any>
#include <string>
#include <unordered_map>
#include <vector>

#include "EngineData/engine_data.h"
#include "event_type.h"
#include "interface.h"
#include "manager_base.h"

namespace EDD {

#if defined(DEBUG)
namespace Tests {
struct SceneInspector;  // forward declaration
}
#endif

namespace Managers {
using InterfaceKeyEvent = Tools::Interface<Tools::EventTypes::KeyEvent>;
class Scene : public Base, public InterfaceKeyEvent {
#if defined(DEBUG)
  friend struct EDD::Tests::SceneInspector;
#endif

 private:
  std::vector<EDD::Data::Viewport*> viewports_;  // List of created viewports
  inline static bool glfw_initialized_ = false;  // Track if GLFW is initialized
  std::unordered_map<int, bool> key_states_;

 public:
  Scene() = default;
  ~Scene() override = default;

  /**
   * @brief Initialize the scene with the given parameters
   *
   * @param args A vector of any type containing initialization parameters
   */
  void Init(std::vector<std::any> args = {}) override;

  /**
   * @brief Not used in the scene manager
   *
   */
  void Update() override;

  /**
   * @brief Free all resources used by the scene manager
   *
   */
  void FreeResources() override;

  /**
   * @brief Destroy a viewport by its title
   *
   * @param title The title of the viewport to destroy
   */
  void DestroyViewport(const std::string& title);

  /**
   * @brief Destroy all created viewports
   *
   */
  void DestroyAllViewport();

  /**
   * @brief Get the Window Ref object
   *
   * @return EDD::Data::Viewport* or nullptr if no viewport exists
   */
  EDD::Data::Viewport* GetViewportRef(const std::string& title) const;

  /**
   * @brief Get all created viewports
   *
   * @return std::vector<EDD::Data::Viewport*>
   */
  std::vector<EDD::Data::Viewport*> GetAllViewports() const;

  /**
   * @brief Create a Viewport object
   *
   * @param title std::string
   * @param width uint16_t
   * @param height uint16_t
   * @return EDD::Data::Viewport*
   */
  EDD::Data::Viewport* CreateViewport(std::string& title, uint16_t width, uint16_t height);

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

}  // namespace Managers
}  // namespace EDD
