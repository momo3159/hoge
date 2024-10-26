#pragma once
#include <stdint.h>

enum PixelFormat {
  kPixelRGBResv8BitPerColor,
  kPixelBGRResv8BitPerColor,
};

struct FrameBufferConfig {
  uint8_t* frame_buffer;
  uint32_t pixels_per_scan_line;  // 1ラインごとのピクセル数(非表示領域含む？)
  uint32_t horizontal_resolution; // ピクセル数
  uint32_t vertical_resolution;   // ピクセル数
  enum PixelFormat pixel_format;
};
