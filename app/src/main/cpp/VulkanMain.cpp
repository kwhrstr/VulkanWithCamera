//
// Created by 川原　将太郎 on 2017/10/03.
//

#include "VulkanMain.h"
#include "VulkanInfo/InItWithUniform.h"
#include "VulkanInfo/DeleteVulkanInfo.h"
#include <chrono>
#include "cube_data.h"
#include "VulkanUtil.h"
#include "VulkanInfo/InitWithTexture.h"
#include "NativeCamera/NativeCamera.h"
#include "NativeCamera/ImageReader.h"
#include <android/native_window_jni.h>
#include <string>


// Android Native App pointer...
VulkanInfo mVulkanInfo;
camera_type m_selected_camera_type = BACK_CAMERA; // Default

// Image Reader
ImageFormat m_view{0, 0, 0};
unique_ptr<ImageReader> m_image_reader;
AImage *m_image;
unique_ptr<NativeCamera> mNativeCamera;

VulkanMain::~VulkanMain() {
    if (m_image) AImage_delete(m_image);
    ANativeWindow_release(offscreenWindow);
}

void VulkanMain::init(ANativeWindow *pWindow) {
    LOGI("initVulkan Vulkan Start");
    initVulkanInfo(mVulkanInfo, pWindow);
}

void VulkanMain::setOffScreenWindow(ANativeWindow *pWindow) {
    LOGI("initVulkan Vulkan Start");
    offscreenWindow = pWindow;
    ANativeWindow_setBuffersGeometry(offscreenWindow, 480,
                                     720, WINDOW_FORMAT_RGBA_8888);

}

bool VulkanMain::run() {
    return mainLoop();
}

void VulkanMain::resume(ANativeWindow *window) {
    state = STATE_RUNNING;
}

void VulkanMain::surfaceChanged() {
    state = STATE_PAUSED;
    reInitSwapChain();
    state = STATE_RUNNING;
}

void VulkanMain::pause() {
    LOGI("pause!!!!!!");
    state = STATE_PAUSED;
    vkDeviceWaitIdle(mVulkanInfo.device_info.device_);
}


bool VulkanMain::initNativeCamera() {
    mNativeCamera = make_unique<NativeCamera>(BACK_CAMERA);
    mNativeCamera->MatchCaptureSizeRequest(&m_view, 480, 720);
    m_image_reader = make_unique<ImageReader>(&m_view, AIMAGE_FORMAT_YUV_420_888);

    m_image_reader->SetPresentRotation(mNativeCamera->GetOrientation());
    ANativeWindow *image_reader_window = m_image_reader->GetNativeWindow();
    return mNativeCamera->CreateCaptureSession(image_reader_window);
}

static const VertexUV vertexUV[] = {
        {XYZ1(-1.f, -1.f, 0),  UV(0.f, 0.f)},
        {XYZ1( 1.f, -1.f, 0),  UV(1.f, 0.f)},
        {XYZ1(-1.f,  1.f, 0),  UV(0.f, 1.f)},
        {XYZ1( 1.f,  1.f, 0),  UV(1.f, 1.f)},
        {XYZ1( 1.f, -1.f, 0),  UV(1.f, 0.f)},
        {XYZ1(-1.f,  1.f, 0),  UV(0.f, 1.f)},
};


auto init_vulkan = make_unique<InitWithTexture>();
auto delete_vulkan = make_unique<DeleteVulkanInfo>();

void VulkanMain::initInstance() {
    init_vulkan->initGlobalLayerProp(mVulkanInfo);
    init_vulkan->initVulkanInstance(mVulkanInfo);
    init_vulkan->initEnumerateDevice(mVulkanInfo);
}

void VulkanMain::initVulkanInfo(VulkanInfo &oVulkanInfo, ANativeWindow *pWindow) {
    oVulkanInfo.width = ANativeWindow_getWidth(pWindow);
    oVulkanInfo.height = ANativeWindow_getHeight(pWindow);
    initWindow(pWindow);
    bool isIncludeDepth = false;
    init_vulkan->initSurfaceAndQueueIndex(oVulkanInfo, window);
    init_vulkan->initSwapChainExtension(oVulkanInfo);
    init_vulkan->initVulkanDevice(oVulkanInfo);
    init_vulkan->initDeviceQueue(oVulkanInfo);
    init_vulkan->initSwapChain(oVulkanInfo);
    init_vulkan->initSwapChainViews(oVulkanInfo);
    init_vulkan->vInitDescriptorLayout(oVulkanInfo);
    init_vulkan->vInitPipelineLayouts(oVulkanInfo);
    init_vulkan->initRenderPass(oVulkanInfo, isIncludeDepth);
    init_vulkan->initCommandPool(oVulkanInfo);

    init_vulkan->initFrameBuffers(oVulkanInfo, isIncludeDepth);
    init_vulkan->initVertexBuffer(oVulkanInfo, vertexUV, sizeof(vertexUV), sizeof(vertexUV[0]),
                                  true);

    init_vulkan->initPipeLineCache(oVulkanInfo);
    initShader(oVulkanInfo, vertexShader.c_str(), VERTEX_SHADER);
    initShader(oVulkanInfo, fragmentShader.c_str(), FRAGMENT_SHADER);
    init_vulkan->initPipeLine(oVulkanInfo, (VkResult) isIncludeDepth);
}

void VulkanMain::initTexture() {
    init_vulkan->initTexture(mVulkanInfo);
    init_vulkan->vInitDescriptorPool(mVulkanInfo);
    init_vulkan->vInitDescriptorSet(mVulkanInfo);
    init_vulkan->initCommandBuffer(mVulkanInfo);
    init_vulkan->initSemaphores(mVulkanInfo);
}

void VulkanMain::initWindow(ANativeWindow* pWindow) {
    window = pWindow;
//    ANativeWindow_setBuffersGeometry(window, 2268,
//                                     4032, WINDOW_FORMAT_RGBA_8888);
//    ANativeWindow_setBuffersGeometry(window, mVulkanInfo.width,
//                                     mVulkanInfo.height, WINDOW_FORMAT_RGBA_8888);

}


bool VulkanMain::mainLoop() {

    if (m_image_reader->GetBufferCount() == 0) {
        return true;
    } else {
        m_image_reader->DecBufferCount();
    }
    updateTexture(mVulkanInfo);
    VkResult res = drawFrame(mVulkanInfo);
    return !(res != VK_SUCCESS);
//    return true;
}

void VulkanMain::updateUniformBuffer(VulkanInfo &oVulkanInfo) {
    static auto startTime = chrono::high_resolution_clock::now();
    auto currentTime = chrono::high_resolution_clock::now();

    float time =
            chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count() / 1e3f;

    oVulkanInfo.glm_info.Model = glm::rotate(glm::mat4(1.f), time * glm::radians(90.0f),
                                             glm::vec3(0.0f, 0.0f, 1.0f));
    oVulkanInfo.glm_info.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, 1.0f));
    oVulkanInfo.glm_info.Projection = glm::perspective(glm::radians(45.0f),
                                                       oVulkanInfo.swapchain_info.displaySize_.width /
                                                       (float) oVulkanInfo.swapchain_info.displaySize_.height,
                                                       0.1f,
                                                       10.0f);
    oVulkanInfo.glm_info.Clip = glm::mat4(1.f);
    oVulkanInfo.glm_info.Projection[1][1] *= -1;
    oVulkanInfo.glm_info.MVP = glm::operator*(oVulkanInfo.glm_info.Clip,
                                              oVulkanInfo.glm_info.Projection);
    oVulkanInfo.glm_info.MVP = glm::operator*(oVulkanInfo.glm_info.MVP, oVulkanInfo.glm_info.View);
    oVulkanInfo.glm_info.MVP = glm::operator*(oVulkanInfo.glm_info.MVP, oVulkanInfo.glm_info.Model);

    void *data;
    vkMapMemory(oVulkanInfo.device_info.device_, oVulkanInfo.uniform_info.mem, 0,
                sizeof(oVulkanInfo.glm_info.MVP), 0, &data);
    memcpy(data, &oVulkanInfo.glm_info.MVP, sizeof(oVulkanInfo.glm_info.MVP));
    vkUnmapMemory(oVulkanInfo.device_info.device_, oVulkanInfo.uniform_info.mem);
}

void VulkanMain::updateTexture(VulkanInfo &oVulkanInfo) {

    m_image = m_image_reader->GetLatestImage();
    if (m_image == nullptr)
        return;
    ANativeWindow_acquire(offscreenWindow);
    ANativeWindow_Buffer buf;
    if (ANativeWindow_lock(offscreenWindow, &buf, nullptr) < 0) {
        m_image_reader->DeleteImage(m_image);
        return;
    }

    m_image_reader->DisplayImage(&buf, m_image);
    ANativeWindow_unlockAndPost(offscreenWindow);
    ANativeWindow_release(offscreenWindow);


    void *data;
    CALL_VK(vkMapMemory(oVulkanInfo.device_info.device_, oVulkanInfo.texture_object_info.mem, 0,
                        oVulkanInfo.texture_image_info.textureSize, 0, &data));

    unsigned char* imageData = reinterpret_cast<unsigned char *>(buf.bits);
//    for (int32_t y = 0; y < oVulkanInfo.texture_object_info.tex_height ; y++) {
//        unsigned char *row = reinterpret_cast<unsigned char *>((char *) data +
//                                                               oVulkanInfo.texture_image_info.rowPitch *
//                                                               y );
//        size_t srcSize = (size_t) (4 * buf.stride);
//        for (int32_t x = 0; x < oVulkanInfo.texture_object_info.tex_width ; x++) {
//            row[x * 4 + 0] = imageData[(x + y * buf.stride)* 4 + 0];
//            row[x * 4 + 1] = imageData[(x + y * buf.stride)* 4 + 1];
//            row[x * 4 + 2] = imageData[(x + y * buf.stride)* 4 + 2];
//            row[x * 4 + 3] = imageData[(x + y * buf.stride)* 4 + 3];
//        }
//    }
    for (int32_t y = 0; y < oVulkanInfo.texture_object_info.tex_height; y++) {
        unsigned char *row = reinterpret_cast<unsigned char *>((char *) data +
                                                               oVulkanInfo.texture_image_info.rowPitch *
                                                               y );
        unsigned char* src = reinterpret_cast<unsigned char *>((char*)buf.bits + 4 * buf.stride * y);
        size_t srcSize = (size_t) (4 * buf.stride);
        memcpy(row , src, srcSize);
    }
}

VkResult VulkanMain::drawFrame(VulkanInfo &oVulkanInfo) {
    uint32_t nextIndex = 0;
    VkResult result = vkAcquireNextImageKHR(oVulkanInfo.device_info.device_,
                                            oVulkanInfo.swapchain_info.swapchain_,
                                            std::numeric_limits<uint64_t>::max(),
                                            oVulkanInfo.render_info.imageAvailableSemaphore_,
                                            VK_NULL_HANDLE, &nextIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        reInitSwapChain();
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain");
    }
    VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSemaphore waitSemaphores[] = {oVulkanInfo.render_info.imageAvailableSemaphore_};
    VkSemaphore signalSemaphores[] = {oVulkanInfo.render_info.renderFinSemaphore_};

    VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = &waitStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &oVulkanInfo.render_info.cmdBuffer_[nextIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
    };
    CALL_VK(vkQueueSubmit(oVulkanInfo.queue_info.graphics_queue, 1, &submit_info,
                          VK_NULL_HANDLE));

    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .swapchainCount = 1,
            .pSwapchains = &oVulkanInfo.swapchain_info.swapchain_,
            .pImageIndices = &nextIndex,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .pResults = &result
    };
    result = vkQueuePresentKHR(oVulkanInfo.queue_info.present_queue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        reInitSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    result = vkQueueWaitIdle(oVulkanInfo.queue_info.present_queue);
    return result;
}

void VulkanMain::initShader(VulkanInfo &oVulkanInfo, const char *iFilePath, ShaderType iType) {
    AAsset *file = AAssetManager_open(assetManager, iFilePath, AASSET_MODE_BUFFER);
    assert(file);
    size_t fileLength = (size_t) AAsset_getLength(file);
    unique_ptr<char[]> fileContent(new char[fileLength]);
    AAsset_read(file, fileContent.get(), fileLength);

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .codeSize = fileLength,
            .pCode = (const uint32_t *) fileContent.get(),
            .flags = 0,
    };

    oVulkanInfo.shaderStages[iType] = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .pSpecializationInfo = nullptr,
            .flags = 0,
            .stage = iType == VERTEX_SHADER ? VK_SHADER_STAGE_VERTEX_BIT
                                            : VK_SHADER_STAGE_FRAGMENT_BIT,
            .pName = "main",
    };
    CALL_VK(vkCreateShaderModule(oVulkanInfo.device_info.device_, &shaderModuleCreateInfo,
                                 nullptr, &oVulkanInfo.shaderStages[iType].module));
    AAsset_close(file);
}

void VulkanMain::cleanUp() {
    LOGI("clean up");
    deleteVulkanSurface();
    delete_vulkan->deleteInstance(mVulkanInfo);
}

void VulkanMain::deleteVulkanSurface() {
    LOGI("delete surface");
    delete_vulkan->deleteSwapChain(mVulkanInfo);
    delete_vulkan->deleteTexture(mVulkanInfo);
    delete_vulkan->deleteVertexBuffer(mVulkanInfo);
    delete_vulkan->deleteSemaphore(mVulkanInfo);
    delete_vulkan->deleteSurface(mVulkanInfo);
    ANativeWindow_release(window);
    LOGI("delete finish");
}

void VulkanMain::reInitSwapChain() {
    LOGI("start");
    bool isIncludeDepth = false;
    vkDeviceWaitIdle(mVulkanInfo.device_info.device_);
    delete_vulkan->deleteSwapChain(mVulkanInfo);
    init_vulkan->initSwapChainExtension(mVulkanInfo);
    init_vulkan->initSwapChain(mVulkanInfo);
    init_vulkan->initRenderPass(mVulkanInfo, isIncludeDepth);
    initShader(mVulkanInfo, vertexShader.c_str(), VERTEX_SHADER);
    initShader(mVulkanInfo, fragmentShader.c_str(), FRAGMENT_SHADER);
    init_vulkan->vInitPipelineLayouts(mVulkanInfo);
    init_vulkan->initPipeLine(mVulkanInfo, (VkBool32) isIncludeDepth);
    init_vulkan->initFrameBuffers(mVulkanInfo, isIncludeDepth);
    init_vulkan->initCommandBuffer(mVulkanInfo);
    LOGI("end");
}
