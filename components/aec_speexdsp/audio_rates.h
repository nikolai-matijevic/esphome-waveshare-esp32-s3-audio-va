#pragma once
#include "esphome/core/defines.h"
#ifndef AEC_SPEEXDSP_PHYSICAL_SAMPLE_RATE
#define AEC_SPEEXDSP_PHYSICAL_SAMPLE_RATE 16000
#endif
namespace esphome::aec_speexdsp {
static constexpr unsigned PHYSICAL_SAMPLE_RATE = AEC_SPEEXDSP_PHYSICAL_SAMPLE_RATE;
static constexpr unsigned CAPTURE_RATIO = PHYSICAL_SAMPLE_RATE / 16000;
static_assert(PHYSICAL_SAMPLE_RATE == 16000 || PHYSICAL_SAMPLE_RATE == 48000);
}
