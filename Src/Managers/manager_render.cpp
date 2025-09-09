#include "manager_render.h"

#include <algorithm>
#include <cstring>

namespace EDD::Managers {

void Render::Init(std::vector<std::any> args) {
  LOG::Debug() << "Render::Init called";
  if (args.size() < 3) {
    LOG::Fatal(__FILE__, __LINE__) << "Render::Init - insufficient parameters";
    return;
  }
  try {
    viewports_ = std::any_cast<std::vector<EDD::Data::Viewport*>>(args[0]);
    entity_manager_ = std::any_cast<Managers::Entity*>(args[1]);
    render_type_ = std::any_cast<RenderType>(args[2]);
  } catch (...) {
    LOG::Fatal(__FILE__, __LINE__) << "Render::Init - bad any_cast";
    return;
  }
  if (viewports_.empty() || !viewports_[0] || !viewports_[0]->viewport_window) {
    LOG::Fatal(__FILE__, __LINE__) << "Render::Init - invalid viewport";
    return;
  }
  if (!InitVulkan()) {
    LOG::Fatal(__FILE__, __LINE__) << "Render::Init - Vulkan init failed";
    return;
  }
  initialized_ = true;
  LOG::Debug() << "Render::Init OK";
}

void Render::Update() {
  if (!initialized_) return;
  DrawFrame();
}

void Render::FreeResources() {
  if (!initialized_) return;
  vkDeviceWaitIdle(device_);
  CleanupVulkan();
  initialized_ = false;
}

bool Render::InitVulkan() {
  return CreateInstance() &&
         CreateSurface() &&
         PickPhysicalDevice() &&
         CreateLogicalDevice() &&
         CreateSwapchain() &&
         CreateImageViews() &&
         CreateRenderPass() &&
         CreateFramebuffers() &&
         CreateCommandPool() &&
         AllocateCommandBuffers() &&
         RecordAllCommandBuffers() &&
         CreateSyncObjects();
}

// ---- Vulkan steps ---------------------------------------------------------

bool Render::CreateInstance() {
  VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
  app.pApplicationName = "EnoddEngine";
  app.applicationVersion = VK_MAKE_VERSION(0,1,0);
  app.pEngineName = "EnoddEngine";
  app.engineVersion = VK_MAKE_VERSION(0,1,0);
  app.apiVersion = VK_API_VERSION_1_2;

  uint32_t ext_count = 0;
  const char** glfw_ext = glfwGetRequiredInstanceExtensions(&ext_count);
  std::vector<const char*> extensions(glfw_ext, glfw_ext + ext_count);

  VkInstanceCreateInfo ci{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  ci.pApplicationInfo = &app;
  ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  ci.ppEnabledExtensionNames = extensions.data();

  if (vkCreateInstance(&ci, nullptr, &instance_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkCreateInstance failed";
    return false;
  }
  return true;
}

bool Render::CreateSurface() {
  GLFWwindow* wnd = viewports_[0]->viewport_window;
  if (glfwCreateWindowSurface(instance_, wnd, nullptr, &surface_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "glfwCreateWindowSurface failed";
    return false;
  }
  return true;
}

Render::QueueFamilies Render::FindQueueFamilies(VkPhysicalDevice dev) {
  QueueFamilies qf;
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
  std::vector<VkQueueFamilyProperties> props(count);
  vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());
  int i = 0;
  for (const auto& p : props) {
    if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      qf.graphics = i;
    VkBool32 present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface_, &present_support);
    if (present_support)
      qf.present = i;
    if (qf.Complete()) break;
    ++i;
  }
  return qf;
}

Render::SwapSupport Render::QuerySwapSupport(VkPhysicalDevice dev) {
  SwapSupport s;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, surface_, &s.caps);
  uint32_t count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface_, &count, nullptr);
  if (count) {
    s.formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface_, &count, s.formats.data());
  }
  vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface_, &count, nullptr);
  if (count) {
    s.present_modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface_, &count, s.present_modes.data());
  }
  return s;
}

bool Render::PickPhysicalDevice() {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance_, &count, nullptr);
  if (!count) {
    LOG::Fatal(__FILE__, __LINE__) << "No physical devices";
    return false;
  }
  std::vector<VkPhysicalDevice> devs(count);
  vkEnumeratePhysicalDevices(instance_, &count, devs.data());
  for (auto d : devs) {
    auto qf = FindQueueFamilies(d);
    auto sup = QuerySwapSupport(d);
    if (qf.Complete() && !sup.formats.empty() && !sup.present_modes.empty()) {
      physical_device_ = d;
      break;
    }
  }
  if (physical_device_ == VK_NULL_HANDLE) {
    LOG::Fatal(__FILE__, __LINE__) << "No suitable GPU";
    return false;
  }
  return true;
}

bool Render::CreateLogicalDevice() {
  auto qf = FindQueueFamilies(physical_device_);
  std::vector<VkDeviceQueueCreateInfo> queue_infos;
  std::vector<uint32_t> unique = {qf.graphics.value()};
  if (qf.present.value() != qf.graphics.value())
    unique.push_back(qf.present.value());
  float priority = 1.f;
  for (uint32_t idx : unique) {
    VkDeviceQueueCreateInfo qi{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qi.queueFamilyIndex = idx;
    qi.queueCount = 1;
    qi.pQueuePriorities = &priority;
    queue_infos.push_back(qi);
  }

  VkPhysicalDeviceFeatures feats{};
  VkDeviceCreateInfo ci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  ci.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
  ci.pQueueCreateInfos = queue_infos.data();
  ci.pEnabledFeatures = &feats;

  const char* device_ext = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  ci.enabledExtensionCount = 1;
  ci.ppEnabledExtensionNames = &device_ext;

  if (vkCreateDevice(physical_device_, &ci, nullptr, &device_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkCreateDevice failed";
    return false;
  }
  vkGetDeviceQueue(device_, qf.graphics.value(), 0, &graphics_queue_);
  vkGetDeviceQueue(device_, qf.present.value(), 0, &present_queue_);
  return true;
}

VkSurfaceFormatKHR Render::ChooseFormat(const std::vector<VkSurfaceFormatKHR>& fmts) {
  for (auto& f : fmts) {
    if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      return f;
  }
  return fmts[0];
}

VkPresentModeKHR Render::ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
  for (auto m : modes)
    if (m == VK_PRESENT_MODE_MAILBOX_KHR) return m;
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Render::ChooseExtent(const VkSurfaceCapabilitiesKHR& caps, GLFWwindow* wnd) {
  if (caps.currentExtent.width != UINT32_MAX) return caps.currentExtent;
  int w, h;
  glfwGetFramebufferSize(wnd, &w, &h);
  VkExtent2D e{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
  e.width = std::clamp(e.width, caps.minImageExtent.width, caps.maxImageExtent.width);
  e.height = std::clamp(e.height, caps.minImageExtent.height, caps.maxImageExtent.height);
  return e;
}

bool Render::CreateSwapchain() {
  auto support = QuerySwapSupport(physical_device_);
  auto format = ChooseFormat(support.formats);
  auto present_mode = ChoosePresentMode(support.present_modes);
  GLFWwindow* wnd = viewports_[0]->viewport_window;
  auto extent = ChooseExtent(support.caps, wnd);

  uint32_t image_count = support.caps.minImageCount + 1;
  if (support.caps.maxImageCount && image_count > support.caps.maxImageCount)
    image_count = support.caps.maxImageCount;

  VkSwapchainCreateInfoKHR ci{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  ci.surface = surface_;
  ci.minImageCount = image_count;
  ci.imageFormat = format.format;
  ci.imageColorSpace = format.colorSpace;
  ci.imageExtent = extent;
  ci.imageArrayLayers = 1;
  ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  auto qf = FindQueueFamilies(physical_device_);
  uint32_t indices[] = {qf.graphics.value(), qf.present.value()};
  if (qf.graphics != qf.present) {
    ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    ci.queueFamilyIndexCount = 2;
    ci.pQueueFamilyIndices = indices;
  } else {
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  ci.preTransform = support.caps.currentTransform;
  ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci.presentMode = present_mode;
  ci.clipped = VK_TRUE;
  ci.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_, &ci, nullptr, &swapchain_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkCreateSwapchainKHR failed";
    return false;
  }

  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr);
  swapchain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, swapchain_images_.data());

  swapchain_image_format_ = format.format;
  swapchain_extent_ = extent;
  return true;
}

bool Render::CreateImageViews() {
  swapchain_image_views_.resize(swapchain_images_.size());
  for (size_t i = 0; i < swapchain_images_.size(); ++i) {
    VkImageViewCreateInfo ci{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    ci.image = swapchain_images_[i];
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = swapchain_image_format_;
    ci.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                     VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device_, &ci, nullptr, &swapchain_image_views_[i]) != VK_SUCCESS) {
      LOG::Fatal(__FILE__, __LINE__) << "vkCreateImageView failed";
      return false;
    }
  }
  return true;
}

bool Render::CreateRenderPass() {
  VkAttachmentDescription color{};
  color.format = swapchain_image_format_;
  color.samples = VK_SAMPLE_COUNT_1_BIT;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference ref{};
  ref.attachment = 0;
  ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription sub{};
  sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub.colorAttachmentCount = 1;
  sub.pColorAttachments = &ref;

  VkSubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo ci{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  ci.attachmentCount = 1;
  ci.pAttachments = &color;
  ci.subpassCount = 1;
  ci.pSubpasses = &sub;
  ci.dependencyCount = 1;
  ci.pDependencies = &dep;

  if (vkCreateRenderPass(device_, &ci, nullptr, &render_pass_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkCreateRenderPass failed";
    return false;
  }
  return true;
}

bool Render::CreateFramebuffers() {
  framebuffers_.resize(swapchain_image_views_.size());
  for (size_t i = 0; i < swapchain_image_views_.size(); ++i) {
    VkImageView attachments[] = {swapchain_image_views_[i]};
    VkFramebufferCreateInfo ci{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    ci.renderPass = render_pass_;
    ci.attachmentCount = 1;
    ci.pAttachments = attachments;
    ci.width = swapchain_extent_.width;
    ci.height = swapchain_extent_.height;
    ci.layers = 1;
    if (vkCreateFramebuffer(device_, &ci, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
      LOG::Fatal(__FILE__, __LINE__) << "vkCreateFramebuffer failed";
      return false;
    }
  }
  return true;
}

bool Render::CreateCommandPool() {
  auto qf = FindQueueFamilies(physical_device_);
  VkCommandPoolCreateInfo ci{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
  ci.queueFamilyIndex = qf.graphics.value();
  ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  if (vkCreateCommandPool(device_, &ci, nullptr, &command_pool_) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkCreateCommandPool failed";
    return false;
  }
  return true;
}

bool Render::AllocateCommandBuffers() {
  command_buffers_.resize(framebuffers_.size());
  VkCommandBufferAllocateInfo ai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  ai.commandPool = command_pool_;
  ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  ai.commandBufferCount = static_cast<uint32_t>(command_buffers_.size());
  if (vkAllocateCommandBuffers(device_, &ai, command_buffers_.data()) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkAllocateCommandBuffers failed";
    return false;
  }
  return true;
}

bool Render::RecordAllCommandBuffers() {
  for (size_t i = 0; i < command_buffers_.size(); ++i) {
    VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(command_buffers_[i], &bi);

    VkClearValue clear{};
    clear.color = {{0.05f, 0.07f, 0.12f, 1.0f}};

    VkRenderPassBeginInfo rp{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rp.renderPass = render_pass_;
    rp.framebuffer = framebuffers_[i];
    rp.renderArea.offset = {0, 0};
    rp.renderArea.extent = swapchain_extent_;
    rp.clearValueCount = 1;
    rp.pClearValues = &clear;

    vkCmdBeginRenderPass(command_buffers_[i], &rp, VK_SUBPASS_CONTENTS_INLINE);
    // Ничего не рисуем: только очистка
    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      LOG::Fatal(__FILE__, __LINE__) << "vkEndCommandBuffer failed";
      return false;
    }
  }
  return true;
}

bool Render::CreateSyncObjects() {
  image_available_.resize(kMaxFramesInFlight);
  render_finished_.resize(kMaxFramesInFlight);
  in_flight_.resize(kMaxFramesInFlight);

  VkSemaphoreCreateInfo si{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fi{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fi.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
    if (vkCreateSemaphore(device_, &si, nullptr, &image_available_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &si, nullptr, &render_finished_[i]) != VK_SUCCESS ||
        vkCreateFence(device_, &fi, nullptr, &in_flight_[i]) != VK_SUCCESS) {
      LOG::Fatal(__FILE__, __LINE__) << "Sync creation failed";
      return false;
    }
  }
  return true;
}

// ---- Frame -----------------------------------------------------------------

void Render::DrawFrame() {
  vkWaitForFences(device_, 1, &in_flight_[current_frame_], VK_TRUE, UINT64_MAX);
  vkResetFences(device_, 1, &in_flight_[current_frame_]);

  uint32_t image_index = 0;
  VkResult acq = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX,
                                       image_available_[current_frame_],
                                       VK_NULL_HANDLE, &image_index);
  if (acq != VK_SUCCESS) {
    // (Resize обработка не реализована)
    return;
  }

  VkSemaphore wait_sems[] = {image_available_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signal_sems[] = {render_finished_[current_frame_]};

  VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = wait_sems;
  submit.pWaitDstStageMask = wait_stages;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &command_buffers_[image_index];
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = signal_sems;

  if (vkQueueSubmit(graphics_queue_, 1, &submit, in_flight_[current_frame_]) != VK_SUCCESS) {
    LOG::Fatal(__FILE__, __LINE__) << "vkQueueSubmit failed";
    return;
  }

  VkPresentInfoKHR present{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = signal_sems;
  present.swapchainCount = 1;
  present.pSwapchains = &swapchain_;
  present.pImageIndices = &image_index;

  vkQueuePresentKHR(present_queue_, &present);

  current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;
}

// ---- Cleanup ---------------------------------------------------------------

void Render::CleanupVulkan() {
  for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
    if (image_available_[i]) vkDestroySemaphore(device_, image_available_[i], nullptr);
    if (render_finished_[i]) vkDestroySemaphore(device_, render_finished_[i], nullptr);
    if (in_flight_[i]) vkDestroyFence(device_, in_flight_[i], nullptr);
  }

  if (command_pool_) {
    vkFreeCommandBuffers(device_, command_pool_,
                         static_cast<uint32_t>(command_buffers_.size()),
                         command_buffers_.data());
  }

  for (auto fb : framebuffers_)
    if (fb) vkDestroyFramebuffer(device_, fb, nullptr);
  framebuffers_.clear();

  if (render_pass_) vkDestroyRenderPass(device_, render_pass_, nullptr);

  for (auto iv : swapchain_image_views_)
    if (iv) vkDestroyImageView(device_, iv, nullptr);
  swapchain_image_views_.clear();

  if (swapchain_) vkDestroySwapchainKHR(device_, swapchain_, nullptr);
  if (command_pool_) vkDestroyCommandPool(device_, command_pool_, nullptr);
  if (device_) vkDestroyDevice(device_, nullptr);
  if (surface_) vkDestroySurfaceKHR(instance_, surface_, nullptr);
  if (instance_) vkDestroyInstance(instance_, nullptr);

  instance_ = VK_NULL_HANDLE;
  surface_ = VK_NULL_HANDLE;
  physical_device_ = VK_NULL_HANDLE;
  device_ = VK_NULL_HANDLE;
}

} // namespace EDD::Managers