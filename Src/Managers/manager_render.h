#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <any>
#include <cstdint>
#include <vector>

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
  std::vector<EDD::Data::Viewport*> viewports_ = {};  // список вьюпортов
  EDD::Managers::Entity* entity_manager_ = nullptr;   // менеджер сущностей
  RenderType render_type_ = RenderType::RENDER_2D;    // тип рендера
  bool initialized_ = false;                          // флаг инициализации менеджера

  // Vulkan resources
  VkInstance instance_ = VK_NULL_HANDLE;
  std::vector<VkSurfaceKHR> surfaces_;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;
  std::vector<VkSwapchainKHR> swapchains_;
  std::vector<std::vector<VkImage>> swapchain_images_;
  std::vector<VkFormat> swapchainimage_formats_;
  std::vector<VkExtent2D> swapchain_extents_;
  std::vector<std::vector<VkImageView>> swapchain_image_views_;
  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  std::vector<std::vector<VkFramebuffer>> framebuffers_;
  VkCommandPool command_pool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> command_buffers_;
  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  uint32_t graphics_queue_family_index_ = UINT32_MAX;
  uint32_t present_queue_family_index_ = UINT32_MAX;

  // Graphics pipeline resources
  VkShaderModule vert_shader_module_ = VK_NULL_HANDLE;
  VkShaderModule frag_shader_module_ = VK_NULL_HANDLE;
  VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
  VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;

  // Vertex buffer
  VkBuffer vertex_buffer_ = VK_NULL_HANDLE;
  VkDeviceMemory vertex_buffer_memory_ = VK_NULL_HANDLE;

 private:
  /**
   * @brief Инициализация Vulkan
   *
   * @return bool true если успешно
   */
  bool InitVulkan();

  /**
   * @brief Проверка, подходит ли устройство
   *
   * @param device физическое устройство
   * @return bool true если подходит
   */
  bool IsDeviceSuitable(VkPhysicalDevice device);

  /**
   * @brief Создание экземпляра Vulkan
   *
   * @return bool true если успешно
   */
  inline bool CreateInstance();

  /**
   * @brief Создание поверхности окна
   *
   * @return bool true если успешно
   */
  inline bool CreateSurface();

  /**
   * @brief Выбор физического устройства
   *
   * @return bool true если успешно
   */
  inline bool PickPhysicalDevice();

  /**
   * @brief Поиск семейств очередей
   *
   * @return bool true если успешно
   */
  inline bool FindQueueFamilies();

  /**
   * @brief Создание логического устройства
   *
   * @return bool true если успешно
   */
  inline bool CreateLogicalDevice();

  /**
   * @brief Создание свопчейна
   *
   * @return bool true если успешно
   */
  inline bool CreateSwapchain();

  /**
   * @brief Создание представлений изображений свопчейна
   *
   * @return bool true если успешно
   */
  inline bool CreateImageViews();

  /**
   * @brief Создание прохода рендеринга
   *
   * @return bool true если успешно
   */
  inline bool CreateRenderPass();

  /**
   * @brief Создание фреймбуферов
   *
   * @return bool true если успешно
   */
  inline bool CreateFramebuffers();

  /**
   * @brief Создание пула команд
   *
   * @return bool true если успешно
   */
  inline bool CreateCommandPool();

  /**
   * @brief Создание буферов команд
   *
   * @return bool true если успешно
   */
  inline bool CreateCommandBuffers();

  /**
   * @brief Создание семафоров и заборов
   *
   * @return bool true если успешно
   */
  inline bool CreateSyncObjects();

  /**
   * @brief Создание шейдерных модулей
   *
   * @return bool true если успешно
   */
  inline bool CreateShaderModules();

  /**
   * @brief Создание графического пайплайна
   *
   * @return bool true если успешно
   */
  inline bool CreateGraphicsPipeline();

  /**
   * @brief Создание вершинного буфера
   *
   * @return bool true если успешно
   */
  inline bool CreateVertexBuffer();

  /**
   * @brief Найти подходящий тип памяти
   *
   * @param typeFilter фильтр типов
   * @param properties свойства памяти
   * @return uint32_t индекс типа памяти
   */
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  /**
   * @brief Отрисовка кадра
   *
   */
  void DrawFrame();

  /**
   * @brief Очистка кадра
   *
   */
  void CleanupFrame();

 public:
  Render() = default;
  ~Render() override = default;
  /**
   * @brief Инициализация менеджера рендеринга
   *
   * @param args
   */
  void Init(std::vector<std::any> args) override;
  /**
   * @brief Обновление менеджера рендеринга (отрисовка кадра)
   *
   */
  void Update() override;
  /**
   * @brief Освобождение ресурсов менеджера рендеринга
   *
   */
  void FreeResources() override;
};

}  // namespace Managers
}  // namespace EDD