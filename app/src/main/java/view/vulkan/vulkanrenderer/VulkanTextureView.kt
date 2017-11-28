package view.vulkan.vulkanrenderer

import android.app.Activity
import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.camera2.CameraManager
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.TextureView

class VulkanTextureView : TextureView {
    companion object {
        private val FLOAT_SIZE_BYTES = 4
        private var mIsNeedInit = false
    }
    private val tag = this::class.java.simpleName
    internal var mIsSleep = false
    private var mFirstSurfaceChange = false
    private lateinit var mCameraManager: CameraManager
    private lateinit var mActivity: Activity
    private var mSurface: Surface? = null
    private var mHasSurface = false
    private var mDetached = true
    private lateinit var mThread: VKThread
    var mRenderer: VulkanRenderer? = null
    var mCleanup = false

    constructor(context: Context) : super(context)
    constructor(context: Context, attrs: AttributeSet) : super(context, attrs)
    constructor(context: Context, attrs: AttributeSet, defStyle: Int) : super(context, attrs, defStyle)

    init {
        val surfaceTextureListener = object :SurfaceTextureListener{
            override fun onSurfaceTextureSizeChanged(surface: SurfaceTexture?, width: Int, height: Int) {
            }

            override fun onSurfaceTextureUpdated(surface: SurfaceTexture?) {
            }

            override fun onSurfaceTextureDestroyed(surface: SurfaceTexture?): Boolean {
                mHasSurface = false
                mIsNeedInit = false
                mThread.join()
                return true
            }

            override fun onSurfaceTextureAvailable(surface: SurfaceTexture?, width: Int, height: Int) {
                Log.i(tag, "created")
                mSurface = Surface(surface)
                mHasSurface = true
                mCleanup = true
                mThread = VKThread()
                mThread.start()
            }
        }
        setSurfaceTextureListener(surfaceTextureListener)
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        mDetached = false
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        mDetached = true
        try {
            mThread.join()
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    fun onResume(){
        mIsSleep = false
    }

    fun onPause(){
        mIsSleep = true
    }

    fun onDestroy() {
        mRenderer?.delete()
    }

    private inner class VKThread() : Thread("VKThread") {
        override fun run() {
            while (!mDetached) {
                if(mIsSleep){
                    sleep(10)
                    continue
                }
                if(!mHasSurface){
                    break
                }
                if (mIsNeedInit) {
                    mIsNeedInit = init(mSurface)
                    continue
                }
                if (!mRenderer!!.drawFrame()) {
                    reInit()
                }
            }
            cleanUp()
        }

        private fun init(surface: Surface?): Boolean {
            if (surface == null)
                return true
            if (mRenderer == null){
                Log.i(tag,"render is null")
                mRenderer = VulkanRenderer(context)
            }
            mRenderer!!.setSurface(mSurface!!)
            mRenderer!!.initVulkan()
            return false
        }

        private fun reInit(){
            mRenderer!!.reInit()
        }

        private fun cleanUp() {
            mRenderer?.delete()
            mRenderer = null
        }
    }
}