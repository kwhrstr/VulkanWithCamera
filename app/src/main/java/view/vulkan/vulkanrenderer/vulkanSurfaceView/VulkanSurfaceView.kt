package view.vulkan.vulkanrenderer.vulkanSurfaceView

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import view.vulkan.vulkanrenderer.VulkanRenderer


class VulkanSurfaceView : SurfaceView {
    companion object {
        private val FLOAT_SIZE_BYTES = 4
        private var mIsInit = false
        val RENDER_MODE_WHEN_DIRTY = 0
        val RENDER_MODE_CONTINUOUSLY = 1
    }

    private val tag = this::class.java.simpleName
    private var mSurface: Surface? = null
    private var mDetached = true
    private var mVkThread: VKThread? = null
    var mVkRenderer: VulkanRenderer? = null
    private var mSizeChanged = true

    constructor(context: Context) : super(context)
    constructor(context: Context, attrs: AttributeSet) : super(context, attrs)
    constructor(context: Context, attrs: AttributeSet, defStyle: Int) : super(context, attrs, defStyle)

    init {
        val callBack = object : SurfaceHolder.Callback {
            override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
//                mVkThread?.onWindowResize(width, height)
            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
                mVkThread?.surfaceDestroyed()
            }

            override fun surfaceCreated(holder: SurfaceHolder) {
                mVkThread?.surfaceCreated(holder.surface)
            }
        }
        holder.addCallback(callBack)
    }


    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        if (mDetached) {
            mVkThread = VKThread()
            mVkThread!!.start()
        }
        mDetached = false
    }

    override fun onDetachedFromWindow() {
        if (mVkThread != null)
            mVkThread!!.requestExitAndWait()
        mDetached = true
        super.onDetachedFromWindow()
    }

    private inner class VKThread() : Thread("VKThread" + id) {
        private var mDone = false
        private var mHasSurface: Boolean = false
        private var mWidth: Int = 0
        private var mHeight: Int = 0
        private var mIsNeedInitSurface = true

        override fun run() {
            vulkanRun()
        }

        private fun vulkanRun() {
            try {
                while (!mDone) {
                    while (needToWait()) {
                        synchronized(this){
                            if (!mHasSurface) {
                                if (!mIsNeedInitSurface) {
                                    mVkRenderer!!.deleteSurface()
                                    mIsNeedInitSurface = true
                                }
                            }
                        }
                        sleep(10)
                    }
                    if (mIsNeedInitSurface) {
                        mIsNeedInitSurface = needInitSurface(mSurface)
                        continue
                    }
                    if (mDone) {
                        break
                    }
                    if (!mVkRenderer!!.drawFrame()) {
                        reInit()
                    }
                }
            } finally {
                mVkRenderer?.delete()
            }
        }

        private fun needInitSurface(surface: Surface?): Boolean {
            if (surface == null)
                return true
            if (mVkRenderer == null) {
                Log.i(tag, "render is null")
                mVkRenderer = VulkanRenderer(context)
            }
            mVkRenderer?.setSurface(surface)
            mVkRenderer?.initVulkan()
            return false
        }


        private fun needToWait(): Boolean {
            if (mDone) {
                return false
            }

            if (!mHasSurface) {
                return true
            }
            return false
        }

        fun requestExitAndWait() {
            mDone = true
            try {
                join()
            } catch (ex: InterruptedException) {
                Thread.currentThread().interrupt()
            }

        }

        fun surfaceCreated(surface: Surface) {
            synchronized(this){
                mSurface = surface
                mHasSurface = true
            }
        }

        fun surfaceDestroyed() {
            synchronized(this) {
                mSurface = null
                mHasSurface = false
            }
        }

        fun onWindowResize(w: Int, h: Int) {
            synchronized(this) {
                mWidth = w
                mHeight = h
                mSizeChanged = true
            }
        }

        private fun reInit() {
            mVkRenderer!!.reInit()
        }

    }

}