//
// Created by 川原　将太郎 on 2017/10/10.
//

#include "VulkanUtil.h"
bool mapMemoryTypeToIndex(VulkanInfo &oVulkanInfo, uint32_t iTypeBits, VkFlags iRequirements_mask,
                          uint32_t *oTypeIndex) {
    for(uint32_t ii = 0; ii < oVulkanInfo.device_info.gpuMemoryProperties_.memoryTypeCount; ii++){
        if((iTypeBits & 1) == 1){
            if((oVulkanInfo.device_info.gpuMemoryProperties_.memoryTypes[ii].propertyFlags & iRequirements_mask)
               == iRequirements_mask){
                *oTypeIndex = ii;
                return true;
            }
        }
        iTypeBits >>=1;
    }
    return false;
}


void initBuffer(VulkanInfo &oVulkanInfo,VkDeviceSize iSize, VkBufferUsageFlags iUsage, VkMemoryPropertyFlags iProperties,
                bufferInfo &oBufferInfo){
    VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .usage = iUsage,
            .size = iSize,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &oVulkanInfo.queue_info.graphics_queue_family_index,
    };
    CALL_VK(vkCreateBuffer(oVulkanInfo.device_info.device_, &bufferCreateInfo, nullptr, &oBufferInfo.buf));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(oVulkanInfo.device_info.device_, oBufferInfo.buf, &memReqs);

    VkMemoryAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReqs.size,
    };
    bool pass = mapMemoryTypeToIndex(oVulkanInfo, memReqs.memoryTypeBits,
                                     iProperties, &allocateInfo.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");
    CALL_VK(vkAllocateMemory(oVulkanInfo.device_info.device_, &allocateInfo, nullptr, &oBufferInfo.mem));
    CALL_VK(vkBindBufferMemory(oVulkanInfo.device_info.device_, oBufferInfo.buf, oBufferInfo.mem, 0));
}

void copyBuffer(VulkanInfo &oVulkanInfo ,VkBuffer iSrcBuffer, VkBuffer iDstBuffer, VkDeviceSize iSize) {
    VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandPool = oVulkanInfo.render_info.cmdPool_,
            .commandBufferCount = 1,
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(oVulkanInfo.device_info.device_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {
            .size = iSize
    };
    vkCmdCopyBuffer(commandBuffer, iSrcBuffer, iDstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
    };
    vkQueueSubmit(oVulkanInfo.queue_info.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(oVulkanInfo.queue_info.graphics_queue);

    vkFreeCommandBuffers(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.cmdPool_, 1,
                         &commandBuffer);
}

