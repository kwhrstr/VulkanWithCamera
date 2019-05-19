package view.vulkan.vulkanrenderer

import android.content.Context
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import android.view.Surface
import android.view.TextureView

class VulkanTextureView : TextureView {
    companion object {
        private val FLOAT_SIZE_BYTES = 4
        private var mIsNeedInit = false
    }
    private val tag = this::class.java.simpleName
    internal var mIsSleep = false
    private var mSurface: Surface? = null
    private var mDetached = true
    private lateinit var mThread: VKThread
    var mRenderer: VulkanRenderer? = null

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
                mIsNeedInit = false
                mThread.join()
                return true
            }

            override fun onSurfaceTextureAvailable(surface: SurfaceTexture?, width: Int, height: Int) {
                mIsNeedInit = true
                mSurface = Surface(surface)
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
                if (mIsNeedInit) {
                    mSurface?.let {
                        init(it)
                        mIsNeedInit = false
                    }
                    continue
                }

                mRenderer?.let {
                    if(!it.drawFrame()){
                        reInit()
                    }
                }
            }
            cleanUp()
        }

        private fun init(surface: Surface) {
            if (mRenderer == null){
                mRenderer = VulkanRenderer(context)
            }
            mRenderer?.setSurface(surface)
            mRenderer?.initVulkan()
        }

        private fun reInit(){
            mRenderer?.reInit()
        }

        private fun cleanUp() {
            mRenderer?.delete()
            mRenderer = null
        }
    }
}