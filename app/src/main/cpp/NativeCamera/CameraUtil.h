#ifndef FASTANDFURIOUS_UTIL_H
#define FASTANDFURIOUS_UTIL_H

#include <unistd.h>
#include <stdlib.h>
#include <android/log.h>
#include "../util.h"
// used to get logcat outputs which can be regex filtered by the LOG_TAG we give
// So in Logcat you can filter this example by putting "FastAndFurious"
#define ASSERT(cond, fmt, ...)                                \
 if (!(cond)) {                                              \
   __android_log_assert(#cond, LOG_TAG, fmt, ##__VA_ARGS__); \
 }

// A Data Structure to communicate resolution between camera and ImageReader
struct ImageFormat {
  int32_t width;
  int32_t height;
  int32_t format;  // ex) YUV_420
};

/**
 * A helper class to assist image size comparison, by comparing the absolute
 * size
 * regardless of the portrait or landscape mode.
 */
class DisplayDimension {
 public:
  DisplayDimension(int32_t w, int32_t h) : w_(w), h_(h), portrait_(false) {
    if (h > w) {
      // make it landscape
      w_ = h;
      h_ = w;
      portrait_ = true;
    }
  }
  DisplayDimension(const DisplayDimension& other) {
    w_ = other.w_;
    h_ = other.h_;
    portrait_ = other.portrait_;
  }

  DisplayDimension(void) {
    w_ = 0;
    h_ = 0;
    portrait_ = false;
  }
  DisplayDimension& operator=(const DisplayDimension& other) {
    w_ = other.w_;
    h_ = other.h_;
    portrait_ = other.portrait_;

    return (*this);
  }

  bool IsSameRatio(DisplayDimension& other) {
    return (w_ * other.h_ == h_ * other.w_);
  }
  bool operator>(DisplayDimension& other) {
    return (w_ >= other.w_ & h_ >= other.h_);
  }
  bool operator==(DisplayDimension& other) {
    return (w_ == other.w_ && h_ == other.h_ && portrait_ == other.portrait_);
  }
  DisplayDimension operator-(DisplayDimension& other) {
    DisplayDimension delta(w_ - other.w_, h_ - other.h_);
    return delta;
  }
  void Flip(void) { portrait_ = !portrait_; }
  bool IsPortrait(void) { return portrait_; }
  int32_t width(void) { return w_; }
  int32_t height(void) { return h_; }
  int32_t org_width(void) { return (portrait_ ? h_ : w_); }
  int32_t org_height(void) { return (portrait_ ? w_ : h_); }

 private:
  int32_t w_, h_;
  bool portrait_;

};
#endif  // FASTANDFURIOUS_UTIL_H
