# Lounge: upstream AEC with 48 kHz playback

Experimental port of maintainer `aec` core commit `89f3111` and its pinned
`MichalZaniewicz/esphome-aec` backend commit
`3513103b486dc4290593d8fc3c22932d2706e4ff`. The vendored component keeps
upstream's SpeexDSP AEC, mic handoff, barge-in and codec volume routing.
Upstream license is retained in components/UPSTREAM-LICENSE and source headers.
This branch promotes the tested configuration to base/core.yaml and the backend
to components/aec_speexdsp. BLE support is not included.

## Rate split

- Physical four-slot TDM, ADC, DAC, speaker, music and announcements: 48 kHz.
- Capture: 127-tap Q15 Hamming low-pass with 7 kHz cutoff, then decimation by 3
  of all four slots on the same phase. Persistent history crosses DMA blocks.
- Mic and analog reference have the same 63-input-sample (1.3125 ms) delay.
- SpeexDSP, cleaned microphone, wake word and Assist: unchanged 16 kHz.
- Playback buffers and clock-drift correction use the physical rate. The wake
  beep keeps its 100 ms duration and 1 kHz pitch at either physical rate.

The component's physical_sample_rate defaults to 16000; 48000 is opt-in and
currently requires analog_slot reference. ADC, DAC and speaker sample rates
must match. The experimental core sets all relevant rates to 48000.
Raw-slot diagnostics in this experiment observe downsampled capture, so a
post-filter reference peak alone cannot rule out clipping at the physical ADC.
The internal 16 kHz diagnostic capture replay command is unavailable at 48 kHz;
normal music, voice replies and barge-in continue through the standard speaker.

## Build and rollback

Use ESPHome 2026.10.2:

```sh
esphome compile waveshare-va.yaml
esphome upload waveshare-va.yaml --device <device-address>
```

The thin config fetches this fork's `codex/aec-speexdsp-48khz` branch, including
its vendored backend. CI overrides `audio_components_source` to use local
`../components`. For local development, override that substitution with the
absolute path to this checkout's components directory.

To return to the maintainer's 16 kHz baseline, use their pinned AEC commit
`89f31118437dac1bfa622bbea3d29bbecb0a30a6` in the thin config's package source
and rebuild. This fork's original `aec` branch remains separate and uses the
previous ESP-SR backend.

## Validation

Host C++ tests verify output counts, unity DC/passband gain, rejection of 10/16
kHz input (amplitude below 0.004), identical mic/reference alignment, inverted
channel behavior and output independent of arbitrary input chunk boundaries.
The wake-beep test verifies stereo equality, pitch and duration at 16/48 kHz.
Both 16/48 kHz YAML configurations validate; mismatched speaker rates and a
48 kHz software reference are rejected. Firmware compiles for ESP32-S3.
Hardware checks and user listening results are recorded below when available.

## Installed hardware result — 2026-11-16

Compiled and OTA-installed with ESPHome 2026.10.2, build timestamp
2026-11-16 10:54:26 +0100. API dump confirms speaker at 48000 Hz and cleaned
microphone at 16000 Hz. During a 55-second initial observation, DSP load
averaged 48.7–49.6% of real time; observed processing peaks were
56.5–74.9%. Free heap was 61624–71848 bytes.
The DSP task retained at least 3116 bytes of stack. No API disconnection or
reboot was observed during the window. This is a short startup/idle check,
not a sustained music stress test. User listening checks remain pending.

The initial unoptimized filter averaged about 71–73%, with one peak above its
frame budget. The installed filter uses symmetric coefficients and bounded
int32 partial sums, bit-identical to full int64 convolution in the host test.

Run host checks with `python tests/aec_48khz/run_tests.py`.
The local experiment’s `upstream-backend.patch` records the exact backend changes against the pinned
upstream source. Publication status is tracked in Git.

Two empty-playback-ring underruns were logged during that observation. Their
presence alone does not establish audible cutouts; correlate them with actual
continuous playback before concluding stability. The 16 kHz baseline also
logged one during its initial observation.

## User listening result — 2026-11-16

The user reports: "music sounds better, aec works tested up to 60% volume,
no other issues so far." This supports improved perceived music quality and
working echo cancellation on the lounge at tested volumes up to 60% of the
volume control. It is a subjective listening result, not a calibrated output
level or quantitative echo-reduction measurement. The user reported no issues
so far; sustained playback and volumes above 60% remain unverified.
The lounge remains on the combined 48 kHz playback / 16 kHz AEC build.
