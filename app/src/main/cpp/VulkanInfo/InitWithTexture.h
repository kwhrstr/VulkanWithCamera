//
// Created by 川原　将太郎 on 2017/10/25.
//

#ifndef VULKANMULTITHREAD_INITWITHTEXTURE_H
#define VULKANMULTITHREAD_INITWITHTEXTURE_H

#include "AbstractInitVulkan.h"
class InitWithTexture:public AbstractInitVulkan, public IfInitVulkanOption{
private:
public:
    void vInitDescriptorLayout(VulkanInfo &oVulkanInfo);
    void vInitPipelineLayouts(VulkanInfo &oVulkanInfo);
    void vInitDescriptorPool(VulkanInfo &oVulkanInfo);
    void vInitDescriptorSet(VulkanInfo &oVulkanInfo);
    void initTexture(VulkanInfo &oVulkanInfo,
                     VkImageUsageFlags iIsExtraUsages = 0 , VkFormatFeatureFlags iExtraFeatures = 0);
    InitWithTexture();
    ~InitWithTexture();
private:
    VkResult initImage(VulkanInfo &oVulkanInfo,
                       textureObject* pTexObj,
                       VkImageUsageFlags iIsExtraUsages,
                       VkFormatFeatureFlags iExtraFeatures);

};



#endif //VULKANMULTITHREAD_INITWITHTEXTURE_H
