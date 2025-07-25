#pragma once

#pragma region // --------- DEFINES

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#define VK_USE_PLATFORM_WIN32_KHR

#define PVT_QFAM_SIZE 2u

#pragma endregion

#pragma region // --------- INCLUDES

#include <algorithm>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>

#include "input.h"
#include "sdw_def.h"
#include "sdw_utils.h"
#include "utils.h"

#pragma endregion

#pragma region // --------- TYPES

typedef struct
{
  VkCommandPool pool;
  VkCommandBuffer buffer;
} sdw_cmd_t;

typedef enum
{
  SDW_FAM_GFX, // Graphics
  SDW_FAM_CMP, // Compute
  SDW_FAM_TRX, // Transfer
  SDW_FAM_PRE, // Presentation
  SDW_FAM_NUM,
} sdw_fam_e;

typedef struct
{
  struct
  {
    b8 avail;
    u32 idx;
    VkQueue queue;
  } data[SDW_FAM_NUM];
  b8 complete;
  b8 shared;
  std::vector<u32> indices;
} sdw_fam_t;

typedef struct
{
  struct
  {
    std::vector<std::vector<c8>> spirv;
    std::vector<VkShaderStageFlagBits> types;
  } shaders;
  VkPipelineLayout layout;
  VkPipeline line;
} sdw_pip_t;

typedef struct
{
  std::vector<VkImage> images;
  std::vector<VkImageView> views;
  VkPresentModeKHR mode;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR format;
  VkSwapchainKHR chain;
  u32 image_idx;
} sdw_swp_t;

typedef struct
{
  VkSemaphore sem_image;
  VkSemaphore sem_render;
  VkFence fen_flight;
} sdw_syn_t;

typedef struct
{
  GLFWwindow *ptr;
  i32 width;
  i32 height;
  std::string name;
} sdw_win_t;

typedef std::vector<const char *> sdw_ext_t;
typedef std::vector<VkDynamicState> sdw_dyn_t;
typedef std::vector<VkFramebuffer> sdw_fbo_t;
typedef VkDevice sdw_dev_t;
typedef VkInstance sdw_ins_t;
typedef VkPhysicalDevice sdw_phy_t;
typedef VkSurfaceKHR sdw_sur_t;
typedef VkRenderPass sdw_ren_t;

typedef struct
{
  sdw_cmd_t cmd; // Command pool and buffers
  sdw_fbo_t fbo; // Framebuffers
  sdw_dev_t dev; // Logical device
  sdw_ext_t ext; // Extensions
  sdw_fam_t fam; // Queue families
  sdw_ins_t ins; // Instance
  sdw_phy_t phy; // Physical device
  sdw_pip_t pip; // Pipeline
  sdw_ren_t ren; // Render pass
  sdw_sur_t sur; // Surface
  sdw_swp_t swp; // Swapchain
  sdw_syn_t syn; // Synchronization (semaphores and fences)
  sdw_win_t win; // GLFW window
} sdw_t;

#pragma endregion

#pragma region // --------- LOCAL FUNCTIONS

#pragma region // --- Declarations

local b8 _sdw_chk_vlk_dev(sdw_t *sdw);    // Check if Vulkan physical device is suitable
local b8 _sdw_chk_vlk_ext(sdw_t *sdw);    // Check support for required extensions
local b8 _sdw_chk_vlk_fam(sdw_t *sdw);    // Check support for queue families
local b8 _sdw_chk_vlk_swp(sdw_t *sdw);    // Check support for swapchain
local v0 _sdw_drw_frm(sdw_t *sdw);        // Draw frame
local v0 _sdw_ini_vlk_01_ins(sdw_t *sdw); // Initialize Vulkan instance
local v0 _sdw_ini_vlk_02_sur(sdw_t *sdw); // Initialize Vulkan surface
local v0 _sdw_ini_vlk_03_phy(sdw_t *sdw); // Initialize Vulkan physical device
local v0 _sdw_ini_vlk_04_dev(sdw_t *sdw); // Initialize Vulkan logical device
local v0 _sdw_ini_vlk_05_swp(sdw_t *sdw); // Initialize Vulkan swap chain
local v0 _sdw_ini_vlk_06_viw(sdw_t *sdw); // Initialize Vulkan image views
local v0 _sdw_ini_vlk_07_ren(sdw_t *sdw); // Initialize Vulkan render pass
local v0 _sdw_ini_vlk_08_pip(sdw_t *sdw); // Initialize Vulkan pipeline
local v0 _sdw_ini_vlk_09_fbo(sdw_t *sdw); // Initialize Vulkan framebuffers
local v0 _sdw_ini_vlk_10_cmd(sdw_t *sdw); // Initialize Vulkan command pool and buffers
local v0 _sdw_ini_vlk_11_syn(sdw_t *sdw); // Initialize Vulkan synchronization objects
local v0 _sdw_ini_vlk(sdw_t *sdw);        // Initialize all Vulkan objects
local v0 _sdw_ini_win(sdw_t *sdw);        // Initialize GLFW window
local v0 _sdw_rec_cmd(sdw_t *sdw);        // Record a command buffer
local v0 _sdw_see_ext(sdw_t *sdw);        // Show available Vulkan extensions
local v0 _sdw_set_swp_ext(sdw_t *sdw);    // Set swapchain extent
local v0 _sdw_set_swp_fmt(sdw_t *sdw);    // Set swapchain format
local v0 _sdw_set_swp_mod(sdw_t *sdw);    // Set swapchain presentation mode

#pragma endregion

#pragma region // --- Implementations

local b8 _sdw_chk_vlk_dev(sdw_t *sdw)
{
  b8 chk_ext = false;
  b8 chk_fam = false;
  b8 chk_swp = false;

  chk_ext = _sdw_chk_vlk_ext(sdw);
  chk_fam = _sdw_chk_vlk_fam(sdw);
  chk_swp = _sdw_chk_vlk_swp(sdw);

  return chk_ext && chk_fam && chk_swp;
}

local b8 _sdw_chk_vlk_ext(sdw_t *sdw)
{
  std::set<std::string> required(sdw->ext.begin(), sdw->ext.end());

  SDW_OBJECTS(vkEnumerateDeviceExtensionProperties, ext, sdw->phy, NULL);
  for (const auto &e : ext)
  {
    required.erase(e.extensionName);
  }

  return required.empty();
}

local b8 _sdw_chk_vlk_fam(sdw_t *sdw)
{
  if (sdw->phy)
  {
    // Retrieve information about available queue families
    SDW_OBJECTS(vkGetPhysicalDeviceQueueFamilyProperties, fams, sdw->phy);
    for (u32 i = 0u; (i < fams.size()) && (sdw->fam.indices.size() < SDW_FAM_NUM); i++)
    {
      if (!(sdw->fam.data[SDW_FAM_GFX].avail) && (fams[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
      {
        sdw->fam.indices.push_back(i);
        sdw->fam.data[SDW_FAM_GFX].idx = i;
        sdw->fam.data[SDW_FAM_GFX].avail = true;
      }
      if (!(sdw->fam.data[SDW_FAM_CMP].avail) && (fams[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
      {
        sdw->fam.indices.push_back(i);
        sdw->fam.data[SDW_FAM_CMP].idx = i;
        sdw->fam.data[SDW_FAM_CMP].avail = true;
      }
      if (!(sdw->fam.data[SDW_FAM_TRX].avail) && (fams[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
      {
        sdw->fam.indices.push_back(i);
        sdw->fam.data[SDW_FAM_TRX].idx = i;
        sdw->fam.data[SDW_FAM_TRX].avail = true;
      }
      if (!sdw->fam.data[SDW_FAM_PRE].avail)
      {
        vkGetPhysicalDeviceSurfaceSupportKHR(sdw->phy, i, sdw->sur, &(sdw->fam.data[SDW_FAM_PRE].idx));
        if (sdw->fam.data[SDW_FAM_PRE].idx)
        {
          sdw->fam.indices.push_back(i);
          sdw->fam.data[SDW_FAM_PRE].idx = i;
          sdw->fam.data[SDW_FAM_PRE].avail = true;
        }
      }
    }
    // Set booleans for overall status
    if (sdw->fam.indices.size() == SDW_FAM_NUM)
    {
      sdw->fam.complete = true;
      sdw->fam.shared = true;
      for (u32 i = 0u; (i < (sdw->fam.indices.size() - 1u)) && sdw->fam.shared; i++)
      {
        if (sdw->fam.data[i].idx != sdw->fam.data[i + 1u].idx)
        {
          sdw->fam.shared = false;
        }
      }
    }
  }
  else
  {
    LOG_ERR("invalid physical dev.\n");
  }

  return sdw->fam.complete;
}

local b8 _sdw_chk_vlk_swp(sdw_t *sdw)
{
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(sdw->phy, sdw->sur, &sdw->swp.capabilities);
  SDW_OBJECTS(vkGetPhysicalDeviceSurfaceFormatsKHR, formats, sdw->phy, sdw->sur);
  SDW_OBJECTS(vkGetPhysicalDeviceSurfacePresentModesKHR, modes, sdw->phy, sdw->sur);

  return !formats.empty() && !modes.empty();
}

local v0 _sdw_drw_frm(sdw_t *sdw)
{
  // Wait for previous frame to be completed and reset fence
  vkWaitForFences(sdw->dev, 1u, &(sdw->syn.fen_flight), true, UINT64_MAX);
  vkResetFences(sdw->dev, 1u, &(sdw->syn.fen_flight));

  // Acquire image from the swapchain
  vkAcquireNextImageKHR(sdw->dev, sdw->swp.chain, UINT64_MAX, sdw->syn.sem_image, NULL, &(sdw->swp.image_idx));

  // Record the command buffer
  vkResetCommandBuffer(sdw->cmd.buffer, 0);
  _sdw_rec_cmd(sdw);

  // Submit the command buffer
  SDW_STRUCT(VkSubmitInfo, ci_submit);
  ci_submit.waitSemaphoreCount = 1u;
  ci_submit.pWaitSemaphores = SDW_CAPSULE(VkSemaphore, sdw->syn.sem_image);
  ci_submit.pWaitDstStageMask = SDW_CAPSULE(VkPipelineStageFlags, SDW_D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
  ci_submit.commandBufferCount = 1u;
  ci_submit.pCommandBuffers = &(sdw->cmd.buffer);
  ci_submit.signalSemaphoreCount = 1;
  ci_submit.pSignalSemaphores = SDW_CAPSULE(VkSemaphore, sdw->syn.sem_render);
  SDW_TRY(vkQueueSubmit(sdw->fam.data[SDW_FAM_GFX].queue, 1u, &ci_submit, sdw->syn.fen_flight));

  // Presentation
  SDW_STRUCT(VkPresentInfoKHR, ci_pre);
  ci_pre.waitSemaphoreCount = 1u;
  ci_pre.pWaitSemaphores = ci_submit.pSignalSemaphores;
  ci_pre.swapchainCount = 1u;
  ci_pre.pSwapchains = SDW_CAPSULE(VkSwapchainKHR, sdw->swp.chain);
  ci_pre.pImageIndices = &(sdw->swp.image_idx);
  vkQueuePresentKHR(sdw->fam.data[SDW_FAM_PRE].queue, &ci_pre);
}

local v0 _sdw_ini_vlk_01_ins(sdw_t *sdw)
{
  u32 ext_count = 0u;
  const c8 **ext_names = glfwGetRequiredInstanceExtensions(&ext_count);

  // Create Vulkan instance
  SDW_STRUCT(VkApplicationInfo, app_info);
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;
  const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
  SDW_STRUCT(VkInstanceCreateInfo, ci_ins);
  ci_ins.pApplicationInfo = &app_info;
  ci_ins.enabledExtensionCount = ext_count;
  ci_ins.ppEnabledExtensionNames = ext_names;
  ci_ins.enabledLayerCount = 1;
  ci_ins.ppEnabledLayerNames = validation_layers;
  SDW_TRY(vkCreateInstance(&ci_ins, NULL, &(sdw->ins)));

  // Show available extensions
  _sdw_see_ext(sdw);

  // Manually add other required ext extensions
  sdw->ext.push_back(SDW_D_KHR_SWAPCHAIN_EXTENSION_NAME);
  // sdw->ext.push_back(SDW_D_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
}

local v0 _sdw_ini_vlk_02_sur(sdw_t *sdw)
{
  SDW_STRUCT(VkWin32SurfaceCreateInfoKHR, ci_sur);
  ci_sur.hwnd = glfwGetWin32Window(sdw->win.ptr);
  ci_sur.hinstance = GetModuleHandle(NULL);
  SDW_TRY(vkCreateWin32SurfaceKHR(sdw->ins, &ci_sur, NULL, &(sdw->sur)));
}

local v0 _sdw_ini_vlk_03_phy(sdw_t *sdw)
{
  SDW_OBJECTS(vkEnumeratePhysicalDevices, devices, sdw->ins);

  // Init the variable with a harmless state
  sdw->phy = SDW_D_NULL_HANDLE;

  if (devices.size() > 0u)
  {
    // Check if any of the available devices fits
    for (u32 i = 0u; (i < devices.size()) && !(sdw->phy); i++)
    {
      sdw->phy = devices[i];
      if (_sdw_chk_vlk_dev(sdw) == false)
      {
        sdw->phy = SDW_D_NULL_HANDLE;
      }
    }
    if (!(sdw->phy))
    {
      LOG_ERR("failed to find a suitable GPU.\n");
    }
  }
  else
  {
    LOG_ERR("failed to find GPUs with Vulkan support.\n");
  }
}

local v0 _sdw_ini_vlk_04_dev(sdw_t *sdw)
{
  f32 fam_score = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> ci_fams;
  VkPhysicalDeviceFeatures features{};

  // Create individual queues info if available
  for (u32 i = 0u; i < SDW_FAM_NUM; i++)
  {
    if (sdw->fam.data[i].avail)
    {
      SDW_STRUCT(VkDeviceQueueCreateInfo, ci_fam);
      ci_fam.queueFamilyIndex = sdw->fam.data[i].idx;
      ci_fam.queueCount = 1u;
      ci_fam.pQueuePriorities = &fam_score;
      ci_fams.push_back(ci_fam);
    }
  }

  // Create device info passing accumulated queues info
  SDW_STRUCT(VkDeviceCreateInfo, ci_dev);
  ci_dev.pQueueCreateInfos = ci_fams.data();
  ci_dev.queueCreateInfoCount = 1u;
  ci_dev.pEnabledFeatures = &features;
  ci_dev.enabledExtensionCount = (u32)sdw->ext.size();
  ci_dev.ppEnabledExtensionNames = sdw->ext.data();
  SDW_TRY(vkCreateDevice(sdw->phy, &ci_dev, NULL, &(sdw->dev)));

  // Get queues after device creation
  vkGetDeviceQueue(sdw->dev, sdw->fam.data[SDW_FAM_GFX].idx, 0u, &(sdw->fam.data[SDW_FAM_GFX].queue));
  vkGetDeviceQueue(sdw->dev, sdw->fam.data[SDW_FAM_PRE].idx, 0u, &(sdw->fam.data[SDW_FAM_PRE].queue));
}

local v0 _sdw_ini_vlk_05_swp(sdw_t *sdw)
{
  u32 img_count = 0u;

  _sdw_set_swp_ext(sdw);
  _sdw_set_swp_fmt(sdw);
  _sdw_set_swp_mod(sdw);

  img_count = std::clamp(sdw->swp.capabilities.minImageCount + 1u, 0u, sdw->swp.capabilities.maxImageCount);

  SDW_STRUCT(VkSwapchainCreateInfoKHR, ci_swap);
  ci_swap.surface = sdw->sur;
  ci_swap.minImageCount = img_count;
  ci_swap.imageFormat = sdw->swp.format.format;
  ci_swap.imageColorSpace = sdw->swp.format.colorSpace;
  ci_swap.imageExtent = sdw->swp.capabilities.currentExtent;
  ci_swap.imageArrayLayers = 1u;
  ci_swap.imageUsage = SDW_D_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  ci_swap.preTransform = sdw->swp.capabilities.currentTransform;
  ci_swap.compositeAlpha = SDW_D_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci_swap.presentMode = sdw->swp.mode;
  ci_swap.clipped = SDW_D_TRUE;
  ci_swap.oldSwapchain = SDW_D_NULL_HANDLE;
  if (sdw->fam.shared)
  {
    ci_swap.imageSharingMode = SDW_D_SHARING_MODE_EXCLUSIVE;
    ci_swap.queueFamilyIndexCount = 0u;
    ci_swap.pQueueFamilyIndices = NULL;
  }
  else
  {
    ci_swap.imageSharingMode = SDW_D_SHARING_MODE_CONCURRENT;
    ci_swap.queueFamilyIndexCount = sdw->fam.indices.size();
    ci_swap.pQueueFamilyIndices = sdw->fam.indices.data();
  }
  SDW_TRY(vkCreateSwapchainKHR(sdw->dev, &ci_swap, NULL, &(sdw->swp.chain)));

  SDW_OBJECTS(vkGetSwapchainImagesKHR, images, sdw->dev, sdw->swp.chain);
  sdw->swp.images = images;
}

local v0 _sdw_ini_vlk_06_viw(sdw_t *sdw)
{
  sdw->swp.views.resize(sdw->swp.images.size());

  for (u32 i = 0u; i < sdw->swp.images.size(); i++)
  {
    SDW_STRUCT(VkImageViewCreateInfo, ci_iview);
    ci_iview.image = sdw->swp.images[i];
    ci_iview.viewType = SDW_D_IMAGE_VIEW_TYPE_2D;
    ci_iview.format = sdw->swp.format.format;
    ci_iview.components.r = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    ci_iview.components.g = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    ci_iview.components.b = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    ci_iview.components.a = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    ci_iview.subresourceRange.aspectMask = SDW_D_IMAGE_ASPECT_COLOR_BIT;
    ci_iview.subresourceRange.baseMipLevel = 0u;
    ci_iview.subresourceRange.levelCount = 1u;
    ci_iview.subresourceRange.baseArrayLayer = 0u;
    ci_iview.subresourceRange.layerCount = 1u;
    SDW_TRY(vkCreateImageView(sdw->dev, &ci_iview, NULL, &(sdw->swp.views[i])));
  }
}

local v0 _sdw_ini_vlk_07_ren(sdw_t *sdw)
{
  VkAttachmentDescription attach{};
  attach.format = sdw->swp.format.format;
  attach.samples = SDW_D_SAMPLE_COUNT_1_BIT;
  attach.loadOp = SDW_D_ATTACHMENT_LOAD_OP_CLEAR;
  attach.storeOp = SDW_D_ATTACHMENT_STORE_OP_STORE;
  attach.stencilLoadOp = SDW_D_ATTACHMENT_LOAD_OP_DONT_CARE;
  attach.stencilStoreOp = SDW_D_ATTACHMENT_STORE_OP_DONT_CARE;
  attach.initialLayout = SDW_D_IMAGE_LAYOUT_UNDEFINED;
  attach.finalLayout = SDW_D_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference ref{};
  ref.attachment = 0u;
  ref.layout = SDW_D_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = SDW_D_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1u;
  subpass.pColorAttachments = &ref;

  // Define subpasses dependencies
  VkSubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0u;
  dep.srcStageMask = SDW_D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0u;
  dep.dstStageMask = SDW_D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = SDW_D_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  SDW_STRUCT(VkRenderPassCreateInfo, ci_ren);
  ci_ren.attachmentCount = 1u;
  ci_ren.pAttachments = &attach;
  ci_ren.subpassCount = 1u;
  ci_ren.pSubpasses = &subpass;
  ci_ren.dependencyCount = 1u;
  ci_ren.pDependencies = &dep;
  SDW_TRY(vkCreateRenderPass(sdw->dev, &ci_ren, NULL, &(sdw->ren)));
}

local v0 _sdw_ini_vlk_08_pip(sdw_t *sdw)
{
  // Create shaders
  std::vector<VkPipelineShaderStageCreateInfo> ci_stages;
  for (u8 i = 0u; i < sdw->pip.shaders.spirv.size(); i++)
  {
    auto code = sdw->pip.shaders.spirv[i];
    SDW_STRUCT(VkPipelineShaderStageCreateInfo, ci_stage);
    ci_stage.stage = sdw->pip.shaders.types[i];
    ci_stage.pName = "main";
    SDW_STRUCT(VkShaderModuleCreateInfo, ci_shader);
    ci_shader.codeSize = code.size();
    ci_shader.pCode = reinterpret_cast<const uint32_t *>(code.data());
    SDW_TRY(vkCreateShaderModule(sdw->dev, &ci_shader, NULL, &(ci_stage.module)));
    ci_stages.push_back(ci_stage);
  }

  // Set vertex input properties (bindings/attributes)
  SDW_STRUCT(VkPipelineVertexInputStateCreateInfo, ci_ivex);
  ci_ivex.vertexBindingDescriptionCount = 0u;
  ci_ivex.pVertexBindingDescriptions = NULL;
  ci_ivex.vertexAttributeDescriptionCount = 0u;
  ci_ivex.pVertexAttributeDescriptions = NULL;

  // Set the type of primitive we're dealing with
  SDW_STRUCT(VkPipelineInputAssemblyStateCreateInfo, ci_iasm);
  ci_iasm.topology = SDW_D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  ci_iasm.primitiveRestartEnable = false;

  // Define viewport properties
  SDW_STRUCT(VkPipelineViewportStateCreateInfo, ci_view);
  ci_view.viewportCount = 1u;
  ci_view.scissorCount = 1u;

  // Define rasterization properties (vertex to fragment transformation process)
  SDW_STRUCT(VkPipelineRasterizationStateCreateInfo, ci_raster);
  ci_raster.depthClampEnable = false;
  ci_raster.rasterizerDiscardEnable = false;
  ci_raster.polygonMode = SDW_D_POLYGON_MODE_FILL;
  ci_raster.lineWidth = 1.0f;
  ci_raster.cullMode = SDW_D_CULL_MODE_BACK_BIT;
  ci_raster.frontFace = SDW_D_FRONT_FACE_CLOCKWISE;
  ci_raster.depthBiasEnable = false;

  // Define multisampling properties
  SDW_STRUCT(VkPipelineMultisampleStateCreateInfo, ci_multisamp);
  ci_multisamp.rasterizationSamples = SDW_D_SAMPLE_COUNT_1_BIT;
  ci_multisamp.sampleShadingEnable = false;

  // Define color blending rules
  VkPipelineColorBlendAttachmentState attach{};
  attach.blendEnable = false;
  attach.colorWriteMask = SDW_D_COLOR_COMPONENT_R_BIT |
                          SDW_D_COLOR_COMPONENT_G_BIT |
                          SDW_D_COLOR_COMPONENT_B_BIT |
                          SDW_D_COLOR_COMPONENT_A_BIT;

  SDW_STRUCT(VkPipelineColorBlendStateCreateInfo, ci_blend);
  ci_blend.logicOpEnable = false;
  ci_blend.logicOp = SDW_D_LOGIC_OP_COPY;
  ci_blend.attachmentCount = 1;
  ci_blend.pAttachments = &attach;

  // Declare viewport and scissor as dynamic states
  SDW_STRUCT(VkPipelineDynamicStateCreateInfo, ci_dyn);
  ci_dyn.dynamicStateCount = 2u;
  ci_dyn.pDynamicStates = SDW_CAPSULE(VkDynamicState, SDW_D_DYNAMIC_STATE_VIEWPORT, SDW_D_DYNAMIC_STATE_SCISSOR);

  // Define pipeline layout for managing uniforms
  SDW_STRUCT(VkPipelineLayoutCreateInfo, ci_layout);
  SDW_TRY(vkCreatePipelineLayout(sdw->dev, &ci_layout, NULL, &(sdw->pip.layout)));

  // Create graphics pipeline
  SDW_STRUCT(VkGraphicsPipelineCreateInfo, ci_pip);
  ci_pip.stageCount = 2;
  ci_pip.pStages = ci_stages.data();
  ci_pip.pVertexInputState = &ci_ivex;
  ci_pip.pInputAssemblyState = &ci_iasm;
  ci_pip.pViewportState = &ci_view;
  ci_pip.pRasterizationState = &ci_raster;
  ci_pip.pMultisampleState = &ci_multisamp;
  ci_pip.pColorBlendState = &ci_blend;
  ci_pip.pDynamicState = &ci_dyn;
  ci_pip.layout = sdw->pip.layout;
  ci_pip.renderPass = sdw->ren;

  // Finally instantiate the pipeline
  SDW_TRY(vkCreateGraphicsPipelines(sdw->dev, NULL, 1, &ci_pip, NULL, &(sdw->pip.line)));

  // Destroy info module ci_stages (MUST be done at the end)
  for (auto stage : ci_stages)
  {
    vkDestroyShaderModule(sdw->dev, stage.module, NULL);
  }
}

local v0 _sdw_ini_vlk_09_fbo(sdw_t *sdw)
{
  sdw->fbo.resize(sdw->swp.views.size());
  for (u32 i = 0u; i < sdw->swp.views.size(); i++)
  {
    SDW_STRUCT(VkFramebufferCreateInfo, ci_fbo);
    ci_fbo.renderPass = sdw->ren;
    ci_fbo.attachmentCount = 1u;
    ci_fbo.pAttachments = &(sdw->swp.views[i]);
    ci_fbo.width = sdw->swp.capabilities.currentExtent.width;
    ci_fbo.height = sdw->swp.capabilities.currentExtent.height;
    ci_fbo.layers = 1u;
    SDW_TRY(vkCreateFramebuffer(sdw->dev, &ci_fbo, NULL, &(sdw->fbo[i])));
  }
}

local v0 _sdw_ini_vlk_10_cmd(sdw_t *sdw)
{
  // Define command pool
  SDW_STRUCT(VkCommandPoolCreateInfo, ci_pool);
  ci_pool.flags = SDW_D_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  ci_pool.queueFamilyIndex = sdw->fam.data[SDW_FAM_GFX].idx;
  SDW_TRY(vkCreateCommandPool(sdw->dev, &ci_pool, NULL, &(sdw->cmd.pool)));

  // Allocate command buffer
  SDW_STRUCT(VkCommandBufferAllocateInfo, ci_alloc);
  ci_alloc.commandPool = sdw->cmd.pool;
  ci_alloc.level = SDW_D_COMMAND_BUFFER_LEVEL_PRIMARY;
  ci_alloc.commandBufferCount = 1u;
  SDW_TRY(vkAllocateCommandBuffers(sdw->dev, &ci_alloc, &(sdw->cmd.buffer)));
}

local v0 _sdw_ini_vlk_11_syn(sdw_t *sdw)
{
  // Create semaphores
  SDW_STRUCT(VkSemaphoreCreateInfo, ci_sem);
  SDW_TRY(vkCreateSemaphore(sdw->dev, &ci_sem, NULL, &(sdw->syn.sem_image)));
  SDW_TRY(vkCreateSemaphore(sdw->dev, &ci_sem, NULL, &(sdw->syn.sem_render)));

  // Create fence
  SDW_STRUCT(VkFenceCreateInfo, ci_fen);
  ci_fen.flags = SDW_D_FENCE_CREATE_SIGNALED_BIT;
  SDW_TRY(vkCreateFence(sdw->dev, &ci_fen, NULL, &(sdw->syn.fen_flight)));
}

local v0 _sdw_ini_vlk(sdw_t *sdw)
{
  _sdw_ini_vlk_01_ins(sdw);
  _sdw_ini_vlk_02_sur(sdw);
  _sdw_ini_vlk_03_phy(sdw);
  _sdw_ini_vlk_04_dev(sdw);
  _sdw_ini_vlk_05_swp(sdw);
  _sdw_ini_vlk_06_viw(sdw);
  _sdw_ini_vlk_07_ren(sdw);
  _sdw_ini_vlk_08_pip(sdw);
  _sdw_ini_vlk_09_fbo(sdw);
  _sdw_ini_vlk_10_cmd(sdw);
  _sdw_ini_vlk_11_syn(sdw);
}

local v0 _sdw_ini_win(sdw_t *sdw)
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  sdw->win.ptr = glfwCreateWindow(sdw->win.width, sdw->win.height, sdw->win.name.c_str(), NULL, NULL);
}

// TODO: make it generic to support custom command buffers
local v0 _sdw_rec_cmd(sdw_t *sdw)
{
  // Begin command buffer
  SDW_STRUCT(VkCommandBufferBeginInfo, ci_begin);
  SDW_TRY(vkBeginCommandBuffer(sdw->cmd.buffer, &ci_begin));

  // Begin render pass
  SDW_STRUCT(VkRenderPassBeginInfo, ci_pass);
  ci_pass.renderPass = sdw->ren;
  ci_pass.framebuffer = sdw->fbo[sdw->swp.image_idx];
  ci_pass.renderArea.offset = {0u, 0u};
  ci_pass.renderArea.extent = sdw->swp.capabilities.currentExtent;
  ci_pass.clearValueCount = 1u;
  ci_pass.pClearValues = SDW_CAPSULE(VkClearValue, 0.0f, 0.0f, 0.0f, 1.0f);

  // Start recording commands in the buffer
  vkCmdBeginRenderPass(sdw->cmd.buffer, &ci_pass, SDW_D_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(sdw->cmd.buffer, SDW_D_PIPELINE_BIND_POINT_GRAPHICS, sdw->pip.line);

  // Dynamic state: define viewport properties
  VkViewport view{};
  view.x = 0.0f;
  view.y = 0.0f;
  view.width = sdw->swp.capabilities.currentExtent.width;
  view.height = sdw->swp.capabilities.currentExtent.height;
  view.minDepth = 0.0f;
  view.maxDepth = 1.0f;
  vkCmdSetViewport(sdw->cmd.buffer, 0u, 1u, &view);

  // Dynamic state: define scissor properties
  VkRect2D scissor{};
  scissor.extent = sdw->swp.capabilities.currentExtent;
  scissor.offset = {0u, 0u};
  vkCmdSetScissor(sdw->cmd.buffer, 0u, 1u, &scissor);

  // Finally, issue the draw command and close the recording process
  vkCmdDraw(sdw->cmd.buffer, 3u, 1u, 0u, 0u);
  vkCmdEndRenderPass(sdw->cmd.buffer);
  SDW_TRY(vkEndCommandBuffer(sdw->cmd.buffer));
}

local v0 _sdw_see_ext(sdw_t *sdw)
{
  // Print available extentions
  SDW_OBJECTS(vkEnumerateInstanceExtensionProperties, ext, NULL);

  LOG_MSG("available ext:\n");
  for (u32 i = 0u; i < ext.size(); i++)
  {
    LOG_MSG("\t - %s\n", ext[i].extensionName);
  }
}

local v0 _sdw_set_swp_ext(sdw_t *sdw)
{
  i32 w = 0u;
  i32 h = 0u;

  if (sdw->swp.capabilities.currentExtent.width == UINT32_MAX)
  {
    glfwGetFramebufferSize(sdw->win.ptr, &w, &h);
    sdw->swp.capabilities.currentExtent.width = std::clamp((u32)w, sdw->swp.capabilities.minImageExtent.width, sdw->swp.capabilities.maxImageExtent.width);
    sdw->swp.capabilities.currentExtent.height = std::clamp((u32)h, sdw->swp.capabilities.minImageExtent.height, sdw->swp.capabilities.maxImageExtent.height);
  }
}

local v0 _sdw_set_swp_fmt(sdw_t *sdw)
{
  SDW_OBJECTS(vkGetPhysicalDeviceSurfaceFormatsKHR, formats, sdw->phy, sdw->sur);
  sdw->swp.format = formats[0];
  for (const auto &f : formats)
  {
    if (f.format == SDW_D_FORMAT_B8G8R8_SRGB && f.colorSpace == SDW_D_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      sdw->swp.format = f;
      break;
    }
  }
}

local v0 _sdw_set_swp_mod(sdw_t *sdw)
{
  SDW_OBJECTS(vkGetPhysicalDeviceSurfacePresentModesKHR, modes, sdw->phy, sdw->sur);
  sdw->swp.mode = SDW_D_PRESENT_MODE_FIFO_KHR;
  for (const auto &m : modes)
  {
    if (m == SDW_D_PRESENT_MODE_MAILBOX_KHR)
    {
      sdw->swp.mode = m;
      break;
    }
  }
}

#pragma endregion

#pragma endregion

#pragma region // --------- FUNCTIONS

local v0 sdw_del(sdw_t *sdw)
{
  vkDestroySemaphore(sdw->dev, sdw->syn.sem_image, NULL);
  vkDestroySemaphore(sdw->dev, sdw->syn.sem_render, NULL);
  vkDestroyFence(sdw->dev, sdw->syn.fen_flight, NULL);
  vkDestroyCommandPool(sdw->dev, sdw->cmd.pool, NULL);
  for (auto fbo : sdw->fbo)
  {
    vkDestroyFramebuffer(sdw->dev, fbo, NULL);
  }
  vkDestroyPipeline(sdw->dev, sdw->pip.line, NULL);
  vkDestroyPipelineLayout(sdw->dev, sdw->pip.layout, NULL);
  vkDestroyRenderPass(sdw->dev, sdw->ren, NULL);
  for (auto view : sdw->swp.views)
  {
    vkDestroyImageView(sdw->dev, view, NULL);
  }
  vkDestroySwapchainKHR(sdw->dev, sdw->swp.chain, NULL);
  vkDestroyDevice(sdw->dev, NULL);
  vkDestroySurfaceKHR(sdw->ins, sdw->sur, NULL);
  vkDestroyInstance(sdw->ins, NULL);
  glfwDestroyWindow(sdw->win.ptr);
  glfwTerminate();
}

local v0 sdw_ini(sdw_t *sdw, i32 w, i32 h, std::string name)
{
  sdw->win.name = name;
  sdw->win.width = w;
  sdw->win.height = h;

  _sdw_ini_win(sdw);
  _sdw_ini_vlk(sdw);
}

local v0 sdw_run(sdw_t *sdw)
{
  while ((!glfwWindowShouldClose(sdw->win.ptr)) && (!input_press('Q')))
  {
    glfwPollEvents();
    _sdw_drw_frm(sdw);
  }
  // Ensure all resource operations are completed before free them and quit
  vkDeviceWaitIdle(sdw->dev);
}

#pragma endregion
