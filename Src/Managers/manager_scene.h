#pragma once
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <any>
#include <string>
#include <vector>

#include "EngineData/engine_data.h"
#include "manager_base.h"

namespace EDD {

#if defined(DEBUG)
namespace Tests {
struct SceneInspector;  // forward declaration
}
#endif

namespace Managers {
class Scene : public Base {
#if defined(DEBUG)
  friend struct EDD::Tests::SceneInspector;
#endif

 private:
  std::vector<EDD::Data::Viewport*> viewports_;  // List of created viewports
  inline static bool glfw_initialized_ = false;  // Track if GLFW is initialized

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
};

}  // namespace Managers
}  // namespace EDD
