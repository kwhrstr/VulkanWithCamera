//
// Created by 川原　将太郎 on 2017/10/25.
//

#include "InItWithUniform.h"
InItWithUniform::InItWithUniform() {}
InItWithUniform::~InItWithUniform() {}

void InItWithUniform::vInitDescriptorLayout(VulkanInfo &oVulkanInfo) {
    VkDescriptorSetLayoutBinding layoutBindings[1];
    layoutBindings[0] = {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
    };
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount =  1,
            .pBindings = layoutBindings,
    };
    oVulkanInfo.desc_info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
    CALL_VK(vkCreateDescriptorSetLayout(oVulkanInfo.device_info.device_, &descriptorLayoutInfo,
                                        nullptr, oVulkanInfo.desc_info.desc_layout.data()));

}

void InItWithUniform::vInitPipelineLayouts(VulkanInfo &oVulkanInfo) {
    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = NUM_DESCRIPTOR_SETS,
            .pSetLayouts = oVulkanInfo.desc_info.desc_layout.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    CALL_VK(vkCreatePipelineLayout(oVulkanInfo.device_info.device_, &pipelineLayoutCreateInfo,
                                   nullptr, &oVulkanInfo.gfx_pipeLine_info.layout_));
}

void InItWithUniform::vInitDescriptorPool(VulkanInfo &oVulkanInfo) {
    /* DEPENDS on init_uniform_buffer() and
      * init_descriptor_and_pipeline_layouts() */
    VkDescriptorPoolSize typeCount[1];
    typeCount[0] ={
            .type =VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
    };
    VkDescriptorPoolCreateInfo descriptorPool = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = typeCount,
    };
    CALL_VK(vkCreateDescriptorPool(oVulkanInfo.device_info.device_, &descriptorPool, nullptr, &oVulkanInfo.desc_info.desc_pool))
}


void InItWithUniform::vInitDescriptorSet(VulkanInfo &oVulkanInfo) {
    /* DEPENDS on init_descriptor_pool() */
    VkDescriptorSetAllocateInfo allocateInfo[1];
    allocateInfo[0] = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = oVulkanInfo.desc_info.desc_pool,
            .pSetLayouts = oVulkanInfo.desc_info.desc_layout.data(),
            .descriptorSetCount = NUM_DESCRIPTOR_SETS,
    };
    oVulkanInfo.desc_info.desc_set.resize(NUM_DESCRIPTOR_SETS);
    CALL_VK(vkAllocateDescriptorSets(oVulkanInfo.device_info.device_, allocateInfo, oVulkanInfo.desc_info.desc_set.data()));

    VkWriteDescriptorSet writes[1];
    writes[0] = {
            .sType =VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = oVulkanInfo.desc_info.desc_set[0],
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &oVulkanInfo.uniform_info.buffer_info,
            .dstArrayElement = 0,
            .dstBinding = 0,
    };
    vkUpdateDescriptorSets(oVulkanInfo.device_info.device_, 1, writes, 0, nullptr);

}