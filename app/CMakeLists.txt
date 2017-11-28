
# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.3)

set(SRC_DIR src/main/cpp)
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_library( # Sets the name of the library.
             vulkan-android

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${COMMON_DIR}/vulkan_wrapper/vulkan_wrapper.cpp
             ${SRC_DIR}/VulkanJni.cpp
             ${SRC_DIR}/VulkanUtil.cpp
             ${SRC_DIR}/VulkanInfo/DeleteVulkanInfo.cpp
             ${SRC_DIR}/VulkanInfo/AbstractInitVulkan.cpp
             ${SRC_DIR}/VulkanInfo/InitWithUniform.cpp
             ${SRC_DIR}/VulkanInfo/InitWithTexture.cpp
             ${SRC_DIR}/NativeCamera/NativeCamera.cpp
             ${SRC_DIR}/NativeCamera/ImageReader.cpp
             ${SRC_DIR}/VulkanMain.cpp
             )


set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -std=c++14  -fexceptions -Wall \
            -Wextra -Wno-unused-parameter \
            -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_ANDROID_KHR \
            -DGLM_FORCE_RADIANS")

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
   set(CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} \
       -mhard-float -D_NDK_MATH_NO_SOFTFP=1 -mfloat-abi=hard")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
       -Wl,--no-warn-mismatch")
endif()

find_library( # Sets the name of the path variable.
              log-lib

              log )

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_include_directories(vulkan-android PRIVATE
   ${COMMON_DIR}/vulkan_wrapper
   ${COMMON_DIR}/data
   ${COMMON_DIR}/src
   )

target_link_libraries(vulkan-android
                      ${log-lib}
                      android
                      log
                      mediandk
                      camera2ndk)

