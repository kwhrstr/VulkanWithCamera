//
// Created by 川原　将太郎 on 2017/11/08.
//

#ifndef VULKANRENDERER_UTIL_H
#define VULKANRENDERER_UTIL_H
#include <android/log.h>
#include <cassert>
#include <vector>
#include <memory>
#include <string>

#define LOG_TAG __func__
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define ASSERT(cond, fmt, ...)                                \
 if (!(cond)) {                                              \
   __android_log_assert(#cond, LOG_TAG, fmt, ##__VA_ARGS__); \
 }


#endif //VULKANRENDERER_UTIL_H
