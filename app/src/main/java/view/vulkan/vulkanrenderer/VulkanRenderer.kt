package view.vulkan.vulkanrenderer

import android.content.Context
import android.graphics.SurfaceTexture
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.util.Log
import android.view.Surface

class VulkanRenderer(context: Context) {
    private val tag = this::class.java.simpleName

    private lateinit var mSurface:Surface
    private var mAssets = context.assets
    private val mContext = context
    private var mJniPart:JniPart
    private val lock = Any()
    private var mIsInitCamera = false
    private lateinit var mOffScreenSurfaceTexture:SurfaceTexture
    private lateinit var mOffScreenSurface:Surface

    init{
        mJniPart = JniPart(mAssets, VERTEX_SHADER, FRAGMENT_SHADER)
        val thread = InitNativeCameraThread()
        thread.start()
        mJniPart.initVulkanInstance()
    }

    fun setSurface(surface: Surface){
        mSurface = surface
    }

    fun initVulkan(){
        mJniPart.initVulkan(mSurface)
        mJniPart.initTexture()
    }

    fun drawFrame():Boolean {
        synchronized(lock){
            if(!mIsInitCamera)return false
            return mJniPart.run()
        }
    }

    fun reInit(){
        mJniPart.reInit()
    }

    fun delete(){
        mJniPart.delete()
    }

    fun deleteSurface(){
        mJniPart.deleteSurface()
        mOffScreenSurface.release()
    }

    fun pause(){
        Log.i(tag, "pause3")
        mJniPart.pause()
    }

    fun resume(){
        mJniPart.resume()
    }

    private inner class  InitNativeCameraThread() : Thread("InitThread") {
        override fun run() {
            synchronized(lock){
                mIsInitCamera = mJniPart.initCamera()
                createOffScreenSurface()
                mOffScreenSurface = Surface(mOffScreenSurfaceTexture)
                mJniPart.setOffScreenSurface(mOffScreenSurface)
            }
        }

        private fun createOffScreenSurface(){
            val textures = IntArray(1)
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0)
            GLES20.glGenTextures(1, textures, 0)
            checkGlError()

            val texture = textures[0]
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture)
            checkGlError()

            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)

            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)

            checkGlError()
            mOffScreenSurfaceTexture = SurfaceTexture(texture)
        }


        private fun checkGlError() {
            try{
                val error = GLES20.glGetError()
                if (error != GLES20.GL_NO_ERROR) {
                    Log.e(tag, "GL error = 0x" + Integer.toHexString(error))
                    throw RuntimeException("GL ERROR")
                }
            }catch (ex:RuntimeException){
                Log.e(tag, ex.message)
                throw ex
            }
        }





    }
}
