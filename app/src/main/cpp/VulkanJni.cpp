#include <jni.h>
#include "VulkanMain.h"
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>

extern "C" {
JNIEXPORT jlong JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniCreate(
        JNIEnv *env,
        jclass type, jobject assetManager_, jstring vertexPath_, jstring fragmentPath_) {

    AAssetManager *assetManager = AAssetManager_fromJava(env, assetManager_);
    if (assetManager == nullptr) {
        LOGE("get assetManager fail!");
    }

    const char *vertexPath = env->GetStringUTFChars(vertexPath_, 0);
    const char *fragmentPath = env->GetStringUTFChars(fragmentPath_, 0);

    VulkanMain *vulkanMain = new VulkanMain(assetManager, vertexPath, fragmentPath);
    env->ReleaseStringUTFChars(vertexPath_, vertexPath);
    env->ReleaseStringUTFChars(fragmentPath_, fragmentPath);

    return (jlong) vulkanMain;
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniInitVulkanInstance(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->initInstance();
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniInitVulkan(
        JNIEnv *env, jclass type, jlong jNativeHandle, jobject jSurface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, jSurface);
    if (window == nullptr) {
        LOGE("get window from surface fail!");
        return;
    }
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->init(window);
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniSetOffScreenSurface(
        JNIEnv *env, jclass type, jlong jNativeHandle, jobject jSurface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, jSurface);
    if (window == nullptr) {
        LOGE("get window from surface fail!");
        return;
    }
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->setOffScreenWindow(window);
}

JNIEXPORT jboolean JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniInitCamera(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    return (jboolean)vulkanMain->initNativeCamera();
}

JNIEXPORT jboolean JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniRun__J(
        JNIEnv *env, jclass type, jlong jNativeHandler) {

    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandler);
    return (jboolean) vulkanMain->run();
}


JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniPause__J(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    LOGI("JNI PAUSE");
    vulkanMain->pause();
}


JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniResume(
        JNIEnv *env, jclass type, jlong jNativeHandle, jobject jSurface) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    ANativeWindow *window = ANativeWindow_fromSurface(env, jSurface);
    if (window == nullptr) {
        LOGE("get window from surface fail!");
        return;
    }
    vulkanMain->resume(window);
}


JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniStop__J(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->stop();
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniReInit__J(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->reInitSwapChain();
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniInitTexture__J(
        JNIEnv *env, jclass type, jlong jNativeHandle) {

    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->initTexture();
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniDelete(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);

    vulkanMain->cleanUp();
    delete[] vulkanMain;
}

JNIEXPORT void JNICALL
Java_view_vulkan_vulkanrenderer_JniPart_jniDeleteSurface(
        JNIEnv *env, jclass type, jlong jNativeHandle) {
    VulkanMain *vulkanMain = reinterpret_cast<VulkanMain *>(jNativeHandle);
    vulkanMain->deleteVulkanSurface();
}


}