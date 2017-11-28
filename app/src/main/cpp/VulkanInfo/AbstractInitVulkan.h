//
// Created by 川原　将太郎 on 2017/10/10.
//

#ifndef VULKANPRA_INITVULKAN_H
#define VULKANPRA_INITVULKAN_H

#include "../VulkanUtil.h"

class AbstractInitVulkan {
public:
    VkResult initGlobalLayerProp(VulkanInfo &oVulkanInfo);
    void initVulkanInstance(VulkanInfo &oVulkanInfo);
    void initEnumerateDevice(VulkanInfo &oVulkanInfo);
    void initSurfaceAndQueueIndex(VulkanInfo &oVulkanInfo, ANativeWindow *pPlatformWindow);
    void initSwapChainExtension(VulkanInfo &oVulkanInfo);

    void initVulkanDevice(VulkanInfo &oVulkanInfo);
    void initDeviceQueue(VulkanInfo &oVulkanInfo);
    void initSwapChain(VulkanInfo &oVulkanInfo, VkImageUsageFlags iUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    void initSwapChainViews(VulkanInfo &oVulkanInfo);

    void initDepthBuffer(VulkanInfo &oVulkanInfo);

    virtual void vInitPipelineLayouts(VulkanInfo &oVulkanInfo);
    void initUniformBuffers(VulkanInfo &oVulkanInfo);
    void initIndexBuffer(VulkanInfo &oVulkanInfo);
    void initRenderPass(VulkanInfo &oVulkanInfo, bool iIsIncludeDepth, bool iIsClear = true, VkImageLayout iFinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    void initFrameBuffers(VulkanInfo &oVulkanInfo, bool iIsIncludeDepth);
    void initVertexBuffer(VulkanInfo &oVulkanInfo, const void* iVertexData, uint32_t iDataSize, uint32_t iDataStride, bool iIsUseTexture);


    void initPipeLineCache(VulkanInfo &oVulkanInfo);
    void initPipeLine(VulkanInfo &oVulkanInfo, VkBool32 iIsIncludeDepth,
                      VkBool32 iInclude_vi = VK_TRUE);
    void initCommandPool(VulkanInfo &oVulkanInfo);
    void initCommandBuffer(VulkanInfo &oVulkanInfo);
    void initSemaphores(VulkanInfo &oVulkanInfo);
    AbstractInitVulkan();
    ~AbstractInitVulkan();

private:
    VkResult initDeviceExtensionProps(layer_properties &layerProps);
    const vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };


};

class IfInitVulkanOption{
public:
    virtual void vInitDescriptorLayout(VulkanInfo &oVulkanInfo) = 0;
    virtual void vInitDescriptorPool(VulkanInfo &oVulkanInfo) = 0;
    virtual void vInitDescriptorSet(VulkanInfo &oVulkanInfo) = 0;
    virtual ~IfInitVulkanOption(){};

};


#endif //VULKANPRA_INITVULKAN_H

