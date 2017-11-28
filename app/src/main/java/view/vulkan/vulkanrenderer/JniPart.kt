package view.vulkan.vulkanrenderer

import android.content.res.AssetManager
import android.view.Surface

class JniPart(assetManager: AssetManager, vertexPath:String, fragmentPath:String) {
    private var mNativeHandle:Long = 0
    init {
        System.loadLibrary("vulkan-android")
        mNativeHandle = jniCreate(assetManager, vertexPath, fragmentPath)
    }

    fun run():Boolean = jniRun(mNativeHandle)

    fun pause(){
        jniPause(mNativeHandle)
    }

    fun initVulkan(surface: Surface){
        jniInitVulkan(mNativeHandle, surface)
    }

    fun initVulkanInstance(){
        jniInitVulkanInstance(mNativeHandle)
    }

    fun initCamera():Boolean = jniInitCamera(mNativeHandle)

    fun setOffScreenSurface(surface: Surface){
        jniSetOffScreenSurface(mNativeHandle, surface)
    }

    fun initTexture(){
        jniInitTexture(mNativeHandle)
    }

    fun resume(){
        jniResume(mNativeHandle)
    }

    fun surfaceChanged(){
        jniSurfaceChanged(mNativeHandle)
    }

    fun stop(){
        jniStop(mNativeHandle)
    }

    fun reInit(){
        jniReInit(mNativeHandle)
    }

    fun delete(){
        jniDelete(mNativeHandle)
    }

    fun deleteSurface(){
        jniDeleteSurface(mNativeHandle)
    }

    private external fun jniCreate(assetManager: AssetManager, vertexPath: String, fragmentPath: String):Long
    private external fun jniInitVulkanInstance(nativeHandle:Long)
    private external fun jniInitVulkan(nativeHandle:Long, surface:Surface)
    private external fun jniInitCamera(nativeHandle:Long):Boolean
    private external fun jniInitTexture(nativeHandle:Long)
    private external fun jniRun(nativeHandle:Long):Boolean
    private external fun jniPause(nativeHandle:Long)
    private external fun jniResume(nativeHandle:Long)
    private external fun jniSetOffScreenSurface(nativeHandle:Long, surface: Surface)
    private external fun jniStop(nativeHandle:Long)
    private external fun jniReInit(nativeHandle:Long)
    private external fun jniDelete(nativeHandle:Long)
    private external fun jniDeleteSurface(nativeHandle:Long)
    private external fun jniSurfaceChanged(nativeHandle:Long)
}