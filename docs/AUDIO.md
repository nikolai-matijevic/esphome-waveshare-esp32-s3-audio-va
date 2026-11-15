# 48 kHz audio

The ADC, DAC, microphone and speaker share a **48 kHz** physical sample rate.
Music no longer has to pass through a 16 kHz output path. A microphone resampler
keeps the wake word and Assist at **16 kHz**.

## Setup

Copy `waveshare-va.yaml` beside `secrets.yaml`, edit the substitutions and run
`esphome run waveshare-va.yaml` or Install from the dashboard. The core is pulled
from GitHub (`audio-48khz`, `refresh: 0s`). Requires **ESPHome 2026.10.2+** for
the microphone resampler.

The audio drivers are stock ESPHome components. The original two-bus layout,
16-bit format, microphone gain, volume limits and buffers are unchanged.
Mute and diagnostic stop actions still address the physical microphone.

`physical_sample_rate` defaults to `48000`. Set it to `16000` in the thin
config for a matched baseline: all four physical endpoints change together,
and voice processing stays at 16 kHz.

## Listening checks

Compare the same track at the same volume and ESPHome version. Check pitch,
clarity, dropouts, wake words, a full Assist request and announcement ducking.
Both rates previously compiled and ran on-device with ESPHome 2026.10.2;
listening tests found clearer music at 48 kHz with voice assistance working.
Intermittent cutouts were also reported; sustained playback still needs testing.
A configuration check alone doesn't establish audio quality or clock timing.
