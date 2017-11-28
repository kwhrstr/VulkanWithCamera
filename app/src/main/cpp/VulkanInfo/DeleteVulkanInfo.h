//
// Created by 川原　将太郎 on 2017/11/01.
//

#ifndef VULKANANDROID_DELETEVULKANINFO_H
#define VULKANANDROID_DELETEVULKANINFO_H

#include "../VulkanUtil.h"
class DeleteVulkanInfo {
private:

public:
    void deleteSwapChain(VulkanInfo &oVulkanInfo);
    void deleteUniformBuffer(VulkanInfo &oVulkanInfo);
    void deleteIndexBuffer(VulkanInfo &oVulkanInfo);
    void deleteVertexBuffer(VulkanInfo &oVulkanInfo);
    void deleteSemaphore(VulkanInfo &oVulkanInfo);
    void deleteInstance(VulkanInfo &oVulkanInfo);
    DeleteVulkanInfo();
    ~DeleteVulkanInfo();

    void deleteSurface(VulkanInfo &oVulkanInfo);

    void deleteTexture(VulkanInfo &oVulkanInfo);

    void deletePipeLine(VulkanInfo &oVulkanInfo);
};


#endif //VULKANANDROID_DELETEVULKANINFO_H
