#include "manager_render.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#include "EngineError/engine_logging.h"

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static VkVertexInputBindingDescription GetBindingDescription() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
  }

  static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
  }
};
static std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

  file.close();

  return buffer;
}

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

void Render::FreeResources() {
  if (!initialized_) return;
  for (auto L_fence : in_flight_fences_) {
    vkDestroyFence(device_, L_fence, nullptr);
  }
  for (auto L_semaphore : render_finished_semaphores_) {
    vkDestroySemaphore(device_, L_semaphore, nullptr);
  }
  for (auto L_semaphore : image_available_semaphores_) {
    vkDestroySemaphore(device_, L_semaphore, nullptr);
  }
  vkDestroyCommandPool(device_, command_pool_, nullptr);
  for (size_t L_i = 0; L_i < framebuffers_.size(); ++L_i) {
    for (auto L_framebuffer : framebuffers_[L_i]) {
      vkDestroyFramebuffer(device_, L_framebuffer, nullptr);
    }
  }
  vkDestroyRenderPass(device_, render_pass_, nullptr);
  for (size_t L_i = 0; L_i < swapchain_image_views_.size(); ++L_i) {
    for (auto L_imageView : swapchain_image_views_[L_i]) {
      vkDestroyImageView(device_, L_imageView, nullptr);
    }
  }
  for (auto L_swapchain : swapchains_) {
    vkDestroySwapchainKHR(device_, L_swapchain, nullptr);
  }
  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);
  vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  vkDestroyShaderModule(device_, frag_shader_module_, nullptr);
  vkDestroyShaderModule(device_, vert_shader_module_, nullptr);
  vkDestroyDevice(device_, nullptr);
  for (auto L_surface : surfaces_) {
    vkDestroySurfaceKHR(instance_, L_surface, nullptr);
  }
  vkDestroyInstance(instance_, nullptr);
  initialized_ = false;
}

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
  for (const auto& L_extension : L_availableExtensions) {
    if (strcmp(L_extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
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
  for (size_t L_i = 0; L_i < viewports_.size(); ++L_i) {
    if (glfwCreateWindowSurface(
            instance_, viewports_[L_i]->viewport_window, nullptr, &surfaces_[L_i]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create window surface for viewport " << L_i;
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

  for (const auto& L_device : L_devices) {
    if (IsDeviceSuitable(L_device)) {
      physical_device_ = L_device;
      VkPhysicalDeviceProperties L_props;
      vkGetPhysicalDeviceProperties(L_device, &L_props);
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

  for (uint32_t L_i = 0; L_i < L_queueFamilyCount; ++L_i) {
    if (L_queueFamilies[L_i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_queue_family_index_ = L_i;
    }
    VkBool32 L_presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        physical_device_, L_i, surfaces_[0], &L_presentSupport);
    if (L_presentSupport) {
      present_queue_family_index_ = L_i;
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
  for (uint32_t L_queueFamily : L_uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo L_queueCreateInfo{};
    L_queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    L_queueCreateInfo.queueFamilyIndex = L_queueFamily;
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

inline bool Render::CreateSwapchain() {
  swapchains_.resize(surfaces_.size());
  swapchain_images_.resize(surfaces_.size());
  swapchainimage_formats_.resize(surfaces_.size());
  swapchain_extents_.resize(surfaces_.size());

  for (size_t L_idx = 0; L_idx < surfaces_.size(); ++L_idx) {
    VkSurfaceCapabilitiesKHR L_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device_, surfaces_[L_idx], &L_capabilities);

    uint32_t L_formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device_, surfaces_[L_idx], &L_formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> L_formats(L_formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device_, surfaces_[L_idx], &L_formatCount, L_formats.data());

    VkSurfaceFormatKHR L_surfaceFormat = L_formats[0];
    for (const auto& L_availableFormat : L_formats) {
      if (L_availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          L_availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        L_surfaceFormat = L_availableFormat;
        break;
      }
    }

    uint32_t L_presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device_, surfaces_[L_idx], &L_presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> L_presentModes(L_presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device_, surfaces_[L_idx], &L_presentModeCount, L_presentModes.data());

    VkPresentModeKHR L_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& L_availablePresentMode : L_presentModes) {
      if (L_availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        L_presentMode = L_availablePresentMode;
        break;
      }
    }

    VkExtent2D L_extent = L_capabilities.currentExtent;
    if (L_capabilities.currentExtent.width != UINT32_MAX) {
      L_extent = L_capabilities.currentExtent;
    } else {
      int L_width, L_height;
      glfwGetFramebufferSize(viewports_[L_idx]->viewport_window, &L_width, &L_height);
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
    L_swapchainCreateInfo.surface = surfaces_[L_idx];
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

    if (vkCreateSwapchainKHR(device_, &L_swapchainCreateInfo, nullptr, &swapchains_[L_idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create swapchain for viewport " << L_idx;
      return false;
    }

    vkGetSwapchainImagesKHR(device_, swapchains_[L_idx], &L_imageCount, nullptr);
    swapchain_images_[L_idx].resize(L_imageCount);
    vkGetSwapchainImagesKHR(
        device_, swapchains_[L_idx], &L_imageCount, swapchain_images_[L_idx].data());

    swapchainimage_formats_[L_idx] = L_surfaceFormat.format;
    swapchain_extents_[L_idx] = L_extent;
  }
  return true;
}

inline bool Render::CreateImageViews() {
  swapchain_image_views_.resize(swapchains_.size());
  for (size_t L_idx = 0; L_idx < swapchains_.size(); ++L_idx) {
    swapchain_image_views_[L_idx].resize(swapchain_images_[L_idx].size());
    for (size_t L_i = 0; L_i < swapchain_images_[L_idx].size(); ++L_i) {
      VkImageViewCreateInfo L_viewCreateInfo{};
      L_viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      L_viewCreateInfo.image = swapchain_images_[L_idx][L_i];
      L_viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      L_viewCreateInfo.format = swapchainimage_formats_[L_idx];
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
              device_, &L_viewCreateInfo, nullptr, &swapchain_image_views_[L_idx][L_i]) !=
          VK_SUCCESS) {
        LOG::Error(__PRETTY_FUNCTION__, __LINE__)
            << "Failed to create image view for viewport " << L_idx << " image " << L_i;
        return false;
      }
    }
  }
  return true;
}

inline bool Render::CreateRenderPass() {
  VkAttachmentDescription L_colorAttachment{};
  L_colorAttachment.format = swapchainimage_formats_[0];  // assume all same
  L_colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  L_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  L_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  L_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  L_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  L_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  L_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference L_colorAttachmentRef{};
  L_colorAttachmentRef.attachment = 0;
  L_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription L_subpass{};
  L_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  L_subpass.colorAttachmentCount = 1;
  L_subpass.pColorAttachments = &L_colorAttachmentRef;

  VkRenderPassCreateInfo L_renderPassInfo{};
  L_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  L_renderPassInfo.attachmentCount = 1;
  L_renderPassInfo.pAttachments = &L_colorAttachment;
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
  for (size_t L_idx = 0; L_idx < swapchains_.size(); ++L_idx) {
    framebuffers_[L_idx].resize(swapchain_image_views_[L_idx].size());
    for (size_t L_i = 0; L_i < swapchain_image_views_[L_idx].size(); ++L_i) {
      VkImageView L_attachments[] = {swapchain_image_views_[L_idx][L_i]};

      VkFramebufferCreateInfo L_framebufferInfo{};
      L_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      L_framebufferInfo.renderPass = render_pass_;
      L_framebufferInfo.attachmentCount = 1;
      L_framebufferInfo.pAttachments = L_attachments;
      L_framebufferInfo.width = swapchain_extents_[L_idx].width;
      L_framebufferInfo.height = swapchain_extents_[L_idx].height;
      L_framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(
              device_, &L_framebufferInfo, nullptr, &framebuffers_[L_idx][L_i]) !=
          VK_SUCCESS) {
        LOG::Error(__PRETTY_FUNCTION__, __LINE__)
            << "Failed to create framebuffer for viewport " << L_idx << " image " << L_i;
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

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool_;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());

  if (vkAllocateCommandBuffers(device_, &allocInfo, command_buffers_.data()) != VK_SUCCESS) {
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

  for (size_t L_i = 0; L_i < swapchains_.size(); ++L_i) {
    if (vkCreateSemaphore(
            device_, &L_semaphoreInfo, nullptr, &image_available_semaphores_[L_i]) !=
            VK_SUCCESS ||
        vkCreateSemaphore(
            device_, &L_semaphoreInfo, nullptr, &render_finished_semaphores_[L_i]) !=
            VK_SUCCESS ||
        vkCreateFence(device_, &L_fenceInfo, nullptr, &in_flight_fences_[L_i]) != VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to create sync objects for viewport " << L_i;
      return false;
    }
  }
  return true;
}

void Render::DrawFrame() {
  if (!initialized_) return;

  auto L_entities = entity_manager_->GetAllEntities();
  LOG::Debug(__PRETTY_FUNCTION__) << "Rendering " << L_entities.size() << " entities";

  for (size_t L_idx = 0; L_idx < viewports_.size(); ++L_idx) {
    vkWaitForFences(device_, 1, &in_flight_fences_[L_idx], VK_TRUE, UINT64_MAX);
    vkResetFences(device_, 1, &in_flight_fences_[L_idx]);

    uint32_t L_imageIndex;
    vkAcquireNextImageKHR(device_,
                          swapchains_[L_idx],
                          UINT64_MAX,
                          image_available_semaphores_[L_idx],
                          VK_NULL_HANDLE,
                          &L_imageIndex);

    vkResetCommandBuffer(command_buffers_[L_idx], 0);

    VkCommandBufferBeginInfo L_beginInfo{};
    L_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffers_[L_idx], &L_beginInfo) != VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to begin recording command buffer for viewport " << L_idx;
      return;
    }

    VkRenderPassBeginInfo L_renderPassInfo{};
    L_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    L_renderPassInfo.renderPass = render_pass_;
    L_renderPassInfo.framebuffer = framebuffers_[L_idx][L_imageIndex];
    L_renderPassInfo.renderArea.offset = {0, 0};
    L_renderPassInfo.renderArea.extent = swapchain_extents_[L_idx];

    VkClearValue L_clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};  // Black color
    L_renderPassInfo.clearValueCount = 1;
    L_renderPassInfo.pClearValues = &L_clearColor;

    vkCmdBeginRenderPass(
        command_buffers_[L_idx], &L_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Render entities
    if (!L_entities.empty()) {
      auto L_view = entity_manager_->GetEntitiesByCoord();
      for (auto L_entity : L_view) {
        auto& L_coord = L_view.get<Coord>(L_entity);
        // For simplicity, render a square with the entity's color
        // Note: Position is not used yet, just color

        VkViewport L_viewport{};
        L_viewport.x = 0.0f;
        L_viewport.y = 0.0f;
        L_viewport.width = static_cast<float>(swapchain_extents_[L_idx].width);
        L_viewport.height = static_cast<float>(swapchain_extents_[L_idx].height);
        L_viewport.minDepth = 0.0f;
        L_viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffers_[L_idx], 0, 1, &L_viewport);

        VkRect2D L_scissor{};
        L_scissor.offset = {0, 0};
        L_scissor.extent = swapchain_extents_[L_idx];
        vkCmdSetScissor(command_buffers_[L_idx], 0, 1, &L_scissor);

        vkCmdBindPipeline(
            command_buffers_[L_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

        VkBuffer L_vertexBuffers[] = {vertex_buffer_};
        VkDeviceSize L_offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffers_[L_idx], 0, 1, L_vertexBuffers, L_offsets);

        // Push color from coord
        glm::vec3 L_color(
            L_coord.color_.r / 255.0f, L_coord.color_.g / 255.0f, L_coord.color_.b / 255.0f);
        vkCmdPushConstants(command_buffers_[L_idx],
                           pipeline_layout_,
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(glm::vec3),
                           &L_color);

        vkCmdDraw(command_buffers_[L_idx], 4, 1, 0, 0);

        // Only render the first entity for now
        break;
      }
    }

    vkCmdEndRenderPass(command_buffers_[L_idx]);

    if (vkEndCommandBuffer(command_buffers_[L_idx]) != VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to record command buffer for viewport " << L_idx;
      return;
    }

    VkSubmitInfo L_submitInfo{};
    L_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore L_waitSemaphores[] = {image_available_semaphores_[L_idx]};
    VkPipelineStageFlags L_waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    L_submitInfo.waitSemaphoreCount = 1;
    L_submitInfo.pWaitSemaphores = L_waitSemaphores;
    L_submitInfo.pWaitDstStageMask = L_waitStages;

    L_submitInfo.commandBufferCount = 1;
    L_submitInfo.pCommandBuffers = &command_buffers_[L_idx];

    VkSemaphore L_signalSemaphores[] = {render_finished_semaphores_[L_idx]};
    L_submitInfo.signalSemaphoreCount = 1;
    L_submitInfo.pSignalSemaphores = L_signalSemaphores;

    if (vkQueueSubmit(graphics_queue_, 1, &L_submitInfo, in_flight_fences_[L_idx]) !=
        VK_SUCCESS) {
      LOG::Error(__PRETTY_FUNCTION__, __LINE__)
          << "Failed to submit draw command buffer for viewport " << L_idx;
      return;
    }

    VkPresentInfoKHR L_presentInfo{};
    L_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    L_presentInfo.waitSemaphoreCount = 1;
    L_presentInfo.pWaitSemaphores = L_signalSemaphores;

    VkSwapchainKHR L_swapChains[] = {swapchains_[L_idx]};
    L_presentInfo.swapchainCount = 1;
    L_presentInfo.pSwapchains = L_swapChains;

    L_presentInfo.pImageIndices = &L_imageIndex;

    vkQueuePresentKHR(present_queue_, &L_presentInfo);
  }
}

inline bool Render::CreateShaderModules() {
  auto L_vertShaderCode = readFile("Assets/Shaders/simple.vert.spv");
  auto L_fragShaderCode = readFile("Assets/Shaders/simple.frag.spv");

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

inline bool Render::CreateGraphicsPipeline() {
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vert_shader_module_;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = frag_shader_module_;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  auto bindingDescription = Vertex::GetBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
      attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(glm::vec3);

  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipeline_layout_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create pipeline layout";
    return false;
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline_layout_;
  pipelineInfo.renderPass = render_pass_;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(
          device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline_) !=
      VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to create graphics pipeline";
    return false;
  }

  return true;
}

inline bool Render::CreateVertexBuffer() {
  const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

  VkBufferCreateInfo L_bufferInfo{};
  L_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  L_bufferInfo.size = sizeof(vertices[0]) * vertices.size();
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
  L_allocInfo.memoryTypeIndex = findMemoryType(L_memRequirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device_, &L_allocInfo, nullptr, &vertex_buffer_memory_) != VK_SUCCESS) {
    LOG::Error(__PRETTY_FUNCTION__, __LINE__) << "Failed to allocate vertex buffer memory";
    return false;
  }

  vkBindBufferMemory(device_, vertex_buffer_, vertex_buffer_memory_, 0);

  void* L_data;
  vkMapMemory(device_, vertex_buffer_memory_, 0, L_bufferInfo.size, 0, &L_data);
  memcpy(L_data, vertices.data(), static_cast<size_t>(L_bufferInfo.size));
  vkUnmapMemory(device_, vertex_buffer_memory_);

  return true;
}

uint32_t Render::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties L_memProperties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &L_memProperties);

  for (uint32_t L_i = 0; L_i < L_memProperties.memoryTypeCount; L_i++) {
    if ((typeFilter & (1 << L_i)) &&
        (L_memProperties.memoryTypes[L_i].propertyFlags & properties) == properties) {
      return L_i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

// ---- Cleanup ---------------------------------------------------------------

void Render::CleanupFrame() {}

} // namespace EDD::Managers