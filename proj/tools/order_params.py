a = [
  "GLFWwindow* _window;",
  "VkInstance _instance;",
  "VkSurfaceKHR _surface;",
  "VkPhysicalDevice _physicalDevice;",
  "VkDescriptorPool _descriptorPool;",
  "VkDevice _device;",
  "VkQueue _graphicsQueue;",
  "VkQueue _presentQueue;",
  "VkSwapchainKHR _swapchain;",
  "VkFormat _swapchainImageFormat;",
  "VkExtent2D _swapchainExtent;",
  "std::vector<VkImage> _swapchainImages;",
  "std::vector<VkImageView> _swapchainImageViews;",
  "VkRenderPass _renderPass;",
  "VkDescriptorSetLayout _descriptorLayout;",
  "VkPipelineLayout _pipelineLayout;",
  "VkPipeline _pipeline;",
  "std::vector<VkFramebuffer> _framebuffers;",
  "VkCommandPool _commandPool;",
  "std::vector<VkCommandBuffer> _commandBuffers;",
  "std::vector<VkSemaphore> _imageAvailableSemaphores;",
  "std::vector<VkSemaphore> _renderFinishedSemaphores;",
  "std::vector<VkFence> _inFlightFences;",
  "uint32_t _currentFrame = 0;",
  "bool _framebufferResized = false;",
  "VkBuffer _vertexBuffer;",
  "VkDeviceMemory _vertexBufferMemory;",
  "VkBuffer _indexBuffer;",
  "VkDeviceMemory _indexBufferMemory;",
  "std::vector<VkBuffer> _uniformBuffers;",
  "std::vector<VkDeviceMemory> _uniformBuffersMemory;",
  "std::vector<void *> _uniformBuffersMapped;",
]

def mykey(e):
  return '_' + (e.split(' _')[1])

a.sort()

[print(x) for x in a]