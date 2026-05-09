# WS2812B Driver Using ESP32 RMT Peripheral

## Overview

This project implements a WS2812B (NeoPixel) LED driver using the ESP32 RMT (Remote Control) peripheral without using external libraries like Adafruit NeoPixel or FastLED.

The ESP32 RMT hardware is used to generate accurate signal timings required by the WS2812B communication protocol.

---

# WS2812B Protocol

WS2812B LEDs use a one-wire serial communication protocol with pulse-width encoded data.

Each LED requires:

- 24 bits total
- 8 bits Green
- 8 bits Red
- 8 bits Blue

The LED determines whether a bit is `0` or `1` based on HIGH and LOW pulse durations.

| Bit | HIGH Time | LOW Time |
|------|-----------|----------|
| 0 | ~400 ns | ~850 ns |
| 1 | ~800 ns | ~450 ns |

After transmission, the line must stay LOW for at least 50 µs to latch the data.

---

# Why ESP32 + RMT?

Precise timing is difficult using software delays because interrupts and CPU scheduling can affect signal timing.

The ESP32 RMT peripheral solves this by generating hardware-timed waveforms automatically.

## Advantages

- Accurate nanosecond-level timing
- No dependency on software delay loops
- Reliable communication
- Lower CPU usage
- Immune to interrupt timing issues

---

# How It Works

The driver:

1. Converts RGB values into GRB format
2. Encodes each bit into RMT timing waveforms
3. Stores 24 waveform items
4. Sends data using `rmt_write_items()`
5. Applies a reset pulse to latch the data

WS2812B expects colors in:

```text
Green → Red → Blue
