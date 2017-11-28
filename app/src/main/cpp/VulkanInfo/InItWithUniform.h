//
// Created by 川原　将太郎 on 2017/10/25.
//

#ifndef VULKANMULTITHREAD_INITWITHDEPTH_H
#define VULKANMULTITHREAD_INITWITHDEPTH_H

#include "AbstractInitVulkan.h"
class InItWithUniform:public AbstractInitVulkan, public IfInitVulkanOption{
public:
    void vInitDescriptorLayout(VulkanInfo &oVulkanInfo);
    void vInitPipelineLayouts(VulkanInfo &oVulkanInfo);
    void vInitDescriptorPool(VulkanInfo &oVulkanInfo);
    void vInitDescriptorSet(VulkanInfo &oVulkanInfo);
    InItWithUniform();
    ~InItWithUniform();

private:

};


#endif //VULKANMULTITHREAD_INITWITHDEPTH_H
