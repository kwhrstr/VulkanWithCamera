//
// Created by 川原　将太郎 on 2017/10/10.
//
#ifndef VULKANPRA_UTIL_H
#define VULKANPRA_UTIL_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <unistd.h>
#include "vulkan_wrapper.h"
#include "util.h"

using namespace std;


// Vulkan call wrapper
#define CALL_VK(func)                                                 \
  if (VK_SUCCESS != (func)) {                                         \
    __android_log_print(ANDROID_LOG_ERROR, "Tutorial ",               \
                         "Vulkan error. File[%s], line[%d]", __FILE__,\
                        __LINE__);                                    \
    assert(false);                                                    \
  }
#define NUM_DESCRIPTOR_SETS 1
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT


typedef struct {
    VkLayerProperties properties;
    vector<VkExtensionProperties> extensions;
} layer_properties;

typedef struct {
    bool initialized_;
    VkInstance instance_;
    vector<VkPhysicalDevice> gpuDevices_;
    VkPhysicalDeviceProperties gpuDeviceProperties_;
    VkPhysicalDeviceMemoryProperties gpuMemoryProperties_;
    VkDevice device_;
    VkSurfaceKHR surface_;
} deviceInfo;

typedef struct {
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    vector<VkQueueFamilyProperties> queue_props;
    uint32_t queue_family_count;
} queueInfo;

typedef struct {
    VkSwapchainKHR swapchain_;
    uint32_t swapchainLength_;

    VkExtent2D displaySize_;
    VkFormat displayFormat_;
    VkColorSpaceKHR imageColorSpace_;
    // array of frame buffers and views
    VkFramebuffer* framebuffers_;
    VkImageView* displayViews_;
}swapchainInfo;

typedef struct {
    VkFormat format;
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
}depthInfo;

typedef struct {
    VkRenderPass renderPass_;
    VkCommandPool cmdPool_;
    VkCommandBuffer* cmdBuffer_;
    uint32_t cmdBufferLen_;
    VkSemaphore renderFinSemaphore_;
    VkSemaphore imageAvailableSemaphore_;
    VkFence fence_;
} renderInfo;

typedef struct {
    VkBuffer buf;
    VkDeviceMemory mem;
    VkDescriptorBufferInfo buffer_info;
}bufferInfo;

typedef struct {
    VkPipelineLayout layout_;
    VkPipelineCache cache_;
    VkPipeline pipeline_;
}gfxPipeLineInfo;

typedef struct {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 Clip;
    glm::mat4 MVP;
}glmInfo;

typedef struct {
    VkDescriptorPool desc_pool;
    vector<VkDescriptorSet> desc_set;
    vector<VkDescriptorSetLayout> desc_layout;
} descInfo;

typedef struct {
    VkDescriptorImageInfo image_info;
    uint32_t rowPitch;
    VkDeviceSize textureSize;
} textureInfo;

typedef struct {
    VkSampler sampler;
    VkImage image;
    VkImageLayout imageLayout;
    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width, tex_height;
} textureObject;

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
};


struct VulkanInfo{
    deviceInfo device_info;
    queueInfo queue_info;
    swapchainInfo swapchain_info;
    renderInfo render_info;
    depthInfo depth_info;
    bufferInfo vertex_buffer_info;
    glmInfo glm_info;
    gfxPipeLineInfo gfx_pipeLine_info;
    bufferInfo uniform_info;
    bufferInfo index_info;
    descInfo desc_info;
    textureInfo texture_image_info;
    textureObject texture_object_info;
    vector<layer_properties> instance_layer_properties;
    vector<const char*> instance_extension_names;
    vector<const char*> device_extension_names;
    vector<const char *> instance_layer_names;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkVertexInputBindingDescription vi_binding;
    VkVertexInputAttributeDescription vi_attributes[2];
    int width;
    int height;
};


enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

void initBuffer(VulkanInfo &oVulkanInfo, VkDeviceSize iSize ,VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties,
                bufferInfo &oBufferInfo);

bool mapMemoryTypeToIndex(VulkanInfo &oVulkanInfo, uint32_t iTypeBits, VkFlags iRequirements_mask,
                          uint32_t *oTypeIndex);
void copyBuffer(VulkanInfo &oVulkanInfo ,VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

#endif //VULKANPRA_UTIL_H