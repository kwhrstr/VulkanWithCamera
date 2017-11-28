//
// Created by 川原　将太郎 on 2017/10/03.
//

#ifndef VULKANPRA_VULKANMAIN_HPP
#define VULKANPRA_VULKANMAIN_HPP

#include <android/asset_manager.h>
#include "VulkanMain.h"
#include "VulkanUtil.h"


class VulkanMain
{
#define STATE_RUNNING 1
#define STATE_PAUSED 2
#define STATE_EXIT 3

public:
    VulkanMain(AAssetManager *assetManager,
               const char *vertexShader,
               const char *fragmentShader) :
            assetManager(assetManager),
            vertexShader(string(vertexShader)),
            fragmentShader(string(fragmentShader)),
            state(STATE_RUNNING) {
    }
    void surfaceChanged();
    void resume(ANativeWindow *pWindow);
    void pause();
    inline void stop() { state = STATE_EXIT; }
    void init(ANativeWindow *pWindow);
    void initTexture();
    void cleanUp();
    bool run();
    bool initNativeCamera();
    ~VulkanMain();
    void initInstance();
    void setOffScreenWindow(ANativeWindow *pWindow);

    void deleteVulkanSurface();
    void reInitSwapChain();

private:
    ANativeWindow *window;
    ANativeWindow *offscreenWindow;
    AAssetManager *assetManager;
    string vertexShader;
    string fragmentShader;
    int state;
    void initVulkanInfo(VulkanInfo &oVulkanInfo ,ANativeWindow *pWindow);
    void initWindow(ANativeWindow* pWindow);

    bool mainLoop();
    void updateUniformBuffer(VulkanInfo &oVulkanInfo);
    void initShader(VulkanInfo &oVulkanInfo, const char* iFilePath, ShaderType iType);
    void updateTexture(VulkanInfo &oVulkanInfo);

    VkResult drawFrame(VulkanInfo &oVulkanInfo);

};




#endif //VULKANPRA_VULKANMAIN_HPP
