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

typedef enum
{
  SDW_FAM_GFX,
  SDW_FAM_PRE,
  SDW_FAM_NUM,
} sdw_fam_e;

typedef struct
{
  b8 complete;
  b8 shared;
  std::vector<u32> indices;
  VkQueue queue_gfx;
  VkQueue queue_pre;
} sdw_fam_t;

typedef struct
{

} sdw_shd_t;

typedef struct
{
  struct
  {
    std::vector<std::vector<c8>> spirv;
    std::vector<VkShaderStageFlagBits> types;
  } shaders;
  std::vector<VkDynamicState> dyn_states;
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
} sdw_swp_t;

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
  sdw_fbo_t fbo; // Framebuffers
  sdw_dev_t dev; // Logical device
  sdw_dyn_t dyn; // Dynamic states (Pipeline)
  sdw_ext_t ext; // Extensions
  sdw_fam_t fam; // Queue families
  sdw_ins_t ins; // Instance
  sdw_phy_t phy; // Physical device
  sdw_pip_t pip; // Pipeline
  sdw_ren_t ren; // Render pass
  sdw_sur_t sur; // Surface
  sdw_swp_t swp; // Swapchain
  sdw_win_t win; // GLFW window
} sdw_t;

#pragma endregion

#pragma region // --------- LOCAL FUNCTIONS

#pragma region // --- Declarations

local b8 _sdw_chk_vlk_dev(sdw_t *sdw);   // Check if Vulkan physical device is suitable
local b8 _sdw_chk_vlk_ext(sdw_t *sdw);   // Check support for required extensions
local b8 _sdw_chk_vlk_fam(sdw_t *sdw);   // Check support for queue families
local b8 _sdw_chk_vlk_swp(sdw_t *sdw);   // Check support for swapchain
local v0 _sdw_ini_vlk_1_ins(sdw_t *sdw); // Initialize Vulkan instance
local v0 _sdw_ini_vlk_2_sur(sdw_t *sdw); // Initialize Vulkan surface
local v0 _sdw_ini_vlk_3_phy(sdw_t *sdw); // Initialize Vulkan physical device
local v0 _sdw_ini_vlk_4_dev(sdw_t *sdw); // Initialize Vulkan logical device
local v0 _sdw_ini_vlk_5_swp(sdw_t *sdw); // Initialize Vulkan swap chain
local v0 _sdw_ini_vlk_6_viw(sdw_t *sdw); // Initialize Vulkan image views
local v0 _sdw_ini_vlk_7_ren(sdw_t *sdw); // Initialize Vulkan render pass
local v0 _sdw_ini_vlk_8_pip(sdw_t *sdw); // Initialize Vulkan pipeline
local v0 _sdw_ini_vlk_9_fbo(sdw_t *sdw); // Initialize Vulkan framebuffers
local v0 _sdw_ini_vlk(sdw_t *sdw);       // Initialize all Vulkan objects
local v0 _sdw_ini_win(sdw_t *sdw);       // Initialize GLFW window
local v0 _sdw_see_ext(sdw_t *sdw);       // Show available Vulkan extensions
local v0 _sdw_set_swp_ext(sdw_t *sdw);   // Set swapchain extent
local v0 _sdw_set_swp_fmt(sdw_t *sdw);   // Set swapchain format
local v0 _sdw_set_swp_mod(sdw_t *sdw);   // Set swapchain presentation mode

#pragma endregion

#pragma region // --- Implementations

local b8 _sdw_chk_vlk_dev(sdw_t *sdw)
{
  b8 chk_ext = false;
  b8 chk_fam = false;
  b8 chk_swp = false;

  sdw->fam.complete = false;
  sdw->fam.shared = false;
  sdw->fam.indices.resize(SDW_FAM_NUM);
  sdw->fam.queue_gfx = SDW_D_NULL_HANDLE;
  sdw->fam.queue_pre = SDW_D_NULL_HANDLE;

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
  u32 present_support = false;

  if (sdw->phy)
  {
    SDW_OBJECTS(vkGetPhysicalDeviceQueueFamilyProperties, qfams, sdw->phy);
    for (u32 i = 0u; (i < qfams_count) && (sdw->fam.indices.size() < sdw->fam.indices.capacity()); i++)
    {
      if (qfams[i].queueFlags & SDW_D_QUEUE_GRAPHICS_BIT)
      {
        sdw->fam.indices.push_back(i);
      }
      present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(sdw->phy, i, sdw->sur, &present_support);
      if (present_support)
      {
        sdw->fam.indices.push_back(i);
      }
    }
  }
  else
  {
    LOG_ERR("invalid physical dev.\n");
  }

  sdw->fam.complete = sdw->fam.indices.size() == sdw->fam.indices.capacity();
  sdw->fam.shared = sdw->fam.indices[SDW_FAM_GFX] == sdw->fam.indices[SDW_FAM_PRE];

  return sdw->fam.complete;
}

local b8 _sdw_chk_vlk_swp(sdw_t *sdw)
{
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(sdw->phy, sdw->sur, &sdw->swp.capabilities);
  SDW_OBJECTS(vkGetPhysicalDeviceSurfaceFormatsKHR, formats, sdw->phy, sdw->sur);
  SDW_OBJECTS(vkGetPhysicalDeviceSurfacePresentModesKHR, modes, sdw->phy, sdw->sur);

  return !formats.empty() && !modes.empty();
}

local v0 _sdw_ini_vlk_1_ins(sdw_t *sdw)
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
  SDW_STRUCT(VkInstanceCreateInfo, create_info);
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = ext_count;
  create_info.ppEnabledExtensionNames = ext_names;
  create_info.enabledLayerCount = 1;
  create_info.ppEnabledLayerNames = validation_layers;
  SDW_TRY(vkCreateInstance(&create_info, NULL, &(sdw->ins)));

  // Show available extensions
  _sdw_see_ext(sdw);

  // Manually add other required ext extensions
  sdw->ext.push_back(SDW_D_KHR_SWAPCHAIN_EXTENSION_NAME);
  // sdw->ext.push_back(SDW_D_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
}

local v0 _sdw_ini_vlk_2_sur(sdw_t *sdw)
{
  SDW_STRUCT(VkWin32SurfaceCreateInfoKHR, create_info);
  create_info.hwnd = glfwGetWin32Window(sdw->win.ptr);
  create_info.hinstance = GetModuleHandle(NULL);
  SDW_TRY(vkCreateWin32SurfaceKHR(sdw->ins, &create_info, NULL, &(sdw->sur)));
}

local v0 _sdw_ini_vlk_3_phy(sdw_t *sdw)
{
  SDW_OBJECTS(vkEnumeratePhysicalDevices, devices, sdw->ins);

  // Init the variable with a harmless state
  sdw->phy = SDW_D_NULL_HANDLE;

  if (devices_count > 0u)
  {
    // Check if any of the available devices fits
    for (u32 i = 0u; (i < devices_count) && !(sdw->phy); i++)
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

local v0 _sdw_ini_vlk_4_dev(sdw_t *sdw)
{
  f32 fam_score = 1.0f;
  std::set<u32> fam_set = {sdw->fam.indices.begin(), sdw->fam.indices.end()};
  std::vector<VkDeviceQueueCreateInfo> fam_create_infos;
  VkPhysicalDeviceFeatures dev_feats{};

  for (const auto &i : fam_set)
  {
    SDW_STRUCT(VkDeviceQueueCreateInfo, fam_create_info);
    fam_create_info.queueFamilyIndex = sdw->fam.indices[i];
    fam_create_info.queueCount = 1u;
    fam_create_info.pQueuePriorities = &fam_score;
    fam_create_infos.push_back(fam_create_info);
  }

  SDW_STRUCT(VkDeviceCreateInfo, dev_ini_info);
  dev_ini_info.pQueueCreateInfos = fam_create_infos.data();
  dev_ini_info.queueCreateInfoCount = 1u;
  dev_ini_info.pEnabledFeatures = &dev_feats;
  dev_ini_info.enabledExtensionCount = (u32)sdw->ext.size();
  dev_ini_info.ppEnabledExtensionNames = sdw->ext.data();
  SDW_TRY(vkCreateDevice(sdw->phy, &dev_ini_info, NULL, &(sdw->dev)));

  vkGetDeviceQueue(sdw->dev, sdw->fam.indices[SDW_FAM_GFX], 0u, &(sdw->fam.queue_gfx));
  vkGetDeviceQueue(sdw->dev, sdw->fam.indices[SDW_FAM_PRE], 0u, &(sdw->fam.queue_pre));
}

local v0 _sdw_ini_vlk_5_swp(sdw_t *sdw)
{
  u32 img_count = 0u;

  _sdw_set_swp_ext(sdw);
  _sdw_set_swp_fmt(sdw);
  _sdw_set_swp_mod(sdw);

  img_count = std::clamp(sdw->swp.capabilities.minImageCount + 1u, 0u, sdw->swp.capabilities.maxImageCount);

  SDW_STRUCT(VkSwapchainCreateInfoKHR, create_info);
  create_info.surface = sdw->sur;
  create_info.minImageCount = img_count;
  create_info.imageFormat = sdw->swp.format.format;
  create_info.imageColorSpace = sdw->swp.format.colorSpace;
  create_info.imageExtent = sdw->swp.capabilities.currentExtent;
  create_info.imageArrayLayers = 1u;
  create_info.imageUsage = SDW_D_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  create_info.preTransform = sdw->swp.capabilities.currentTransform;
  create_info.compositeAlpha = SDW_D_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = sdw->swp.mode;
  create_info.clipped = SDW_D_TRUE;
  create_info.oldSwapchain = SDW_D_NULL_HANDLE;
  if (sdw->fam.shared)
  {
    create_info.imageSharingMode = SDW_D_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0u;
    create_info.pQueueFamilyIndices = NULL;
  }
  else
  {
    create_info.imageSharingMode = SDW_D_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2u;
    create_info.pQueueFamilyIndices = sdw->fam.indices.data();
  }
  SDW_TRY(vkCreateSwapchainKHR(sdw->dev, &create_info, NULL, &(sdw->swp.chain)));

  SDW_OBJECTS(vkGetSwapchainImagesKHR, images, sdw->dev, sdw->swp.chain);
  sdw->swp.images = images;
}

local v0 _sdw_ini_vlk_6_viw(sdw_t *sdw)
{
  sdw->swp.views.resize(sdw->swp.images.size());

  for (u32 i = 0u; i < sdw->swp.images.size(); i++)
  {
    SDW_STRUCT(VkImageViewCreateInfo, create_info);
    create_info.image = sdw->swp.images[i];
    create_info.viewType = SDW_D_IMAGE_VIEW_TYPE_2D;
    create_info.format = sdw->swp.format.format;
    create_info.components.r = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = SDW_D_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = SDW_D_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0u;
    create_info.subresourceRange.levelCount = 1u;
    create_info.subresourceRange.baseArrayLayer = 0u;
    create_info.subresourceRange.layerCount = 1u;
    SDW_TRY(vkCreateImageView(sdw->dev, &create_info, NULL, &(sdw->swp.views[i])));
  }
}

local v0 _sdw_ini_vlk_7_ren(sdw_t *sdw)
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

  SDW_STRUCT(VkRenderPassCreateInfo, ci_ren);
  ci_ren.attachmentCount = 1u;
  ci_ren.pAttachments = &attach;
  ci_ren.subpassCount = 1u;
  ci_ren.pSubpasses = &subpass;
  SDW_TRY(vkCreateRenderPass(sdw->dev, &ci_ren, NULL, &(sdw->ren)));
}

local v0 _sdw_ini_vlk_8_pip(sdw_t *sdw)
{
  // Create shaders
  std::vector<VkPipelineShaderStageCreateInfo> stages;
  for (u8 i = 0u; i < sdw->pip.shaders.spirv.size(); i++)
  {
    auto code = sdw->pip.shaders.spirv[i];
    SDW_STRUCT(VkPipelineShaderStageCreateInfo, stage_info);
    stage_info.stage = sdw->pip.shaders.types[i];
    stage_info.pName = "main";
    SDW_STRUCT(VkShaderModuleCreateInfo, create_info);
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());
    SDW_TRY(vkCreateShaderModule(sdw->dev, &create_info, NULL, &(stage_info.module)));
    stages.push_back(stage_info);
  }

  // Set vertex input properties (bindings/attributes)
  SDW_STRUCT(VkPipelineVertexInputStateCreateInfo, ci_vertex);
  ci_vertex.vertexBindingDescriptionCount = 0u;
  ci_vertex.pVertexBindingDescriptions = NULL;
  ci_vertex.vertexAttributeDescriptionCount = 0u;
  ci_vertex.pVertexAttributeDescriptions = NULL;

  // Set the type of primitive we're dealing with
  SDW_STRUCT(VkPipelineInputAssemblyStateCreateInfo, ci_assembly);
  ci_assembly.topology = SDW_D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  ci_assembly.primitiveRestartEnable = false;

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
  ci_pip.pStages = stages.data();
  ci_pip.pVertexInputState = &ci_vertex;
  ci_pip.pInputAssemblyState = &ci_assembly;
  ci_pip.pViewportState = &ci_view;
  ci_pip.pRasterizationState = &ci_raster;
  ci_pip.pMultisampleState = &ci_multisamp;
  ci_pip.pColorBlendState = &ci_blend;
  ci_pip.pDynamicState = &ci_dyn;
  ci_pip.layout = sdw->pip.layout;
  ci_pip.renderPass = sdw->ren;

  // Finally instantiate the pipeline
  SDW_TRY(vkCreateGraphicsPipelines(sdw->dev, NULL, 1, &ci_pip, NULL, &(sdw->pip.line)));

  // Destroy info module stages (MUST be done at the end)
  for (auto stage : stages)
  {
    vkDestroyShaderModule(sdw->dev, stage.module, NULL);
  }
}

local v0 _sdw_ini_vlk_9_fbo(sdw_t *sdw)
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

local v0 _sdw_ini_vlk(sdw_t *sdw)
{
  _sdw_ini_vlk_1_ins(sdw);
  _sdw_ini_vlk_2_sur(sdw);
  _sdw_ini_vlk_3_phy(sdw);
  _sdw_ini_vlk_4_dev(sdw);
  _sdw_ini_vlk_5_swp(sdw);
  _sdw_ini_vlk_6_viw(sdw);
  _sdw_ini_vlk_7_ren(sdw);
  _sdw_ini_vlk_8_pip(sdw);
  _sdw_ini_vlk_9_fbo(sdw);
}

local v0 _sdw_ini_win(sdw_t *sdw)
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  sdw->win.ptr = glfwCreateWindow(sdw->win.width, sdw->win.height, sdw->win.name.c_str(), NULL, NULL);
}

local v0 _sdw_see_ext(sdw_t *sdw)
{
  // Print available extentions
  SDW_OBJECTS(vkEnumerateInstanceExtensionProperties, ext, NULL);

  LOG_MSG("available ext:\n");
  for (u32 i = 0u; i < ext_count; i++)
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
  }
}

#pragma endregion
