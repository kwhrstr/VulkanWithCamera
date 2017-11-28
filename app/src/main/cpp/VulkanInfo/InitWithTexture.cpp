//
// Created by 川原　将太郎 on 2017/10/25.
//

#include "InitWithTexture.h"
#include "../VulkanUtil.h"
#include "malloc.h"

InitWithTexture::InitWithTexture() {
}
InitWithTexture::~InitWithTexture() {
}
static const VkFormat kTexFmt = VK_FORMAT_R8G8B8A8_UNORM;

void InitWithTexture::initTexture(VulkanInfo &oVulkanInfo,
                                  VkImageUsageFlags iIsExtraUsages,
                                  VkFormatFeatureFlags iExtraFeatures ){
    textureObject texObj;
    initImage(oVulkanInfo, &texObj, iIsExtraUsages, iExtraFeatures);

    const VkSamplerCreateInfo sampler = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .maxAnisotropy = 1,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.f,
            .maxLod = 0.f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
    };
    CALL_VK(vkCreateSampler(oVulkanInfo.device_info.device_, &sampler, nullptr, &texObj.sampler));
    VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = VK_NULL_HANDLE,
            .format = kTexFmt,
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_R,
                    .g = VK_COMPONENT_SWIZZLE_G,
                    .b = VK_COMPONENT_SWIZZLE_B,
                    .a = VK_COMPONENT_SWIZZLE_A,
            },
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
            },
            .flags = 0,
    };
    view.image = texObj.image;
    CALL_VK(vkCreateImageView(oVulkanInfo.device_info.device_, &view, nullptr,&texObj.view ));
    oVulkanInfo.texture_object_info = texObj;
    oVulkanInfo.texture_image_info.image_info.imageView = oVulkanInfo.texture_object_info.view;
    oVulkanInfo.texture_image_info.image_info.sampler = oVulkanInfo.texture_object_info.sampler;
    oVulkanInfo.texture_image_info.image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

VkResult
InitWithTexture::initImage(VulkanInfo &oVulkanInfo,
                           textureObject* pTexObj,
                           VkImageUsageFlags iIsExtraUsages,
                           VkFormatFeatureFlags iExtraFeatures) {

//    uint32_t imgWidth = (uint32_t) oVulkanInfo.width;
//    uint32_t imgHeight = (uint32_t) oVulkanInfo.height;
    uint32_t imgWidth = 480;
    uint32_t imgHeight = 720;

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(oVulkanInfo.device_info.gpuDevices_[0], kTexFmt, &formatProps);

    VkFormatFeatureFlags allFeatures = (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | iExtraFeatures);
    bool isNeedStage = true;
    if(formatProps.linearTilingFeatures & allFeatures){
        isNeedStage = false;
    }

    unsigned char* imageData = new unsigned char [4 * imgHeight * imgHeight];

    uint32_t *imageDataPix = reinterpret_cast<uint32_t *>(imageData);
    // test make half blue and half red
    for (uint32_t ii = 0; ii < imgHeight; ii++) {
        for (uint32_t jj = 0; jj < imgWidth; jj++) {
            if (ii > 3* imgHeight/4 ) {
                imageDataPix[imgWidth * ii + jj ] = (ii%3==0) ? 0xFFFFFFFF : 0xFF0000FF;
            } else if (ii > imgHeight/2) {
                imageDataPix[imgWidth * ii + jj ] = (ii%3==0) ? 0xFFFFFFFF : 0xFF00FF00;
            } else if (ii > imgHeight/4) {
                imageDataPix[imgWidth * ii + jj ] = (ii%3==0) ? 0xFFFFFFFF : 0xFFFF0000;
            } else if (jj > imgWidth/2) {
                imageDataPix[imgWidth * ii + jj ] = (ii%3==0) ? 0xFF000000 : 0xFFFFFFFF;
            }
        }
    }

    pTexObj->tex_width = imgWidth;
    pTexObj->tex_height = imgHeight;
//    pTexObj->tex_width = imgWidth;
//    pTexObj->tex_height = imgHeight;

    // Allocate the linear texture so texture could be copied over
    VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = kTexFmt,
            .extent = {
                    .width = static_cast<uint32_t>(imgWidth),
                    .height = static_cast<uint32_t>(imgHeight),
                    .depth = 1,
            },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_LINEAR,
            .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
            .usage = (isNeedStage? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : VK_IMAGE_USAGE_SAMPLED_BIT) | iIsExtraUsages,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .flags = 0,
    };

    CALL_VK(vkCreateImage(oVulkanInfo.device_info.device_, &image_create_info, nullptr,
                          &pTexObj->image));

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(oVulkanInfo.device_info.device_, pTexObj->image, &mem_reqs);

    VkMemoryAllocateInfo mem_alloc = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 0,
            .memoryTypeIndex = 0,
    };
    mem_alloc.allocationSize = mem_reqs.size;

    bool pass = mapMemoryTypeToIndex(oVulkanInfo, mem_reqs.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                     &mem_alloc.memoryTypeIndex);
    assert(pass);
    CALL_VK(vkAllocateMemory(oVulkanInfo.device_info.device_, &mem_alloc, nullptr, &pTexObj->mem));
    CALL_VK(vkBindImageMemory(oVulkanInfo.device_info.device_, pTexObj->image, pTexObj->mem, 0));

    const VkImageSubresource subres = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .arrayLayer = 0,
    };
    VkSubresourceLayout layout;

    vkGetImageSubresourceLayout(oVulkanInfo.device_info.device_, pTexObj->image, &subres,
                                &layout);
    void* data;
    CALL_VK(vkMapMemory(oVulkanInfo.device_info.device_, pTexObj->mem, 0,
                        mem_alloc.allocationSize, 0, &data));
    oVulkanInfo.texture_image_info.rowPitch = (uint32_t) layout.rowPitch;
    oVulkanInfo.texture_image_info.textureSize = mem_alloc.allocationSize;
    for (uint32_t  y = 0; y < imgHeight; y++) {
        unsigned char *row = (unsigned char *) ((char *) data + layout.rowPitch * y);
        for (uint32_t x = 0; x < imgWidth; x++) {
            row[x * 4] = imageData[(x + y * imgWidth) * 4];
            row[x * 4 + 1] = imageData[(x + y * imgWidth) * 4 + 1];
            row[x * 4 + 2] = imageData[(x + y * imgWidth) * 4 + 2];
            row[x * 4 + 3] = imageData[(x + y * imgWidth) * 4 + 3];
        }
    }
//    vkUnmapMemory(oVulkanInfo.device_info.device_, pTexObj->mem);

    delete[] imageData;
    pTexObj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
     if(!isNeedStage){
        return  VK_SUCCESS;
    }

    // save current image and mem as staging image and memory
    VkImage stageImage = pTexObj->image;
    VkDeviceMemory stageMem = pTexObj->mem;
    pTexObj->image = VK_NULL_HANDLE;
    pTexObj->mem = VK_NULL_HANDLE;

    // Create a tile texture to blit into
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT
                              | VK_IMAGE_USAGE_SAMPLED_BIT;
    CALL_VK(vkCreateImage(oVulkanInfo.device_info.device_, &image_create_info, nullptr,
                          &pTexObj->image));
    vkGetImageMemoryRequirements(oVulkanInfo.device_info.device_, pTexObj->image, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;
    pass = mapMemoryTypeToIndex(oVulkanInfo,mem_reqs.memoryTypeBits,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                &mem_alloc.memoryTypeIndex);
    assert(pass);
    CALL_VK(vkAllocateMemory(oVulkanInfo.device_info.device_, &mem_alloc, nullptr, &pTexObj->mem));
    CALL_VK(vkBindImageMemory(oVulkanInfo.device_info.device_, pTexObj->image, pTexObj->mem, 0));
    oVulkanInfo.texture_image_info.textureSize = mem_alloc.allocationSize;

    VkCommandPoolCreateInfo cmdPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = oVulkanInfo.queue_info.graphics_queue_family_index,
    };

    VkCommandPool cmdPool;
    CALL_VK(vkCreateCommandPool(oVulkanInfo.device_info.device_, &cmdPoolCreateInfo, nullptr,
                                &cmdPool));

    VkCommandBuffer gfxCmd;
    const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    CALL_VK(vkAllocateCommandBuffers(oVulkanInfo.device_info.device_, &cmd, &gfxCmd));
    VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr};
    CALL_VK(vkBeginCommandBuffer(gfxCmd, &cmd_buf_info));

    VkImageCopy bltInfo = {
            .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .srcSubresource.mipLevel = 0,
            .srcSubresource.baseArrayLayer = 0,
            .srcSubresource.layerCount = 1,
            .srcOffset.x = 0,
            .srcOffset.y = 0,
            .srcOffset.z = 0,
            .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .dstSubresource.mipLevel = 0,
            .dstSubresource.baseArrayLayer = 0,
            .dstSubresource.layerCount = 1,
            .dstOffset.x = 0,
            .dstOffset.y = 0,
            .dstOffset.z = 0,
            .extent.width = imgWidth,
            .extent.height = imgHeight,
            .extent.depth = 1,
    };
    vkCmdCopyImage(gfxCmd, stageImage,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pTexObj->image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bltInfo);
    pTexObj->imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    CALL_VK(vkEndCommandBuffer(gfxCmd));
    VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    VkFence fence;
    CALL_VK(vkCreateFence(oVulkanInfo.device_info.device_, &fenceInfo, nullptr, &fence));

    VkSubmitInfo submitInfo = {
            .pNext = nullptr,
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfxCmd,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };
    CALL_VK(vkQueueSubmit(oVulkanInfo.queue_info.present_queue, 1, &submitInfo, fence) != VK_SUCCESS);
    CALL_VK(vkWaitForFences(oVulkanInfo.device_info.device_, 1, &fence, VK_TRUE, 100000000) !=
            VK_SUCCESS);
    vkDestroyFence(oVulkanInfo.device_info.device_, fence, nullptr);

    vkFreeCommandBuffers(oVulkanInfo.device_info.device_, cmdPool, 1, &gfxCmd);
    vkDestroyCommandPool(oVulkanInfo.device_info.device_, cmdPool, nullptr);
    vkDestroyImage(oVulkanInfo.device_info.device_, stageImage, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, stageMem, nullptr);
    return VK_SUCCESS;

}


void InitWithTexture::vInitDescriptorLayout(VulkanInfo &oVulkanInfo) {
    VkDescriptorSetLayoutBinding layoutBindings[1];
    layoutBindings[0] = {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
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

void InitWithTexture::vInitPipelineLayouts(VulkanInfo &oVulkanInfo) {
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

void InitWithTexture::vInitDescriptorPool(VulkanInfo &oVulkanInfo) {
    /* DEPENDS on init_uniform_buffer() and
      * init_descriptor_and_pipeline_layouts() */
    VkDescriptorPoolSize typeCount[1];
    typeCount[0] = {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
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


void InitWithTexture::vInitDescriptorSet(VulkanInfo &oVulkanInfo) {
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
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &oVulkanInfo.texture_image_info.image_info,
            .dstArrayElement = 0,
            .dstBinding = 0,
    };
    vkUpdateDescriptorSets(oVulkanInfo.device_info.device_, 1, writes, 0, nullptr);
}