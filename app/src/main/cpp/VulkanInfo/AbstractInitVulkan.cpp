//
// Created by 川原　将太郎 on 2017/10/10.
//

#include "AbstractInitVulkan.h"
#include <cstdlib>
#include <string.h>
#include <iostream>
#include "malloc.h"
#include "../VulkanUtil.h"

AbstractInitVulkan::AbstractInitVulkan() {}
AbstractInitVulkan::~AbstractInitVulkan() {}

VkResult AbstractInitVulkan::initGlobalLayerProp(VulkanInfo &oVulkanInfo) {
    uint32_t instance_layer_count;
    unique_ptr<VkLayerProperties[]> vk_props = nullptr;
    VkResult res;
    if(!InitVulkan()){
        LOGW("Vulkan is unavailable, install vulkan and re-start");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    do{
        res = vkEnumerateInstanceLayerProperties(&instance_layer_count,NULL);
        if(res) return res;
        if(instance_layer_count == 0){
            return VK_SUCCESS;
        }
        vk_props.reset(new VkLayerProperties[instance_layer_count]);
        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props.get());
    }while (res == VK_INCOMPLETE);

    for(uint32_t ii = 0; ii < instance_layer_count; ii++){
        layer_properties layer_props;
        layer_props.properties = vk_props[ii];
        res = initDeviceExtensionProps(layer_props);
        if(res){
            return res;
        }
        oVulkanInfo.instance_layer_properties.push_back(layer_props);
    }
    return res;
}

VkResult AbstractInitVulkan::initDeviceExtensionProps(layer_properties &layerProps) {
    VkExtensionProperties * instanceExtensions;
    uint32_t instanceExtensionCount;
    VkResult res;
    char* layerName = nullptr;

    layerName = layerProps.properties.layerName;
    do{
        res = vkEnumerateInstanceExtensionProperties(layerName, &instanceExtensionCount, NULL);
        if(res) return res;
        if(instanceExtensionCount == 0) return VK_SUCCESS;
        layerProps.extensions.resize(instanceExtensionCount);
        instanceExtensions = layerProps.extensions.data();
        res = vkEnumerateInstanceExtensionProperties(layerName,&instanceExtensionCount,instanceExtensions);
    }while (res == VK_INCOMPLETE);
    return res;
}

void AbstractInitVulkan::initVulkanInstance(VulkanInfo &oVulkanInfo) {
    oVulkanInfo.instance_extension_names.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    oVulkanInfo.instance_extension_names.emplace_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
    oVulkanInfo.device_extension_names.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkApplicationInfo appInfo ={
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .apiVersion = VK_MAKE_VERSION(1,0,0),
            .applicationVersion = VK_MAKE_VERSION(1,0,0),
            .engineVersion = VK_MAKE_VERSION(1,0,0),
            .pApplicationName = "vkTutorial",
            .pEngineName = "tutorial",
    };

    VkInstanceCreateInfo instanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(oVulkanInfo.instance_extension_names.size()),
            .ppEnabledExtensionNames = oVulkanInfo.instance_extension_names.data(),
            .enabledLayerCount = static_cast<uint32_t >(oVulkanInfo.instance_layer_names.size()),
            .ppEnabledLayerNames =oVulkanInfo.instance_extension_names.size() ? oVulkanInfo.instance_layer_names.data(): nullptr,
    };
    CALL_VK(vkCreateInstance(&instanceCreateInfo, nullptr,&oVulkanInfo.device_info.instance_))
}

void AbstractInitVulkan::initEnumerateDevice(VulkanInfo &oVulkanInfo) {
    uint32_t  gpuCount = 0;
    CALL_VK(vkEnumeratePhysicalDevices(oVulkanInfo.device_info.instance_,&gpuCount, nullptr));
    VkPhysicalDevice  tmpGpus[gpuCount];
    CALL_VK(vkEnumeratePhysicalDevices(oVulkanInfo.device_info.instance_, &gpuCount,tmpGpus));
    for(uint32_t ii = 0; ii < gpuCount; ii++ ){
        oVulkanInfo.device_info.gpuDevices_.push_back(tmpGpus[ii]);
    }
    vkGetPhysicalDeviceQueueFamilyProperties(oVulkanInfo.device_info.gpuDevices_[0],&oVulkanInfo.queue_info.queue_family_count,nullptr);
    assert(oVulkanInfo.queue_info.queue_family_count >= 1);

    VkQueueFamilyProperties tmpQueues[oVulkanInfo.queue_info.queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(oVulkanInfo.device_info.gpuDevices_[0],&oVulkanInfo.queue_info.queue_family_count,tmpQueues);

    oVulkanInfo.queue_info.queue_props.clear();
    for(uint32_t ii = 0; ii < oVulkanInfo.queue_info.queue_family_count ; ii++){
        oVulkanInfo.queue_info.queue_props.push_back(tmpQueues[ii]);
    }
    vkGetPhysicalDeviceMemoryProperties(oVulkanInfo.device_info.gpuDevices_[0],&oVulkanInfo.device_info.gpuMemoryProperties_);
    vkGetPhysicalDeviceProperties(oVulkanInfo.device_info.gpuDevices_[0],&oVulkanInfo.device_info.gpuDeviceProperties_);
}

void AbstractInitVulkan::initSurfaceAndQueueIndex(VulkanInfo &oVulkanInfo, ANativeWindow *pPlatformWindow) {
    VkAndroidSurfaceCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = pPlatformWindow,
    };
    CALL_VK(vkCreateAndroidSurfaceKHR(oVulkanInfo.device_info.instance_, &createInfo, nullptr,
                                      &oVulkanInfo.device_info.surface_));
    unique_ptr<VkBool32[]> pSupportsPresent (new VkBool32[oVulkanInfo.queue_info.queue_family_count]);
    for (uint32_t ii = 0; ii < oVulkanInfo.queue_info.queue_family_count; ii++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(oVulkanInfo.device_info.gpuDevices_[0], ii,
                                             oVulkanInfo.device_info.surface_,
                                             &pSupportsPresent[ii]);
    }

    oVulkanInfo.queue_info.graphics_queue_family_index = UINT32_MAX;
    oVulkanInfo.queue_info.present_queue_family_index = UINT32_MAX;
    for (uint32_t ii = 0; ii < oVulkanInfo.queue_info.queue_family_count; ii++) {
        if ((oVulkanInfo.queue_info.queue_props[ii].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
            continue;
        }
        if (oVulkanInfo.queue_info.graphics_queue_family_index == UINT32_MAX) {
            oVulkanInfo.queue_info.graphics_queue_family_index = ii;
        }
        if (pSupportsPresent[ii] == VK_TRUE) {
            oVulkanInfo.queue_info.graphics_queue_family_index = ii;
            oVulkanInfo.queue_info.present_queue_family_index = ii;
            break;
        }
    }
    if (oVulkanInfo.queue_info.present_queue_family_index == UINT32_MAX) {
        for (uint32_t ii = 0; oVulkanInfo.queue_info.queue_family_count; ii++) {
            if (pSupportsPresent[ii] == VK_TRUE) {
                oVulkanInfo.queue_info.present_queue_family_index = ii;
                break;
            }
        }
    }

    if (oVulkanInfo.queue_info.graphics_queue_family_index == UINT32_MAX ||
        oVulkanInfo.queue_info.present_queue_family_index == UINT32_MAX) {
        cout << "Could not find a queues for both graphics and present";
        exit(-1);
    }

}

void AbstractInitVulkan::initSwapChainExtension(VulkanInfo &oVulkanInfo) {
    memset(&oVulkanInfo.swapchain_info, 0, sizeof(oVulkanInfo.swapchain_info));
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(oVulkanInfo.device_info.gpuDevices_[0],
                                         oVulkanInfo.device_info.surface_, &formatCount, nullptr);
    unique_ptr<VkSurfaceFormatKHR[]> formats(new VkSurfaceFormatKHR[formatCount]);
    vkGetPhysicalDeviceSurfaceFormatsKHR(oVulkanInfo.device_info.gpuDevices_[0],
                                         oVulkanInfo.device_info.surface_, &formatCount, formats.get());
    LOGI("Got %d formats", formatCount);
    uint32_t chosenFormat;
    for (chosenFormat = 0; chosenFormat < formatCount; chosenFormat++) {
        if (formats[chosenFormat].format == VK_FORMAT_R8G8B8A8_UNORM)break;
    }
    assert(chosenFormat < formatCount);
    oVulkanInfo.swapchain_info.displayFormat_ = formats[chosenFormat].format;
    oVulkanInfo.swapchain_info.imageColorSpace_ = formats[chosenFormat].colorSpace;
}

void AbstractInitVulkan::initVulkanDevice(VulkanInfo &oVulkanInfo) {
    float priorities[1] = {1.f};
    VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .queueCount = 1,
            .flags = 0,
            .queueFamilyIndex = oVulkanInfo.queue_info.graphics_queue_family_index,
            .pQueuePriorities = priorities,
    };

    VkDeviceCreateInfo deviceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .queueCreateInfoCount =  1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(oVulkanInfo.device_extension_names.size()),
            .ppEnabledExtensionNames = oVulkanInfo.device_extension_names.data(),
            .pEnabledFeatures = nullptr,
    };
    CALL_VK(vkCreateDevice(oVulkanInfo.device_info.gpuDevices_[0], &deviceCreateInfo, nullptr, &oVulkanInfo.device_info.device_));
}

void AbstractInitVulkan::initDeviceQueue(VulkanInfo &oVulkanInfo) {
    vkGetDeviceQueue(oVulkanInfo.device_info.device_,oVulkanInfo.queue_info.graphics_queue_family_index,0, &oVulkanInfo.queue_info.graphics_queue);
    if(oVulkanInfo.queue_info.graphics_queue_family_index == oVulkanInfo.queue_info.present_queue_family_index){
        oVulkanInfo.queue_info.present_queue = oVulkanInfo.queue_info.graphics_queue;
    } else {
        vkGetDeviceQueue(oVulkanInfo.device_info.device_,oVulkanInfo.queue_info.present_queue_family_index,0, &oVulkanInfo.queue_info.present_queue);
    }
}

void AbstractInitVulkan::initSwapChain(VulkanInfo &oVulkanInfo, VkImageUsageFlags iUsageFlags) {
    LOGI("->initSwapChain");
    VkSurfaceCapabilitiesKHR surfaceCapabilitiesKHR;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(oVulkanInfo.device_info.gpuDevices_[0],oVulkanInfo.device_info.surface_,&surfaceCapabilitiesKHR);

    uint32_t presentModeCount = 0;
    CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(oVulkanInfo.device_info.gpuDevices_[0],
                                              oVulkanInfo.device_info.surface_,&presentModeCount, nullptr));
    VkPresentModeKHR* presentMode= new VkPresentModeKHR[presentModeCount];
    CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(oVulkanInfo.device_info.gpuDevices_[0],
                                                      oVulkanInfo.device_info.surface_,&presentModeCount, presentMode));
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    for (uint32_t ii = 0; ii < presentModeCount; ii++) {
        swapchainPresentMode = presentMode[ii];
        if (swapchainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            break;
        }
    }
    delete[]presentMode;


    VkExtent2D swapChainExtent;
    if(surfaceCapabilitiesKHR.currentExtent.width == 0xFFFFFFFF){
        swapChainExtent.width = static_cast<uint32_t > (oVulkanInfo.width);
        if (swapChainExtent.width < surfaceCapabilitiesKHR.minImageExtent.width) {
            swapChainExtent.width = surfaceCapabilitiesKHR.minImageExtent.width;
        } else if (swapChainExtent.width > surfaceCapabilitiesKHR.maxImageExtent.width) {
            swapChainExtent.width = surfaceCapabilitiesKHR.maxImageExtent.width;
        }
        swapChainExtent.height = static_cast<uint32_t > (oVulkanInfo.height);
        if (swapChainExtent.height < surfaceCapabilitiesKHR.minImageExtent.height) {
            swapChainExtent.height = surfaceCapabilitiesKHR.minImageExtent.height;
        } else if (swapChainExtent.height > surfaceCapabilitiesKHR.maxImageExtent.height) {
            swapChainExtent.height = surfaceCapabilitiesKHR.maxImageExtent.height;
        }
    }else{
        swapChainExtent = surfaceCapabilitiesKHR.currentExtent;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if(surfaceCapabilitiesKHR.supportedTransforms &VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR){
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }else{
        preTransform = surfaceCapabilitiesKHR.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for(uint32_t ii = 0 ; ii < sizeof(compositeAlphaFlags); ii++){
        if(surfaceCapabilitiesKHR.supportedCompositeAlpha & compositeAlphaFlags[ii]){
            compositeAlpha = compositeAlphaFlags[ii];
            break;
        }
    }

    bool isDiff= false;
    uint32_t queueFamilyIndices[2] = {
            static_cast<uint32_t > (oVulkanInfo.queue_info.graphics_queue_family_index),
            static_cast<uint32_t > (oVulkanInfo.queue_info.present_queue_family_index),
    };
    if(queueFamilyIndices[0] != queueFamilyIndices[1]){
        isDiff = true;
    }
    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .surface = oVulkanInfo.device_info.surface_,
            .minImageCount = surfaceCapabilitiesKHR.minImageCount,
            .imageFormat = oVulkanInfo.swapchain_info.displayFormat_,
            .imageColorSpace = oVulkanInfo.swapchain_info.imageColorSpace_,
            .imageExtent = swapChainExtent,
            .imageUsage = iUsageFlags,
            .preTransform = preTransform,
            .compositeAlpha = compositeAlpha,
            .imageArrayLayers = 1,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = static_cast<uint32_t >(isDiff? 2:0),
            .pQueueFamilyIndices = isDiff? queueFamilyIndices: nullptr,
            .presentMode = swapchainPresentMode,
            .oldSwapchain = VK_NULL_HANDLE,
            .clipped = VK_TRUE,
    };
    oVulkanInfo.swapchain_info.displaySize_ = swapChainExtent;
    CALL_VK(vkCreateSwapchainKHR(oVulkanInfo.device_info.device_, &swapchainCreateInfo, nullptr,&oVulkanInfo.swapchain_info.swapchain_));
    LOGI("<-initSwapChain");
}

void AbstractInitVulkan::initSwapChainViews(VulkanInfo &oVulkanInfo){
    uint32_t  swapChainImagesCount = 0;
    CALL_VK(vkGetSwapchainImagesKHR(oVulkanInfo.device_info.device_, oVulkanInfo.swapchain_info.swapchain_,&swapChainImagesCount, nullptr));
    unique_ptr<VkImage[]> displayImages(new VkImage[swapChainImagesCount]);
    CALL_VK(vkGetSwapchainImagesKHR(oVulkanInfo.device_info.device_, oVulkanInfo.swapchain_info.swapchain_,&swapChainImagesCount, displayImages.get()));
    oVulkanInfo.swapchain_info.swapchainLength_ = swapChainImagesCount;
    oVulkanInfo.swapchain_info.displayViews_ = new VkImageView[swapChainImagesCount];
    for(uint32_t ii = 0;  ii < swapChainImagesCount; ii++){
        VkImageViewCreateInfo viewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .image = displayImages[ii],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = oVulkanInfo.swapchain_info.displayFormat_,
                .components =
                        {
                                .r = VK_COMPONENT_SWIZZLE_R,
                                .g = VK_COMPONENT_SWIZZLE_G,
                                .b = VK_COMPONENT_SWIZZLE_B,
                                .a = VK_COMPONENT_SWIZZLE_A,
                        },
                .subresourceRange =
                        {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1,
                        },
                .flags = 0,
        };
        CALL_VK(vkCreateImageView(oVulkanInfo.device_info.device_,&viewCreateInfo, nullptr,&oVulkanInfo.swapchain_info.displayViews_[ii]))
    }
}

void AbstractInitVulkan::initDepthBuffer(VulkanInfo &oVulkanInfo) {
    const VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    oVulkanInfo.depth_info.format = depthFormat;
    VkImageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthFormat,
            .extent =
                    {
                            .width = oVulkanInfo.swapchain_info.displaySize_.width,
                            .height = oVulkanInfo.swapchain_info.displaySize_.height,
                            .depth = 1,
                    },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = NUM_SAMPLES,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .flags = 0,
    };
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(oVulkanInfo.device_info.gpuDevices_[0], depthFormat, &props);
    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        createInfo.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else {
        /* Try other depth formats? */
        std::cout << "depth_format " << depthFormat << " Unsupported.\n";
        exit(-1);
    }
    CALL_VK(vkCreateImage(oVulkanInfo.device_info.device_,&createInfo, nullptr, &oVulkanInfo.depth_info.image))

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(oVulkanInfo.device_info.device_, oVulkanInfo.depth_info.image, &memReqs);
    VkMemoryAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memReqs.size,
            .memoryTypeIndex = 0,
    };
    bool pass = mapMemoryTypeToIndex(oVulkanInfo, memReqs.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                     &allocateInfo.memoryTypeIndex);
    assert(pass);
    CALL_VK(vkAllocateMemory(oVulkanInfo.device_info.device_, &allocateInfo, nullptr, &oVulkanInfo.depth_info.mem));
    CALL_VK(vkBindImageMemory(oVulkanInfo.device_info.device_, oVulkanInfo.depth_info.image, oVulkanInfo.depth_info.mem, 0));

    VkImageViewCreateInfo imageViewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .image = oVulkanInfo.depth_info.image,
            .format = depthFormat,
            .components =
                    {
                            .r = VK_COMPONENT_SWIZZLE_R,
                            .g = VK_COMPONENT_SWIZZLE_G,
                            .b = VK_COMPONENT_SWIZZLE_B,
                            .a = VK_COMPONENT_SWIZZLE_A,
                    },
            .subresourceRange =
                    {
                            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT,
                            .baseMipLevel = 0,
                            .levelCount = 1,
                            .baseArrayLayer = 0,
                            .layerCount = 1,
                    },
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .flags = 0,
    };
    CALL_VK(vkCreateImageView(oVulkanInfo.device_info.device_,&imageViewCreateInfo, nullptr,&oVulkanInfo.depth_info.view));
}

void AbstractInitVulkan::initIndexBuffer(VulkanInfo &oVulkanInfo) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    bufferInfo tmpBuffer;
    initBuffer(oVulkanInfo,
               bufferSize,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               tmpBuffer);

    void *data;
    vkMapMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem);

    initBuffer(oVulkanInfo,
               bufferSize,
               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               oVulkanInfo.index_info);

    copyBuffer(oVulkanInfo, tmpBuffer.buf, oVulkanInfo.index_info.buf, bufferSize);

    vkDestroyBuffer(oVulkanInfo.device_info.device_, tmpBuffer.buf, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem, nullptr);
}


void AbstractInitVulkan::initUniformBuffers(VulkanInfo &oVulkanInfo){
    uint32_t  bufferSize = sizeof(oVulkanInfo.glm_info.MVP);
    initBuffer(oVulkanInfo,
               bufferSize,
               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               oVulkanInfo.uniform_info);

    oVulkanInfo.uniform_info.buffer_info.buffer = oVulkanInfo.uniform_info.buf;
    oVulkanInfo.uniform_info.buffer_info.offset = 0;
    oVulkanInfo.uniform_info.buffer_info.range = bufferSize;
}

void AbstractInitVulkan::vInitPipelineLayouts(VulkanInfo &oVulkanInfo) {
    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };
    CALL_VK(vkCreatePipelineLayout(oVulkanInfo.device_info.device_, &pipelineLayoutCreateInfo,
                                   nullptr, &oVulkanInfo.gfx_pipeLine_info.layout_));
}




void AbstractInitVulkan::initRenderPass(VulkanInfo &oVulkanInfo, bool iIsIncludeDepth,bool iIsClear, VkImageLayout iFinalLayout){
    LOGI("->initRenderPass");
    VkAttachmentDescription attachmentDescriptions[2];
    attachmentDescriptions[0] = {
            .format = oVulkanInfo.swapchain_info.displayFormat_,
            .samples = NUM_SAMPLES,
            .loadOp =iIsClear? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = iFinalLayout,
            .flags = 0,
    };
    if(iIsIncludeDepth) {
        attachmentDescriptions[1] = {
                .format = oVulkanInfo.depth_info.format,
                .samples = NUM_SAMPLES,
                .loadOp =iIsClear? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .flags = 0,
        };
    }
    VkAttachmentReference colourReference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    VkAttachmentReference depthReference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    VkSubpassDescription subpassDescription{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .flags = 0,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colourReference,
            .pDepthStencilAttachment = iIsIncludeDepth? &depthReference: nullptr,
            .pResolveAttachments = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
    };
    VkRenderPassCreateInfo renderPassCreateInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .attachmentCount = static_cast<uint32_t >(iIsIncludeDepth ? 2 :1),
            .pAttachments = attachmentDescriptions,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 0,
            .pDependencies = nullptr,
    };
    CALL_VK(vkCreateRenderPass(oVulkanInfo.device_info.device_, &renderPassCreateInfo, nullptr,&oVulkanInfo.render_info.renderPass_));
}

void AbstractInitVulkan::initFrameBuffers(VulkanInfo &oVulkanInfo, bool iIsIncludeDepth) {
    /* DEPENDS on init_depth_buffer(), init_renderpass() and
     * init_swapchain_extension() */
    oVulkanInfo.swapchain_info.framebuffers_ = new VkFramebuffer[oVulkanInfo.swapchain_info.swapchainLength_];
    for(uint32_t ii = 0; ii < oVulkanInfo.swapchain_info.swapchainLength_; ii++){
        VkImageView attachments[2]={
                oVulkanInfo.swapchain_info.displayViews_[ii], oVulkanInfo.depth_info.view
        };
        VkFramebufferCreateInfo fbCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .renderPass = oVulkanInfo.render_info.renderPass_,
                .layers = 1,
                .attachmentCount = static_cast<uint32_t >(iIsIncludeDepth ? 2 : 1),  // 2 if using depth
                .pAttachments = attachments,
                .width = static_cast<uint32_t >(oVulkanInfo.swapchain_info.displaySize_.width),
                .height = static_cast<uint32_t >(oVulkanInfo.swapchain_info.displaySize_.height),
        };
        CALL_VK(vkCreateFramebuffer(oVulkanInfo.device_info.device_,&fbCreateInfo, nullptr,&oVulkanInfo.swapchain_info.framebuffers_[ii]));
    }
}

void AbstractInitVulkan::initVertexBuffer(VulkanInfo &oVulkanInfo, const void* iVertexData, uint32_t iDataSize, uint32_t iDataStride, bool iIsUseTexture){
    VkDeviceSize bufferSize = iDataStride * iDataSize;
    bufferInfo tmpBuffer;
    initBuffer(oVulkanInfo, bufferSize,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               tmpBuffer);
    uint8_t *pData;
    CALL_VK(vkMapMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem, 0, bufferSize, 0, (void **)&pData));
    memcpy(pData, iVertexData, bufferSize);
    vkUnmapMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem);

    initBuffer(oVulkanInfo, bufferSize,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               oVulkanInfo.vertex_buffer_info);

    copyBuffer(oVulkanInfo, tmpBuffer.buf, oVulkanInfo.vertex_buffer_info.buf, bufferSize);
    oVulkanInfo.vertex_buffer_info.buffer_info = {
            .buffer = oVulkanInfo.vertex_buffer_info.buf,
            .offset = 0,
            .range = bufferSize
    };

    oVulkanInfo.vi_binding = {
            .binding = 0,
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            .stride = iDataStride,
    };
    oVulkanInfo.vi_attributes[0] = {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = 0,
    };
    oVulkanInfo.vi_attributes[1] = {
            .binding = 0,
            .location = 1,
            .format = iIsUseTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = 4 * sizeof(float),
    };

    vkDestroyBuffer(oVulkanInfo.device_info.device_, tmpBuffer.buf, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, tmpBuffer.mem, nullptr);

}


void AbstractInitVulkan::initPipeLineCache(VulkanInfo &oVulkanInfo) {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = nullptr,
            .initialDataSize = 0,
            .pInitialData = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreatePipelineCache(oVulkanInfo.device_info.device_, &pipelineCacheCreateInfo, nullptr,&oVulkanInfo.gfx_pipeLine_info.cache_));
}

void AbstractInitVulkan::initPipeLine(VulkanInfo &oVulkanInfo , VkBool32 iIsIncludeDepth, VkBool32 iInclude_vi) {
    VkDynamicState dynamicStateEnable[VK_DYNAMIC_STATE_RANGE_SIZE];
    memset(dynamicStateEnable,0, sizeof(dynamicStateEnable));
    VkPipelineDynamicStateCreateInfo dynamicState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .pDynamicStates = dynamicStateEnable,
            .dynamicStateCount = 0,
    };

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputState;
    if(iInclude_vi) {
        memset(&pipelineVertexInputState, 0, sizeof(pipelineVertexInputState));
        pipelineVertexInputState = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &oVulkanInfo.vi_binding,
                .vertexAttributeDescriptionCount = 2,
                .pVertexAttributeDescriptions = oVulkanInfo.vi_attributes,
        };
    }

    VkPipelineInputAssemblyStateCreateInfo ia = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .primitiveRestartEnable = VK_FALSE,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkPipelineRasterizationStateCreateInfo rs = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0,
            .depthBiasSlopeFactor = 0,
            .depthBiasClamp = 0,
            .lineWidth = 1.0f,
    };
    VkPipelineColorBlendAttachmentState attState[1] = {
            {
                    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                    .blendEnable = VK_FALSE,
                    .colorBlendOp = VK_BLEND_OP_ADD,
                    .alphaBlendOp = VK_BLEND_OP_ADD,
                    .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            }
    };


    VkPipelineColorBlendStateCreateInfo cb = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = attState,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .logicOpEnable = VK_FALSE,
            .blendConstants[0] = 1.0f,
            .blendConstants[1] = 1.0f,
            .blendConstants[2] = 1.0f,
            .blendConstants[3] = 1.0f,
    };

    VkViewport viewport = {
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
            .x = 0,
            .y = 0,
            .width = static_cast<float >(oVulkanInfo.width),
            .height = static_cast<float >(oVulkanInfo.height),
    };
    VkRect2D scissor = {
            .extent = {
                    .width = static_cast<uint32_t >(oVulkanInfo.width),
                    .height = static_cast<uint32_t >(oVulkanInfo.height),
            },
            .offset = {
                    .x = 0, .y = 0,
            },
    };
    VkPipelineViewportStateCreateInfo vp={
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = NUM_VIEWPORTS,
            .scissorCount = NUM_SCISSORS,
            .pScissors = &scissor,
            .pViewports = &viewport,
    };

    VkPipelineDepthStencilStateCreateInfo ds = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = iIsIncludeDepth,
            .depthWriteEnable = iIsIncludeDepth,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .back = {
                    .compareOp = VK_COMPARE_OP_ALWAYS,
                    .compareMask = 0,
                    .failOp = VK_STENCIL_OP_KEEP,
                    .passOp = VK_STENCIL_OP_KEEP,
                    .reference = 0,
                    .depthFailOp = VK_STENCIL_OP_KEEP,
                    .writeMask = 0,
            },
            .minDepthBounds = 0,
            .maxDepthBounds = 0,
    };
    ds.front = ds.back;

    VkPipelineMultisampleStateCreateInfo ms = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pSampleMask = nullptr,
            .rasterizationSamples =  NUM_SAMPLES,
            .sampleShadingEnable = VK_FALSE,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
            .minSampleShading = 0.0,
    };

    VkGraphicsPipelineCreateInfo pipeline = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .layout = oVulkanInfo.gfx_pipeLine_info.layout_,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
            .flags = 0,
            .pVertexInputState = &pipelineVertexInputState,
            .pInputAssemblyState = &ia,
            .pRasterizationState = &rs,
            .pColorBlendState = &cb,
            .pTessellationState = nullptr,
            .pMultisampleState = &ms,
            .pDynamicState = &dynamicState,
            .pViewportState = &vp,
            .pDepthStencilState = &ds,
            .pStages = oVulkanInfo.shaderStages,
            .stageCount = 2,
            .renderPass = oVulkanInfo.render_info.renderPass_,
            .subpass = 0,
    };
    VkResult res = vkCreateGraphicsPipelines(oVulkanInfo.device_info.device_, oVulkanInfo.gfx_pipeLine_info.cache_, 1,
                                             &pipeline, nullptr, &oVulkanInfo.gfx_pipeLine_info.pipeline_ );
    assert(res == VK_SUCCESS);
    vkDestroyShaderModule(oVulkanInfo.device_info.device_, oVulkanInfo.shaderStages[0].module, nullptr);
    vkDestroyShaderModule(oVulkanInfo.device_info.device_, oVulkanInfo.shaderStages[1].module, nullptr);
}

void AbstractInitVulkan::initCommandPool(VulkanInfo &oVulkanInfo){
    /* DEPENDS on initSwapChainExtension */
    VkCommandPoolCreateInfo cmdCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .queueFamilyIndex = oVulkanInfo.queue_info.graphics_queue_family_index,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };
    CALL_VK(vkCreateCommandPool(oVulkanInfo.device_info.device_,&cmdCreateInfo, nullptr,&oVulkanInfo.render_info.cmdPool_))
}

void AbstractInitVulkan::initCommandBuffer(VulkanInfo &oVulkanInfo) {
    /* DEPENDS on initSwapChainExtension */
    oVulkanInfo.render_info.cmdBufferLen_ = oVulkanInfo.swapchain_info.swapchainLength_;
    oVulkanInfo.render_info.cmdBuffer_ = new VkCommandBuffer[oVulkanInfo.swapchain_info.swapchainLength_];
    VkCommandBufferAllocateInfo allocateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = oVulkanInfo.render_info.cmdPool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = oVulkanInfo.render_info.cmdBufferLen_,
    };
    CALL_VK(vkAllocateCommandBuffers(oVulkanInfo.device_info.device_,&allocateInfo,oVulkanInfo.render_info.cmdBuffer_));
    for(uint32_t ii = 0; ii < oVulkanInfo.render_info.cmdBufferLen_; ii++){
        VkCommandBufferBeginInfo beginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                .pInheritanceInfo = nullptr, // Optional
        };

        vkBeginCommandBuffer(oVulkanInfo.render_info.cmdBuffer_[ii], &beginInfo);

        VkClearValue clearColor = {
                .color.float32[0] = 0.2f,
                .color.float32[1] = 0.2f,
                .color.float32[2] = 0.2f,
                .color.float32[3] = 1.f,
        };
        VkRenderPassBeginInfo renderPassInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = oVulkanInfo.render_info.renderPass_,
                .framebuffer = oVulkanInfo.swapchain_info.framebuffers_[ii],
                .renderArea.offset = {0, 0},
                .renderArea.extent = oVulkanInfo.swapchain_info.displaySize_,
                .clearValueCount = 1,
                .pClearValues = &clearColor,
        };
        vkCmdBeginRenderPass(oVulkanInfo.render_info.cmdBuffer_[ii], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(oVulkanInfo.render_info.cmdBuffer_[ii], VK_PIPELINE_BIND_POINT_GRAPHICS, oVulkanInfo.gfx_pipeLine_info.pipeline_);
        VkBuffer vertexBuffers[] = {oVulkanInfo.vertex_buffer_info.buf};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(oVulkanInfo.render_info.cmdBuffer_[ii], 0, 1, vertexBuffers, offsets);

        vkCmdBindDescriptorSets(oVulkanInfo.render_info.cmdBuffer_[ii], VK_PIPELINE_BIND_POINT_GRAPHICS, oVulkanInfo.gfx_pipeLine_info.layout_,
                                0, 1, oVulkanInfo.desc_info.desc_set.data(), 0, nullptr);

        vkCmdDraw(oVulkanInfo.render_info.cmdBuffer_[ii], 6, 1, 0, 0);

        vkCmdEndRenderPass(oVulkanInfo.render_info.cmdBuffer_[ii]);

        CALL_VK( vkEndCommandBuffer(oVulkanInfo.render_info.cmdBuffer_[ii]) );
    }
}

void AbstractInitVulkan::initSemaphores(VulkanInfo &oVulkanInfo) {
    // We need to create a fence to be able, in the main loop, to wait for our
    // draw command(s) to finish before swapping the framebuffers
    VkSemaphoreCreateInfo semaphoreCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
    };
    CALL_VK(vkCreateSemaphore(oVulkanInfo.device_info.device_,&semaphoreCreateInfo, nullptr,
                              &oVulkanInfo.render_info.renderFinSemaphore_));
    CALL_VK(vkCreateSemaphore(oVulkanInfo.device_info.device_,&semaphoreCreateInfo, nullptr,
                              &oVulkanInfo.render_info.imageAvailableSemaphore_));

}
