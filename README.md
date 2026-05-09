# WS2812B LED Driver Using ESP32 RMT Peripheral

## Overview

This project implements a custom WS2812B (NeoPixel) LED driver using the ESP32 RMT (Remote Control) peripheral without using external libraries such as Adafruit NeoPixel or FastLED.

The implementation directly controls the WS2812B communication protocol using ESP32 hardware-level timing generation through the RMT peripheral. This ensures accurate pulse timing and reliable LED control while reducing CPU overhead compared to software bit-banging.

The program demonstrates controlling a single WS2812B LED by cycling through multiple colors:

* Red
* Green
* Blue
* White
* Off

---

# Hardware Requirements

* ESP32 Development Board
* WS2812B / NeoPixel LED
* Jumper wires
* USB cable

---

# Connections

| WS2812B Pin | ESP32 Pin |
| ----------- | --------- |
| DIN         | GPIO 5    |
| VCC         | 5V        |
| GND         | GND       |

---

# Features

* Uses ESP32 RMT hardware peripheral
* Accurate nanosecond-level signal timing
* No external WS2812 libraries used
* Efficient hardware waveform generation
* Supports GRB color format
* Easy to expand for multiple LEDs

---

# WS2812B Communication Protocol

WS2812B LEDs use a single-wire communication protocol where data is transmitted using precisely timed high and low pulses.

Each LED receives:

* 24 bits total
* 8 bits Green
* 8 bits Red
* 8 bits Blue

The protocol uses pulse-width encoding:

| Bit Value | HIGH Time | LOW Time |
| --------- | --------- | -------- |
| 0         | 400 ns    | 850 ns   |
| 1         | 800 ns    | 450 ns   |

After sending data, the signal line must stay LOW for at least 50 microseconds to latch the data.

---

# How It Works

## 1. RMT Peripheral Initialization

The function `initWS2812()` configures the ESP32 RMT peripheral in transmission mode.

### Important Configuration Choices

### RMT TX Mode

```cpp
config.rmt_mode = RMT_MODE_TX;
```

This enables waveform transmission mode.

### Clock Divider

```cpp
config.clk_div = 2;
```

The ESP32 APB clock is 80 MHz.

Clock period:

```text
80 MHz / 2 = 40 MHz
```

So each RMT tick equals:

```text
1 / 40 MHz = 25 ns
```

This provides enough precision for WS2812 timing requirements.

### Idle State

```cpp
config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
```

The WS2812 data line must remain LOW when idle.

---

# Timing Conversion

The macro:

```cpp
#define NS_TO_TICKS(ns) ((ns) / 25)
```

converts nanoseconds into RMT clock ticks.

Example:

```cpp
400 ns / 25 ns = 16 ticks
```

---

# Data Encoding Process

The function `setPixelColor()` converts RGB values into WS2812 pulse timings.

The WS2812 expects data in:

```text
GRB format
```

So:

```cpp
uint8_t data[3] = { g, r, b };
```

---

# Bit Transmission

Each bit is converted into one `rmt_item32_t`.

For logic 1:

```cpp
HIGH for 800 ns
LOW  for 450 ns
```

For logic 0:

```cpp
HIGH for 400 ns
LOW  for 850 ns
```

The program loops through:

* 3 bytes
* 8 bits per byte

Total:

```text
24 bits transmitted
```

---

# Platform-Specific Decisions

## Why ESP32?

The ESP32 contains:

* Dedicated RMT hardware
* High-speed peripherals
* Precise timing generation

This makes it ideal for driving WS2812 LEDs.

---

# Error Handling

The code checks for:

* RMT configuration failure
* Driver installation failure

Example:

```cpp
if (err != ESP_OK)
{
    Serial.println("RMT config failed");
}
```

This improves debugging reliability.

---

# Research Path

## Initial Goal

The first goal was understanding how WS2812 LEDs communicate and what exact timing requirements are needed.

---

# Research Topics Explored

## 1. Understanding WS2812 Timing

### Search Terms Used

* WS2812B protocol timing
* WS2812B datasheet timing
* NeoPixel one wire protocol
* WS2812B communication protocol
* WS2812B GRB format

### What Was Learned

From the datasheet and protocol diagrams:

* Each bit uses pulse-width encoding
* Timing must be extremely accurate
* LEDs use GRB byte order instead of RGB
* Reset pulse requires LOW signal for 50+ microseconds

---

## 2. ESP32 Hardware Peripheral Research

### Search Terms Used

* ESP32 RMT WS2812
* ESP32 RMT waveform generation
* ESP32 RMT example
* ESP32 RMT timing control
* ESP32 RMT documentation

### What Was Learned

Research showed:

* ESP32 RMT can generate precise waveforms
* RMT stores pulse durations in memory
* Timing generation happens in hardware
* RMT is commonly used for infrared and LED protocols

This confirmed RMT was the best solution.

---

## 3. Timing Accuracy Research

### Search Terms Used

* ESP32 APB clock frequency
* ESP32 RMT clock divider
* nanoseconds to RMT ticks
* ESP32 RMT tick calculation

### What Was Learned

The ESP32 APB clock runs at:

```text
80 MHz
```

Using:

```cpp
clk_div = 2
```

Resulted in:

```text
25 ns per tick
```

This allowed accurate timing conversion.

---

## 4. Data Encoding Research

### Search Terms Used

* WS2812 data format
* WS2812 GRB order
* rmt_item32_t structure
* ESP32 RMT item encoding

This perfectly matches WS2812 pulse requirements.

---

# Output Demonstration

The LED cycles through:

1. Red
2. Green
3. Blue
4. White
5. Off

with 1-second delay between transitions.

---

# Conclusion

This project successfully demonstrates low-level WS2812B control using the ESP32 RMT peripheral without relying on external LED libraries.

The implementation provides:

* Accurate timing generation
* Hardware-assisted waveform control
* Reliable LED communication
* Scalable architecture for future expansion

The project also helped in understanding:

* Embedded timing protocols
* ESP32 hardware peripherals
* Pulse-width encoded communication
* Real-time waveform generation
