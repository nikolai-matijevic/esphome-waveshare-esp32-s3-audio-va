# Acoustic echo cancellation

The ADC captures both the microphone and an analog playback reference. ESP-SR
subtracts the playback echo before the stream reaches the wake word and Assist,
so music is less likely to drown out a request.

## Setup

Use `waveshare-va.yaml` as usual: copy it next to `secrets.yaml`, edit the
substitutions, then run `esphome run waveshare-va.yaml` or Install from the
dashboard. The core is pulled from GitHub (`aec`, `refresh: 0s`); there's no
extra package to keep locally. Requires **ESPHome 2026.10.2+**.

## Audio layout

- **Backend:** `n-IA-hane/esphome-audio-stack`, pinned to `v2026.11.0`.
- **One full-duplex TDM bus**, 48 kHz, four 16-bit slots. Slot 0 carries the mic;
  slot 1 carries the analog playback reference.
- **AEC:** ESP-SR `sr_low_cost`, 16 kHz, filter length 4.
- **Voice stream:** processed mono 16 kHz directly from the backend to both
  micro_wake_word and Assist. No microphone resampler is needed.
- **Gain:** mic and reference are fixed at 24 dB. The ES7210 gain slider is
  omitted because it doesn't control this backend.

## Comparing AEC on and off

**AEC test enabled** starts on. Turn it off in Home Assistant to bypass the
processor, then compare the same request at the same music volume. Check wake
words, microphone mute, playback and whether the device hears its own reply.

Bypass keeps the new audio backend running. To restore the original two-bus
layout, flash the stock firmware. AEC doesn't add TTS barge-in on its own.

## Status

Compiled and tested on-device with ESPHome 2026.10.2. Recognition during music
improved in listening tests; echo reduction hasn't been measured, and longer
playback/voice stability testing is still open. A successful build alone doesn't
show how well echo cancellation works in a particular room.
