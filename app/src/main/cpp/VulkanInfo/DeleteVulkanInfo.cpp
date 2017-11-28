//
// Created by 川原　将太郎 on 2017/11/01.
//

#include "DeleteVulkanInfo.h"
#include "../VulkanUtil.h"

DeleteVulkanInfo::DeleteVulkanInfo() {}
DeleteVulkanInfo::~DeleteVulkanInfo() {}

void DeleteVulkanInfo::deleteSwapChain(VulkanInfo &oVulkanInfo) {

    vkFreeCommandBuffers(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.cmdPool_, oVulkanInfo.render_info.cmdBufferLen_,
                         oVulkanInfo.render_info.cmdBuffer_);
    delete[] oVulkanInfo.render_info.cmdBuffer_;

    for (size_t ii = 0; ii < oVulkanInfo.swapchain_info.swapchainLength_; ii++) {
        vkDestroyFramebuffer(oVulkanInfo.device_info.device_, oVulkanInfo.swapchain_info.framebuffers_[ii], nullptr);
        vkDestroyImageView(oVulkanInfo.device_info.device_, oVulkanInfo.swapchain_info.displayViews_[ii], nullptr);
    }
    delete[] oVulkanInfo.swapchain_info.framebuffers_;
    delete[] oVulkanInfo.swapchain_info.displayViews_;
    vkDestroySwapchainKHR(oVulkanInfo.device_info.device_, oVulkanInfo.swapchain_info.swapchain_, nullptr);
    deletePipeLine(oVulkanInfo);
}

void DeleteVulkanInfo::deletePipeLine(VulkanInfo &oVulkanInfo){
    vkDestroyPipeline(oVulkanInfo.device_info.device_, oVulkanInfo.gfx_pipeLine_info.pipeline_, nullptr);
    vkDestroyPipelineCache(oVulkanInfo.device_info.device_, oVulkanInfo.gfx_pipeLine_info.cache_,
                           nullptr);
    vkFreeDescriptorSets(oVulkanInfo.device_info.device_, oVulkanInfo.desc_info.desc_pool,1, oVulkanInfo.desc_info.desc_set.data());
    vkDestroyDescriptorPool(oVulkanInfo.device_info.device_, oVulkanInfo.desc_info.desc_pool,
                            nullptr);
    for(int ii = 0; ii <  oVulkanInfo.desc_info.desc_layout.size(); ii++){
        vkDestroyDescriptorSetLayout(oVulkanInfo.device_info.device_, oVulkanInfo.desc_info.desc_layout[ii],
                                     nullptr);
    }
    vkDestroyPipelineLayout(oVulkanInfo.device_info.device_, oVulkanInfo.gfx_pipeLine_info.layout_, nullptr);
    vkDestroyRenderPass(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.renderPass_, nullptr);

}

void DeleteVulkanInfo::deleteUniformBuffer(VulkanInfo &oVulkanInfo) {
    vkDestroyBuffer(oVulkanInfo.device_info.device_, oVulkanInfo.uniform_info.buf, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, oVulkanInfo.uniform_info.mem, nullptr);
}

void DeleteVulkanInfo::deleteTexture(VulkanInfo &oVulkanInfo){
    vkDestroySampler(oVulkanInfo.device_info.device_, oVulkanInfo.texture_object_info.sampler,
                     nullptr);
    vkDestroyImageView(oVulkanInfo.device_info.device_, oVulkanInfo.texture_object_info.view,
                       nullptr);
    vkDestroyImage(oVulkanInfo.device_info.device_, oVulkanInfo.texture_object_info.image, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, oVulkanInfo.texture_object_info.mem, nullptr);
}

void DeleteVulkanInfo::deleteIndexBuffer(VulkanInfo &oVulkanInfo) {
    vkDestroyBuffer(oVulkanInfo.device_info.device_, oVulkanInfo.index_info.buf, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, oVulkanInfo.index_info.mem, nullptr);
}

void DeleteVulkanInfo::deleteVertexBuffer(VulkanInfo &oVulkanInfo) {
    vkDestroyBuffer(oVulkanInfo.device_info.device_, oVulkanInfo.vertex_buffer_info.buf, nullptr);
    vkFreeMemory(oVulkanInfo.device_info.device_, oVulkanInfo.vertex_buffer_info.mem, nullptr);
}

void DeleteVulkanInfo::deleteSemaphore(VulkanInfo &oVulkanInfo) {
    vkDestroySemaphore(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.renderFinSemaphore_, nullptr);
    vkDestroySemaphore(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.imageAvailableSemaphore_, nullptr);
}

void DeleteVulkanInfo::deleteSurface(VulkanInfo &oVulkanInfo) {
    vkDestroyCommandPool(oVulkanInfo.device_info.device_, oVulkanInfo.render_info.cmdPool_, nullptr);
    vkDestroyDevice(oVulkanInfo.device_info.device_, nullptr);
    vkDestroySurfaceKHR(oVulkanInfo.device_info.instance_, oVulkanInfo.device_info.surface_, nullptr);
}
void DeleteVulkanInfo::deleteInstance(VulkanInfo &oVulkanInfo) {
    vkDestroyInstance(oVulkanInfo.device_info.instance_, nullptr);
}
