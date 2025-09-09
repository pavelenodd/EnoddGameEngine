#pragma once
#include <any>
#include <optional>
#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "EngineData/engine_data.h"
#include "manager_base.h"
#include "manager_entity.h"

namespace Managers {
class Entity;
}

namespace EDD {
namespace Managers {

enum class RenderType { RENDER_2D = 0, RENDER_3D = 1 };

class Render : public Base {
 private:
  std::vector<EDD::Data::Viewport*> viewports_ = {};
  EDD::Managers::Entity* entity_manager_ = nullptr;
  RenderType render_type_ = RenderType::RENDER_2D;

  // Vulkan core
  VkInstance instance_ = VK_NULL_HANDLE;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;

  // Swapchain
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  VkFormat swapchain_image_format_{};
  VkExtent2D swapchain_extent_{};
  std::vector<VkImage> swapchain_images_;
  std::vector<VkImageView> swapchain_image_views_;

  // Render pass / framebuffers
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  std::vector<VkFramebuffer> framebuffers_;

  // Commands
  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> command_buffers_;

  // Sync
  static constexpr uint32_t kMaxFramesInFlight = 2;
  uint32_t current_frame_ = 0;
  std::vector<VkSemaphore> image_available_;
  std::vector<VkSemaphore> render_finished_;
  std::vector<VkFence> in_flight_;

  bool initialized_ = false;

 public:
  Render() = default;
  ~Render() override = default;

  void Init(std::vector<std::any> args) override;
  void Update() override;
  void FreeResources() override;

 private:
  bool InitVulkan();

  // Steps
  bool CreateInstance();
  bool CreateSurface();
  bool PickPhysicalDevice();
  bool CreateLogicalDevice();
  bool CreateSwapchain();
  bool CreateImageViews();
  bool CreateRenderPass();
  bool CreateFramebuffers();
  bool CreateCommandPool();
  bool AllocateCommandBuffers();
  bool RecordAllCommandBuffers();
  bool CreateSyncObjects();

  void DrawFrame();
  void CleanupVulkan();

  // Helpers
  struct QueueFamilies {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;
    bool Complete() const { return graphics.has_value() && present.has_value(); }
  };
  QueueFamilies FindQueueFamilies(VkPhysicalDevice dev);
  struct SwapSupport {
    VkSurfaceCapabilitiesKHR caps{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };
  SwapSupport QuerySwapSupport(VkPhysicalDevice dev);
  VkSurfaceFormatKHR ChooseFormat(const std::vector<VkSurfaceFormatKHR>& fmts);
  VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
  VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& caps, GLFWwindow* wnd);
};

}  // namespace Managers
}  // namespace EDD