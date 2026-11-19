# Hardware and wiring

[Back to project](../README.md)

> **Current version (2026-11-19):** the [integrated acquisition guide](CURRENT_ACQUISITION.md) supersedes the earlier USB-only/LED-only descriptions below. Current firmware includes two-camera capture, both LED rings and MPU6050 sampling; Python saves JPEGs plus inclination CSV. The package reports capture 0010; this update did not rerun hardware.

## Bench bill of materials

| Item | Quantity | Evidence / role |
|---|---:|---|
| ESP32-S3 Dev Module | 1 | Controller selected in Arduino IDE |
| Arducam Mega 5 MP modules | 2 | Per-camera initialization and image previews |
| WS2812B eight-pixel rings | 2 | Both illuminated in bench photograph |
| Protoboard, jumper wires, USB cable and PC | As required | Temporary bench connections and host reception |
| MPU6050 | Earlier subsystem test | Not part of a documented synchronized imaging run |

Camera module nominal resolution is not the resolution of this test: previews show 640 × 480. Component revisions, USB bridge type and power budget were not archived.

## Independent SPI buses

**Each camera has its own SCK, MISO, MOSI and CS GPIOs.** Both cameras connect to one ESP32-S3.

| Signal | CAM1 (FSPI) | CAM2 (HSPI) |
|---|---:|---:|
| SCK | GPIO12 | GPIO16 |
| MISO | GPIO13 | GPIO17 |
| MOSI | GPIO11 | GPIO18 |
| CS | GPIO10 | GPIO21 |

The author confirmed the independent-bus architecture in the corrected package. These GPIOs match the preserved `ArducamSpi.cpp` and corrected project context. The capture sketch and replacement library source have since been supplied by the author. Both document the same separate SPI mappings. Board/core settings and a hardware rerun remain to be recorded.

Ring 1 data uses GPIO4 and ring 2 uses GPIO5, with eight LEDs per ring. The reference sketch tests the rings separately from camera capture.

The archived driver holds both CS lines high, selects the corresponding FSPI/HSPI bus and lowers the requested camera's CS. It implements one active camera selection at a time; independent buses do not establish concurrent transfers or simultaneous exposure.

Cameras were reported powered from 5 V and shared ground. Ring supply wiring, level adaptation, current capacity, decoupling and cable lengths are not adequately recorded to publish a complete electrical schematic. Check the actual board and module documentation before wiring; the reported supply voltage does not imply 5 V tolerance of ESP32 GPIOs. Do not derive a power design from the photographs.

Earlier MPU6050 tests used SDA GPIO8 / SCL GPIO7 and reported I²C address 0x68. Do not confuse that sensor with the ultrasonic distance sensor in earlier telemetry tests. No accelerometer-driven capture decision is demonstrated in this release.

## Physical assembly

![General assembly on the bench](../assets/fotos/01_montaje_general.jpg)

*Provisional dry bench arrangement, without a sealed enclosure.*

![Protoboard and controller wiring](../assets/fotos/02_montaje_protoboard.jpg)

*Alternative view for physical context; wire colors are not a certified pin map.*

There is no documented immersion, pressure or marine endurance test for this electronic assembly. The university project's mechanical simulations must not be treated as physical leak testing.
