# BLE room beacons

The board advertises **iBeacon** packets alongside the voice assistant. A phone
can use the signal to estimate which room it's in; no pairing is needed.

The beacon config is a separate remote package, `base/ble-beacon.yaml`, listed
in `waveshare-va.yaml`. Both packages are pulled from GitHub (`ble`,
`refresh: 0s`), so the thin YAML and `secrets.yaml` are still the only files to
keep. Remove the beacon entry from `files:` to build without it.

## Identifiers

Use the same UUID and major across the home, with a **unique minor and device
name per board**. Override these under `substitutions:` in the thin config:

| Substitution | Default | What it does |
|---|---|---|
| `beacon_uuid` | example UUID in the package | Identifies the beacon group; replace it for a permanent installation. |
| `beacon_major` | `1` | Group within the UUID. |
| `beacon_minor` | `1` | Room identifier; use a different value for each board. |
| `beacon_tx_power` | `0` | Transmit power in dBm. |
| `beacon_measured_power` | `-59` | Calibrated RSSI at one metre, not transmit power. |

## Phone setup

Check the UUID, major and minor with an iBeacon scanner first. Room tracking
needs a phone app that maps those identifiers to rooms, averages RSSI and waits
for a sustained change before switching rooms. One beacon gives proximity,
not a room map. Background scanning and permissions depend on the phone and app.

For Home Assistant, the phone app must publish its room through a supported
integration. The firmware doesn't create a phone room entity on its own.

## Calibration

Start at 0 dBm. Try -3, -6, -9 or -12 if rooms overlap too much. Measure RSSI at
one metre and set `beacon_measured_power` to match; repeat after moving a board
or changing its transmit power. Walls don't stop the signal, so test the room
boundaries in place.

Check advertising during music and voice requests too. Bluetooth adds RAM
pressure alongside audio; watch for resets and dropouts.
