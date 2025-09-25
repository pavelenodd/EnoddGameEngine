#include "manager_render.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include "EngineError/engine_logging.h"

struct Vertex {
  //! Удалить нужен для отладки
  glm::vec2 pos;
  glm::vec3 color;

  static VkVertexInputBindingDescription GetBindingDescription() {
    VkVertexInputBindingDescription L_binding_description{};
    L_binding_description.binding = 0;
    L_binding_description.stride = sizeof(Vertex);
    L_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return L_binding_description;
  }

  static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> L_attributeDescriptions{};

    L_attributeDescriptions[0].binding = 0;
    L_attributeDescriptions[0].location = 0;
    L_attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    L_attributeDescriptions[0].offset = offsetof(Vertex, pos);

    L_attributeDescriptions[1].binding = 0;
    L_attributeDescriptions[1].location = 1;
    L_attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    L_attributeDescriptions[1].offset = offsetof(Vertex, color);

    return L_attributeDescriptions;
  }
};

namespace EDD::Managers {

void Render::Init(std::vector<std::any> args) {
  LOG::Debug() << "Render::Init called";
  if (args.size() < 3) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Render::Init - insufficient parameters";
    return;
  }
  try {
    viewports_ = std::any_cast<std::vector<EDD::Data::Viewport*>>(args[0]);
    entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
    render_type_ = std::any_cast<RenderType>(args[2]);
  } catch (...) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Render::Init - bad any_cast";
    return;
  }
  if (viewports_.empty() || !viewports_[0] || !viewports_[0]->viewport_window) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Render::Init - invalid viewport";
    return;
  }
  if (!InitVulkan()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Render::Init - Vulkan init failed";
    return;
  }
  initialized_ = true;
  LOG::Debug() << "Render::Init OK";
}

void Render::Update() {
  if (!initialized_) return;
  DrawFrame();
  // возможно нужно что-то почистить после отрисовки кадра
  CleanupFrame();
}

// <- [WARNING]
//       высокая сложность освобождения ресурсов, отсутствует обработка ошибок при cleanup,
//       порядок освобождения критичен для Vulkan, рассмотреть RAII подход
void Render::FreeResources() {
  if (!initialized_) return;
  for (auto fence : in_flight_fences_) {
    vkDestroyFence(device_, fence, nullptr);
  }
  for (auto semaphore : render_finished_semaphores_) {
    vkDestroySemaphore(device_, semaphore, nullptr);
  }
  for (auto semaphore : image_available_semaphores_) {
    vkDestroySemaphore(device_, semaphore, nullptr);
  }
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  for (size_t i = 0; i < framebuffers_.size(); ++i) {
    for (auto framebuffer : framebuffers_[i]) {
      vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
  }
  vkDestroyRenderPass(device_, render_pass_, nullptr);
  for (size_t i = 0; i < swapchain_image_views_.size(); ++i) {
    for (auto imageView : swapchain_image_views_[i]) {
      vkDestroyImageView(device_, imageView, nullptr);
    }
  }
  for (auto swapchain : swapchains_) {
    vkDestroySwapchainKHR(device_, swapchain, nullptr);
  }
  for (auto view : multisample_image_views_) {
    vkDestroyImageView(device_, view, nullptr);
  }
  for (auto image : multisample_images_) {
    vkDestroyImage(device_, image, nullptr);
  }
  for (auto memory : multisample_image_memories_) {
    vkFreeMemory(device_, memory, nullptr);
  }
  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
  vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  vkDestroyShaderModule(device_, frag_shader_module_, nullptr);
  vkDestroyShaderModule(device_, vert_shader_module_, nullptr);
  vkDestroyDevice(device_, nullptr);
  for (auto surface : surfaces_) {
    vkDestroySurfaceKHR(instance_, surface, nullptr);
  }
  vkDestroyInstance(instance_, nullptr);
  initialized_ = false;
}

// <- [WARNING]
//       высокая сложность инициализации, метод выполняет более 12 различных задач,
//       нарушение принципа единственной ответственности, рассмотреть разделение
bool Render::InitVulkan() {
  if (!CreateInstance()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create Vulkan instance";
    return false;
  }
  if (!CreateSurface()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create Vulkan surface";
    return false;
  }
  if (!PickPhysicalDevice()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to pick physical device";
    return false;
  }
  if (!FindQueueFamilies()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to find queue families";
    return false;
  }
  if (!CreateLogicalDevice()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create logical device";
    return false;
  }
  if (!CreateSwapchain()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create swapchain";
    return false;
  }
  if (!CreateImageViews()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create image views";
    return false;
  }
  if (!CreateRenderPass()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create render pass";
    return false;
  }
  if (!CreateFramebuffers()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create framebuffers";
    return false;
  }
  if (!CreateCommandPool()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create command pool";
    return false;
  }
  if (!CreateCommandBuffers()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create command buffers";
    return false;
  }
  if (!CreateSyncObjects()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create sync objects";
    return false;
  }
  if (!CreateShaderModules()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create shader modules";
    return false;
  }
  if (!CreateGraphicsPipeline()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create graphics pipeline";
    return false;
  }
  if (!CreateVertexBuffer()) {
    LOG::Fatal(__PRETTY_FUNCTION__, __LINE__) << "Failed to create vertex buffer";
    return false;
  }

  LOG::Debug(__PRETTY_FUNCTION__) << "Vulkan initialized successfully";
  return true;
}

bool Render::IsDeviceSuitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties L_deviceProperties;
  vkGetPhysicalDeviceProperties(device, &L_deviceProperties);
  LOG::Debug(__PRETTY_FUNCTION__) << "Checking device: " << L_deviceProperties.deviceName
                                  << " Type: " << L_deviceProperties.deviceType;

  // Check for required extensions
  uint32_t L_extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &L_extensionCount, nullptr);
  std::vector<VkExtensionProperties> L_availableExtensions(L_extensionCount);
  vkEnumerateDeviceExtensionProperties(
      device, nullptr, &L_extensionCount, L_availableExtensions.data());

  bool L_swapchainSupported = false;
  for (const auto& extension : L_availableExtensions) {
    if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
      L_swapchainSupported = true;
      break;
    }
  }

  if (!L_swapchainSupported) {
    LOG::Debug(__PRETTY_FUNCTION__)
        << "Device " << L_deviceProperties.deviceName << " does not support VK_KHR_swapchain";
    return false;
  }

  return L_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
         L_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
         L_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
}

inline bool Render::CreateInstance() {
  uint32_t L_glfwExtensionCount = 0;
  const char** L_glfwExtensions = glfwGetRequiredInstanceExtensions(&L_glfwExtensionCount);

  VkInstanceCreateInfo L_createInfo{};
  L_createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  L_createInfo.enabledExtensionCount = L_glfwExtensionCount;
  L_createInfo.ppEnabledExtensionNames = L_glfwExtensions;
  L_createInfo.enabledLayerCount = 0;

  if (vkCreateInstance(&L_createInfo, nullptr, &instance_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create Vulkan instance";
    return false;
  }
  return true;
}

inline bool Render::CreateSurface() {
  surfaces_.resize(viewports_.size());
  for (size_t i = 0; i < viewports_.size(); ++i) {
    if (glfwCreateWindowSurface(
            instance_, viewports_[i]->viewport_window, nullptr, &surfaces_[i]) != VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create window surface for viewport " << i;
      return false;
    }
  }
  return true;
}

inline bool Render::PickPhysicalDevice() {
  uint32_t L_deviceCount = 0;
  vkEnumeratePhysicalDevices(instance_, &L_deviceCount, nullptr);
  if (L_deviceCount == 0) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "No Vulkan-compatible GPUs found";
    return false;
  }

  LOG::Debug(__PRETTY_FUNCTION__) << "Found " << L_deviceCount << " Vulkan devices";

  std::vector<VkPhysicalDevice> L_devices(L_deviceCount);
  vkEnumeratePhysicalDevices(instance_, &L_deviceCount, L_devices.data());

  for (const auto& device : L_devices) {
    if (IsDeviceSuitable(device)) {
      physical_device_ = device;
      VkPhysicalDeviceProperties L_props;
      vkGetPhysicalDeviceProperties(device, &L_props);
      LOG::Debug(__PRETTY_FUNCTION__) << "Selected suitable device: " << L_props.deviceName;
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE && !L_devices.empty()) {
    physical_device_ = L_devices[0];
    VkPhysicalDeviceProperties L_props;
    vkGetPhysicalDeviceProperties(physical_device_, &L_props);
    LOG::Debug(__PRETTY_FUNCTION__)
        << "No suitable device found, using first available: " << L_props.deviceName;
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "No Vulkan devices available";
    return false;
  }
  return true;
}

inline bool Render::FindQueueFamilies() {
  uint32_t L_queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &L_queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> L_queueFamilies(L_queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      physical_device_, &L_queueFamilyCount, L_queueFamilies.data());

  for (uint32_t i = 0; i < L_queueFamilyCount; ++i) {
    if (L_queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_queue_family_index_ = i;
    }
    VkBool32 L_presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, i, surfaces_[0], &L_presentSupport);
    if (L_presentSupport) {
      present_queue_family_index_ = i;
    }
    if (graphics_queue_family_index_ != UINT32_MAX &&
        present_queue_family_index_ != UINT32_MAX)
      break;
  }

  if (graphics_queue_family_index_ == UINT32_MAX ||
      present_queue_family_index_ == UINT32_MAX) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Required queue families not found";
    return false;
  }
  return true;
}

inline bool Render::CreateLogicalDevice() {
  std::vector<VkDeviceQueueCreateInfo> L_queueCreateInfos;
  std::set<uint32_t> L_uniqueQueueFamilies = {graphics_queue_family_index_,
                                              present_queue_family_index_};
  float L_queuePriority = 1.0f;
  for (uint32_t queueFamily : L_uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo L_queueCreateInfo{};
    L_queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    L_queueCreateInfo.queueFamilyIndex = queueFamily;
    L_queueCreateInfo.queueCount = 1;
    L_queueCreateInfo.pQueuePriorities = &L_queuePriority;
    L_queueCreateInfos.push_back(L_queueCreateInfo);
  }

  VkPhysicalDeviceFeatures L_deviceFeatures{};
  const char* L_deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  VkDeviceCreateInfo L_deviceCreateInfo{};
  L_deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  L_deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(L_queueCreateInfos.size());
  L_deviceCreateInfo.pQueueCreateInfos = L_queueCreateInfos.data();
  L_deviceCreateInfo.pEnabledFeatures = &L_deviceFeatures;
  L_deviceCreateInfo.enabledExtensionCount = 1;
  L_deviceCreateInfo.ppEnabledExtensionNames = L_deviceExtensions;

  if (vkCreateDevice(physical_device_, &L_deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create logical device";
    return false;
  }

  vkGetDeviceQueue(device_, graphics_queue_family_index_, 0, &graphics_queue_);
  vkGetDeviceQueue(device_, present_queue_family_index_, 0, &present_queue_);
  return true;
}

// <- [WARNING]
//       высокая цикломатическая сложность (>96 строк), обрабатывает множество Vulkan структур,
//       алгоритмическая сложность O(n*m) где n - количество поверхностей, m - количество
//       форматов
inline bool Render::CreateSwapchain() {
  swapchains_.resize(surfaces_.size());
  swapchain_images_.resize(surfaces_.size());
  swapchainimage_formats_.resize(surfaces_.size());
  swapchain_extents_.resize(surfaces_.size());

  for (size_t idx = 0; idx < surfaces_.size(); ++idx) {
    VkSurfaceCapabilitiesKHR L_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device_, surfaces_[idx], &L_capabilities);

    uint32_t L_formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device_, surfaces_[idx], &L_formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> L_formats(L_formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device_, surfaces_[idx], &L_formatCount, L_formats.data());

    VkSurfaceFormatKHR L_surfaceFormat = L_formats[0];
    for (const auto& availableFormat : L_formats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        L_surfaceFormat = availableFormat;
        break;
      }
    }

    uint32_t L_presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device_, surfaces_[idx], &L_presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> L_presentModes(L_presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device_, surfaces_[idx], &L_presentModeCount, L_presentModes.data());

    VkPresentModeKHR L_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& availablePresentMode : L_presentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        L_presentMode = availablePresentMode;
        break;
      }
    }

    VkExtent2D L_extent = L_capabilities.currentExtent;
    if (L_capabilities.currentExtent.width != UINT32_MAX) {
      L_extent = L_capabilities.currentExtent;
    } else {
      int L_width, L_height;
      glfwGetFramebufferSize(viewports_[idx]->viewport_window, &L_width, &L_height);
      L_extent.width = std::clamp(static_cast<uint32_t>(L_width),
                                  L_capabilities.minImageExtent.width,
                                  L_capabilities.maxImageExtent.width);
      L_extent.height = std::clamp(static_cast<uint32_t>(L_height),
                                   L_capabilities.minImageExtent.height,
                                   L_capabilities.maxImageExtent.height);
    }

    uint32_t L_imageCount = L_capabilities.minImageCount + 1;
    if (L_capabilities.maxImageCount > 0 && L_imageCount > L_capabilities.maxImageCount) {
      L_imageCount = L_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR L_swapchainCreateInfo{};
    L_swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    L_swapchainCreateInfo.surface = surfaces_[idx];
    L_swapchainCreateInfo.minImageCount = L_imageCount;
    L_swapchainCreateInfo.imageFormat = L_surfaceFormat.format;
    L_swapchainCreateInfo.imageColorSpace = L_surfaceFormat.colorSpace;
    L_swapchainCreateInfo.imageExtent = L_extent;
    L_swapchainCreateInfo.imageArrayLayers = 1;
    L_swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t L_queueFamilyIndices[] = {graphics_queue_family_index_,
                                       present_queue_family_index_};
    if (graphics_queue_family_index_ != present_queue_family_index_) {
      L_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      L_swapchainCreateInfo.queueFamilyIndexCount = 2;
      L_swapchainCreateInfo.pQueueFamilyIndices = L_queueFamilyIndices;
    } else {
      L_swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      L_swapchainCreateInfo.queueFamilyIndexCount = 0;
      L_swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    L_swapchainCreateInfo.preTransform = L_capabilities.currentTransform;
    L_swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    L_swapchainCreateInfo.presentMode = L_presentMode;
    L_swapchainCreateInfo.clipped = VK_TRUE;
    L_swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device_, &L_swapchainCreateInfo, nullptr, &swapchains_[idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create swapchain for viewport " << idx;
      return false;
    }

    vkGetSwapchainImagesKHR(device_, swapchains_[idx], &L_imageCount, nullptr);
    swapchain_images_[idx].resize(L_imageCount);
    vkGetSwapchainImagesKHR(
        device_, swapchains_[idx], &L_imageCount, swapchain_images_[idx].data());

    swapchainimage_formats_[idx] = L_surfaceFormat.format;
    swapchain_extents_[idx] = L_extent;
  }
  return true;
}

inline bool Render::CreateImageViews() {
  // <- [WARNING]
  // высокая цикломатическая сложность, функция обрабатывает несколько ресурсов в
  // циклах, рассмотреть разделение на подфункции
  swapchain_image_views_.resize(swapchains_.size());
  multisample_images_.resize(surfaces_.size());
  multisample_image_memories_.resize(surfaces_.size());
  multisample_image_views_.resize(surfaces_.size());

  for (size_t idx = 0; idx < swapchains_.size(); ++idx) {
    swapchain_image_views_[idx].resize(swapchain_images_[idx].size());
    for (size_t i = 0; i < swapchain_images_[idx].size(); ++i) {
      VkImageViewCreateInfo L_viewCreateInfo{};
      L_viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      L_viewCreateInfo.image = swapchain_images_[idx][i];
      L_viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      L_viewCreateInfo.format = swapchainimage_formats_[idx];
      L_viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      L_viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      L_viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      L_viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      L_viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      L_viewCreateInfo.subresourceRange.baseMipLevel = 0;
      L_viewCreateInfo.subresourceRange.levelCount = 1;
      L_viewCreateInfo.subresourceRange.baseArrayLayer = 0;
      L_viewCreateInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(
              device_, &L_viewCreateInfo, nullptr, &swapchain_image_views_[idx][i]) !=
          VK_SUCCESS) {
        LOG::Error(__PRETTY_FUNCTION__, __LINE__)
            << "Failed to create image view for viewport " << idx << " image " << i;
        return false;
      }
    }

    // Create multisample image
    VkImageCreateInfo L_imageCreateInfo{};
    L_imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    L_imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    L_imageCreateInfo.extent.width = swapchain_extents_[idx].width;
    L_imageCreateInfo.extent.height = swapchain_extents_[idx].height;
    L_imageCreateInfo.extent.depth = 1;
    L_imageCreateInfo.mipLevels = 1;
    L_imageCreateInfo.arrayLayers = 1;
    L_imageCreateInfo.format = swapchainimage_formats_[idx];
    L_imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    L_imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    L_imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    L_imageCreateInfo.samples = VK_SAMPLE_COUNT_4_BIT;
    L_imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device_, &L_imageCreateInfo, nullptr, &multisample_images_[idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create multisample image for viewport " << idx;
      return false;
    }

    VkMemoryRequirements L_memRequirements;
    vkGetImageMemoryRequirements(device_, multisample_images_[idx], &L_memRequirements);

    VkMemoryAllocateInfo L_allocInfo{};
    L_allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    L_allocInfo.allocationSize = L_memRequirements.size;
    L_allocInfo.memoryTypeIndex = FindMemoryType(L_memRequirements.memoryTypeBits,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device_, &L_allocInfo, nullptr, &multisample_image_memories_[idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to allocate multisample image memory for viewport " << idx;
      return false;
    }

    vkBindImageMemory(device_, multisample_images_[idx], multisample_image_memories_[idx], 0);

    VkImageViewCreateInfo L_msViewCreateInfo{};
    L_msViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    L_msViewCreateInfo.image = multisample_images_[idx];
    L_msViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    L_msViewCreateInfo.format = swapchainimage_formats_[idx];
    L_msViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    L_msViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    L_msViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    L_msViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    L_msViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    L_msViewCreateInfo.subresourceRange.baseMipLevel = 0;
    L_msViewCreateInfo.subresourceRange.levelCount = 1;
    L_msViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    L_msViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
            device_, &L_msViewCreateInfo, nullptr, &multisample_image_views_[idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create multisample image view for viewport " << idx;
      return false;
    }
  }
  return true;
}

inline bool Render::CreateRenderPass() {
  VkAttachmentDescription L_colorAttachment{};
  L_colorAttachment.format = swapchainimage_formats_[0];  // assume all same
  L_colorAttachment.samples = VK_SAMPLE_COUNT_4_BIT;      // MSAA samples
  L_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  L_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  L_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  L_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  L_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  L_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription L_resolveAttachment{};
  L_resolveAttachment.format = swapchainimage_formats_[0];
  L_resolveAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  L_resolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  L_resolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  L_resolveAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  L_resolveAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  L_resolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  L_resolveAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription L_attachments[2] = {L_colorAttachment, L_resolveAttachment};

  VkAttachmentReference L_colorAttachmentRef{};
  L_colorAttachmentRef.attachment = 0;
  L_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference L_resolveAttachmentRef{};
  L_resolveAttachmentRef.attachment = 1;
  L_resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription L_subpass{};
  L_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  L_subpass.colorAttachmentCount = 1;
  L_subpass.pColorAttachments = &L_colorAttachmentRef;
  L_subpass.pResolveAttachments = &L_resolveAttachmentRef;

  VkRenderPassCreateInfo L_renderPassInfo{};
  L_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  L_renderPassInfo.attachmentCount = 2;
  L_renderPassInfo.pAttachments = L_attachments;
  L_renderPassInfo.subpassCount = 1;
  L_renderPassInfo.pSubpasses = &L_subpass;

  if (vkCreateRenderPass(device_, &L_renderPassInfo, nullptr, &render_pass_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create render pass";
    return false;
  }
  return true;
}

inline bool Render::CreateFramebuffers() {
  framebuffers_.resize(swapchains_.size());
  for (size_t idx = 0; idx < swapchains_.size(); ++idx) {
    framebuffers_[idx].resize(swapchain_image_views_[idx].size());
    for (size_t i = 0; i < swapchain_image_views_[idx].size(); ++i) {
      VkImageView L_attachments[] = {multisample_image_views_[idx],
                                     swapchain_image_views_[idx][i]};

      VkFramebufferCreateInfo L_framebufferInfo{};
      L_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      L_framebufferInfo.renderPass = render_pass_;
      L_framebufferInfo.attachmentCount = 2;
      L_framebufferInfo.pAttachments = L_attachments;
      L_framebufferInfo.width = swapchain_extents_[idx].width;
      L_framebufferInfo.height = swapchain_extents_[idx].height;
      L_framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(device_, &L_framebufferInfo, nullptr, &framebuffers_[idx][i]) !=
          VK_SUCCESS) {
        LOG::Error(__PRETTY_FUNCTION__, __LINE__)
            << "Failed to create framebuffer for viewport " << idx << " image " << i;
        return false;
      }
    }
  }
  return true;
}

inline bool Render::CreateCommandPool() {
  VkCommandPoolCreateInfo L_poolInfo{};
  L_poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  L_poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  L_poolInfo.queueFamilyIndex = graphics_queue_family_index_;

  if (vkCreateCommandPool(device_, &L_poolInfo, nullptr, &command_pool_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create command pool";
    return false;
  }
  return true;
}

inline bool Render::CreateCommandBuffers() {
  command_buffers_.resize(swapchains_.size());

  VkCommandBufferAllocateInfo L_allocInfo{};
  L_allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  L_allocInfo.commandPool = command_pool_;
  L_allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  L_allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

  if (vkAllocateCommandBuffers(device_, &L_allocInfo, command_buffers_.data()) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to allocate command buffers";
    return false;
  }
  return true;
}

inline bool Render::CreateSyncObjects() {
  image_available_semaphores_.resize(swapchains_.size());
  render_finished_semaphores_.resize(swapchains_.size());
  in_flight_fences_.resize(swapchains_.size());

  VkSemaphoreCreateInfo L_semaphoreInfo{};
  L_semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo L_fenceInfo{};
  L_fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  L_fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < swapchains_.size(); ++i) {
    if (vkCreateSemaphore(
            device_, &L_semaphoreInfo, nullptr, &image_available_semaphores_[i]) !=
            VK_SUCCESS ||
        vkCreateSemaphore(
            device_, &L_semaphoreInfo, nullptr, &render_finished_semaphores_[i]) !=
            VK_SUCCESS ||
        vkCreateFence(device_, &L_fenceInfo, nullptr, &in_flight_fences_[i]) != VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create sync objects for viewport " << i;
      return false;
    }
  }
  return true;
}

void Render::DrawFrame() {
  if (!initialized_) return;

  for (size_t idx = 0; idx < viewports_.size(); ++idx) {
    RenderScreen(idx);
  }
}

// <- [WARNING]
//       отсутствует обработка ошибок Vulkan API (vkAcquireNextImageKHR может вернуть
//       VK_ERROR_*), потенциальные блокировки при UINT64_MAX таймауте, высокая сложность
//       метода (80+ строк)
void Render::RenderScreen(size_t viewport_index) {
  // Ждем завершения предыдущего кадра для этого viewport
  vkWaitForFences(device_, 1, &in_flight_fences_[viewport_index], VK_TRUE, UINT64_MAX);

  // Сбрасываем fence для повторного использования
  vkResetFences(device_, 1, &in_flight_fences_[viewport_index]);

  // Получаем следующий доступный image из swapchain
  uint32_t L_imageIndex;
  vkAcquireNextImageKHR(device_,
                        swapchains_[viewport_index],
                        UINT64_MAX,
                        image_available_semaphores_[viewport_index],
                        VK_NULL_HANDLE,
                        &L_imageIndex);

  // Сбрасываем command buffer для записи новых команд
  vkResetCommandBuffer(command_buffers_[viewport_index], 0);

  // Начинаем запись команд в command buffer
  VkCommandBufferBeginInfo L_beginInfo{};
  L_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(command_buffers_[viewport_index], &L_beginInfo) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__)
        << "Failed to begin recording command buffer for viewport " << viewport_index;
    return;
  }

  // Начинаем render pass для рисования на framebuffer
  VkRenderPassBeginInfo L_renderPassInfo{};
  L_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  L_renderPassInfo.renderPass = render_pass_;
  L_renderPassInfo.framebuffer = framebuffers_[viewport_index][L_imageIndex];
  L_renderPassInfo.renderArea.offset = {0, 0};
  L_renderPassInfo.renderArea.extent = swapchain_extents_[viewport_index];

  VkClearValue L_clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};  // Black color
  L_renderPassInfo.clearValueCount = 1;
  L_renderPassInfo.pClearValues = &L_clearColor;

  vkCmdBeginRenderPass(
      command_buffers_[viewport_index], &L_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // Рисуем все сущности для этого viewport
  RenderEntities(command_buffers_[viewport_index], viewport_index);

  // Заканчиваем render pass
  vkCmdEndRenderPass(command_buffers_[viewport_index]);

  // Заканчиваем запись command buffer
  if (vkEndCommandBuffer(command_buffers_[viewport_index]) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__)
        << "Failed to record command buffer for viewport " << viewport_index;
    return;
  }

  // Отправляем command buffer в очередь выполнения
  VkSubmitInfo L_submitInfo{};
  L_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore L_waitSemaphores[] = {image_available_semaphores_[viewport_index]};
  VkPipelineStageFlags L_waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  L_submitInfo.waitSemaphoreCount = 1;
  L_submitInfo.pWaitSemaphores = L_waitSemaphores;
  L_submitInfo.pWaitDstStageMask = L_waitStages;

  L_submitInfo.commandBufferCount = 1;
  L_submitInfo.pCommandBuffers = &command_buffers_[viewport_index];

  VkSemaphore L_signalSemaphores[] = {render_finished_semaphores_[viewport_index]};
  L_submitInfo.signalSemaphoreCount = 1;
  L_submitInfo.pSignalSemaphores = L_signalSemaphores;

  if (vkQueueSubmit(graphics_queue_, 1, &L_submitInfo, in_flight_fences_[viewport_index]) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__)
        << "Failed to submit draw command buffer for viewport " << viewport_index;
    return;
  }

  // Представляем готовый image на экран
  VkPresentInfoKHR L_presentInfo{};
  L_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  L_presentInfo.waitSemaphoreCount = 1;
  L_presentInfo.pWaitSemaphores = L_signalSemaphores;

  VkSwapchainKHR L_swapChains[] = {swapchains_[viewport_index]};
  L_presentInfo.swapchainCount = 1;
  L_presentInfo.pSwapchains = L_swapChains;

  L_presentInfo.pImageIndices = &L_imageIndex;

  vkQueuePresentKHR(present_queue_, &L_presentInfo);
}

void Render::RenderEntities(VkCommandBuffer command_buffer, size_t viewport_index) {
  if (!entity_manager_->GetAllEntities().empty()) {
    auto L_view = entity_manager_->GetEntitiesByCoord();
    for (auto entity : L_view) {
      auto& L_coord = L_view.get<Coord>(entity);
      // For simplicity, render a square with the entity's color
      // Note: Position is not used yet, just color

      VkViewport L_viewport{};
      L_viewport.x = 0.0f;
      L_viewport.y = 0.0f;
      L_viewport.width = static_cast<float>(swapchain_extents_[viewport_index].width);
      L_viewport.height = static_cast<float>(swapchain_extents_[viewport_index].height);
      L_viewport.minDepth = 0.0f;
      L_viewport.maxDepth = 1.0f;
      vkCmdSetViewport(command_buffer, 0, 1, &L_viewport);

      VkRect2D L_scissor{};
      L_scissor.offset = {0, 0};
      L_scissor.extent = swapchain_extents_[viewport_index];
      vkCmdSetScissor(command_buffer, 0, 1, &L_scissor);

      vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

      VkBuffer L_vertexBuffers[] = {vertex_buffer_};
      VkDeviceSize L_offsets[] = {0};
      vkCmdBindVertexBuffers(command_buffer, 0, 1, L_vertexBuffers, L_offsets);

      // Push color from coord
      glm::vec3 L_color(
          L_coord.color_.r / 255.0f, L_coord.color_.g / 255.0f, L_coord.color_.b / 255.0f);
      vkCmdPushConstants(command_buffer,
                         pipeline_layout_,
                         VK_SHADER_STAGE_FRAGMENT_BIT,
                         0,
                         sizeof(glm::vec3),
                         &L_color);

      vkCmdDraw(command_buffer, 4, 1, 0, 0);
    }
  }
}

inline bool Render::CreateShaderModules() {
  std::vector<uint32_t> L_vertShaderCode;
  std::vector<uint32_t> L_fragShaderCode;

  VkShaderModuleCreateInfo L_createInfo{};
  L_createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  L_createInfo.codeSize = L_vertShaderCode.size();
  L_createInfo.pCode = reinterpret_cast<const uint32_t*>(L_vertShaderCode.data());

  if (vkCreateShaderModule(device_, &L_createInfo, nullptr, &vert_shader_module_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create vertex shader module";
    return false;
  }

  L_createInfo.codeSize = L_fragShaderCode.size();
  L_createInfo.pCode = reinterpret_cast<const uint32_t*>(L_fragShaderCode.data());

  if (vkCreateShaderModule(device_, &L_createInfo, nullptr, &frag_shader_module_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create fragment shader module";
    return false;
  }

  return true;
}

// <- [WARNING]
//       чрезмерная длина метода (125+ строк), высокая сложность настройки пайплайна,
//       нарушение принципа единственной ответственности, разделить на подметоды
inline bool Render::CreateGraphicsPipeline() {
  VkPipelineShaderStageCreateInfo L_vertShaderStageInfo{};
  L_vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  L_vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  L_vertShaderStageInfo.module = vert_shader_module_;
  L_vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo L_fragShaderStageInfo{};
  L_fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  L_fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  L_fragShaderStageInfo.module = frag_shader_module_;
  L_fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo L_shaderStages[] = {L_vertShaderStageInfo,
                                                      L_fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo L_vertexInputInfo{};
  L_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  auto L_bindingDescription = Vertex::GetBindingDescription();
  auto L_attributeDescriptions = Vertex::GetAttributeDescriptions();

  L_vertexInputInfo.vertexBindingDescriptionCount = 1;
  L_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
      L_attributeDescriptions.size());
  L_vertexInputInfo.pVertexBindingDescriptions = &L_bindingDescription;
  L_vertexInputInfo.pVertexAttributeDescriptions = L_attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo L_inputAssembly{};
  L_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  L_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  L_inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo L_viewportState{};
  L_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  L_viewportState.viewportCount = 1;
  L_viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo L_rasterizer{};
  L_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  L_rasterizer.depthClampEnable = VK_FALSE;
  L_rasterizer.rasterizerDiscardEnable = VK_FALSE;
  L_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  L_rasterizer.lineWidth = 1.0f;
  L_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  L_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  L_rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo L_multisampling{};
  L_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  L_multisampling.sampleShadingEnable = VK_FALSE;
  L_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;

  VkPipelineColorBlendAttachmentState L_colorBlendAttachment{};
  L_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  L_colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo L_colorBlending{};
  L_colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  L_colorBlending.logicOpEnable = VK_FALSE;
  L_colorBlending.logicOp = VK_LOGIC_OP_COPY;
  L_colorBlending.attachmentCount = 1;
  L_colorBlending.pAttachments = &L_colorBlendAttachment;
  L_colorBlending.blendConstants[0] = 0.0f;
  L_colorBlending.blendConstants[1] = 0.0f;
  L_colorBlending.blendConstants[2] = 0.0f;
  L_colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> L_dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo L_dynamicState{};
  L_dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  L_dynamicState.dynamicStateCount = static_cast<uint32_t>(L_dynamicStates.size());
  L_dynamicState.pDynamicStates = L_dynamicStates.data();

  VkPipelineLayoutCreateInfo L_pipelineLayoutInfo{};
  L_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  L_pipelineLayoutInfo.setLayoutCount = 0;

  VkPushConstantRange L_pushConstantRange{};
  L_pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  L_pushConstantRange.offset = 0;
  L_pushConstantRange.size = sizeof(glm::vec3);

  L_pipelineLayoutInfo.pushConstantRangeCount = 1;
  L_pipelineLayoutInfo.pPushConstantRanges = &L_pushConstantRange;

  if (vkCreatePipelineLayout(device_, &L_pipelineLayoutInfo, nullptr, &pipeline_layout_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create pipeline layout";
    return false;
  }

  VkGraphicsPipelineCreateInfo L_pipelineInfo{};
  L_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  L_pipelineInfo.stageCount = 2;
  L_pipelineInfo.pStages = L_shaderStages;
  L_pipelineInfo.pVertexInputState = &L_vertexInputInfo;
  L_pipelineInfo.pInputAssemblyState = &L_inputAssembly;
  L_pipelineInfo.pViewportState = &L_viewportState;
  L_pipelineInfo.pRasterizationState = &L_rasterizer;
  L_pipelineInfo.pMultisampleState = &L_multisampling;
  L_pipelineInfo.pDepthStencilState = nullptr;
  L_pipelineInfo.pColorBlendState = &L_colorBlending;
  L_pipelineInfo.pDynamicState = &L_dynamicState;
  L_pipelineInfo.layout = pipeline_layout_;
  L_pipelineInfo.renderPass = render_pass_;
  L_pipelineInfo.subpass = 0;
  L_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(
          device_, VK_NULL_HANDLE, 1, &L_pipelineInfo, nullptr, &graphics_pipeline_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create graphics pipeline";
    return false;
  }

  return true;
}

inline bool Render::CreateVertexBuffer() {
  const std::vector<Vertex> L_vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

  VkBufferCreateInfo L_bufferInfo{};
  L_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  L_bufferInfo.size = sizeof(L_vertices[0]) * L_vertices.size();
  L_bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  L_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &L_bufferInfo, nullptr, &vertex_buffer_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create vertex buffer";
    return false;
  }

  VkMemoryRequirements L_memRequirements;
  vkGetBufferMemoryRequirements(device_, vertex_buffer_, &L_memRequirements);

  VkMemoryAllocateInfo L_allocInfo{};
  L_allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  L_allocInfo.allocationSize = L_memRequirements.size;
  L_allocInfo.memoryTypeIndex = FindMemoryType(L_memRequirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device_, &L_allocInfo, nullptr, &vertex_buffer_memory_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to allocate vertex buffer memory";
    return false;
  }

  vkBindBufferMemory(device_, vertex_buffer_, vertex_buffer_memory_, 0);

  void* L_data;
  vkMapMemory(device_, vertex_buffer_memory_, 0, L_bufferInfo.size, 0, &L_data);
  memcpy(L_data, L_vertices.data(), static_cast<size_t>(L_bufferInfo.size));
  vkUnmapMemory(device_, vertex_buffer_memory_);

  return true;
}

uint32_t Render::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties L_memProperties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &L_memProperties);

  for (uint32_t i = 0; i < L_memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (L_memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

// ---- Cleanup ---------------------------------------------------------------

void Render::CleanupFrame() {
  /*
  удалить временные ресурсы (например, динамические буферы, текстуры или промежуточные данные),
  их нужно освобождать здесь для избежания утечек памяти.
  */
}

} // namespace EDD::Managers