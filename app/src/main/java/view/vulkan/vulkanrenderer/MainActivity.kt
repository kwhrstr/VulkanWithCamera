package view.vulkan.vulkanrenderer

import android.Manifest
import android.os.Bundle
import android.support.v4.app.ActivityCompat
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view.Window
import android.view.WindowManager
import com.simplemobiletools.commons.extensions.hasCameraPermission
import com.simplemobiletools.commons.extensions.hasWriteStoragePermission
import com.simplemobiletools.commons.extensions.toast
import view.vulkan.vulkanrenderer.vulkanSurfaceView.VulkanSurfaceView
import java.util.*

class MainActivity : AppCompatActivity() {
    companion object {
        private val CAMERA_STORAGE_PERMISSION = 1
    }
    private val tag = this::class.java.simpleName
    private var mIsAskingPermissions = false
    private lateinit var mVulkanTextureView: VulkanSurfaceView

    override fun onCreate(savedInstanceState: Bundle?) {
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
        super.onCreate(savedInstanceState)
        supportActionBar?.hide()
        setContentView(R.layout.activity_main)
        mVulkanTextureView = findViewById(R.id.VulkanView)
        tryInitCamera()
    }

    private fun hasCameraAndStoragePermission() = hasCameraPermission() && hasWriteStoragePermission()

    private fun tryInitCamera() {
        if (hasCameraAndStoragePermission()) {
            initPreview()
        } else {
            val permissions = ArrayList<String>(2)
            if (!hasCameraPermission()) {
                permissions.add(Manifest.permission.CAMERA)
            }
            if (!hasWriteStoragePermission()) {
                permissions.add(Manifest.permission.WRITE_EXTERNAL_STORAGE)
            }
            ActivityCompat.requestPermissions(this, permissions.toTypedArray(), CAMERA_STORAGE_PERMISSION)
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        mIsAskingPermissions = false
        if (requestCode == CAMERA_STORAGE_PERMISSION) {
            if (hasCameraAndStoragePermission()) {
                initPreview()
            } else {
                toast(R.string.no)
                finish()
            }
        }
    }


    private fun initPreview() {
        mVulkanTextureView = findViewById(R.id.VulkanView)
    }


    override fun onResume() {
        super.onResume()
//        mVulkanTextureView.onResume()
    }

    override fun onPause() {
        super.onPause()
        Log.i(tag, "activity pause")
//        mVulkanTextureView.onPause()
    }

    override fun onDestroy() {
        super.onDestroy()
//        mVulkanTextureView.onDestroy()
    }

}
