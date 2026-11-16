#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
namespace esphome::aec_speexdsp {
// 127-tap Hamming-window low-pass at 7 kHz, Q15 unity DC gain.
// All four TDM slots share phase, coefficients and 63-sample delay.
// Persistent history makes output independent of DMA block boundaries.
class CaptureDecimator {
 public:
  size_t process(int16_t *pcm, size_t frames, size_t slots) {
    size_t output = 0;
    for (size_t n = 0; n < frames; ++n) {
      for (size_t s = 0; s < slots; ++s)
        history_[s][position_] = history_[s][position_ + TAPS] = pcm[n * slots + s];
      if (++phase_ == 3) {
        phase_ = 0;
        for (size_t s = 0; s < slots; ++s) {
          const int16_t *latest = history_[s] + position_ + TAPS;
          // Symmetry halves the multiplies. The first 63 coefficients have
          // L1 norm 28916, so their paired sum fits int32 even at full scale.
          int32_t sides = 0;
          for (size_t k = 0; k < TAPS / 2; ++k) {
            const int32_t pair = static_cast<int32_t>(latest[-static_cast<int>(k)]) +
                                 latest[-static_cast<int>(TAPS - 1 - k)];
            sides += pair * COEFFICIENTS[k];
          }
          const int64_t sum = static_cast<int64_t>(sides) +
              static_cast<int32_t>(latest[-static_cast<int>(TAPS / 2)]) * COEFFICIENTS[TAPS / 2];
          const int32_t value = (sum + 16384) >> 15;
          pcm[output * slots + s] = static_cast<int16_t>(std::clamp<int32_t>(value, -32768, 32767));
        }
        ++output;
      }
      if (++position_ == TAPS) position_ = 0;
    }
    return output;
  }
 private:
  static constexpr size_t TAPS = 127;
  static constexpr int16_t COEFFICIENTS[TAPS] = {12, 4, -9, -15, -10, 4, 17, 17, 3, -17, -27, -15, 13, 35, 32, 0, -39, -52, -22, 32, 70, 54, -11, -79, -92, -28, 71, 127, 83, -38, -148, -149, -24, 141, 213, 115, -95, -258, -227, 0, 262, 343, 146, -206, -440, -339, 68, 486, 564, 172, -443, -800, -538, 255, 1022, 1088, 189, -1204, -2039, -1292, 1324, 5026, 8270, 9564, 8270, 5026, 1324, -1292, -2039, -1204, 189, 1088, 1022, 255, -538, -800, -443, 172, 564, 486, 68, -339, -440, -206, 146, 343, 262, 0, -227, -258, -95, 115, 213, 141, -24, -149, -148, -38, 83, 127, 71, -28, -92, -79, -11, 54, 70, 32, -22, -52, -39, 0, 32, 35, 13, -15, -27, -17, 3, 17, 17, 4, -10, -15, -9, 4, 12};
  int16_t history_[4][TAPS * 2]{};
  size_t position_{0};
  unsigned phase_{0};
};
}
