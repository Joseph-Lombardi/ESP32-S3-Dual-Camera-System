# Dual-camera USB capture

[Back to project](../../README.md)

This sketch was supplied directly by the project author on 2026-11-19 after the initial portfolio publication. The source is preserved without functional changes. It is the supplied capture implementation; it has not been compiled, flashed or independently hardware tested for this publication.

- CAM1: FSPI, SCK12 / MISO13 / MOSI11 / CS10.
- CAM2: HSPI, SCK16 / MISO17 / MOSI18 / CS21.
- Serial: 115200 baud; `F` requests one sequential VGA JPEG pair.
- Framing: `READY`, `JPEG <id> <bytes>`, raw bytes, `END <id>`, `DONE`.
- No LED or accelerometer control is included in this sketch.

It requires the modified Dual SPI Arducam library. The archived library implements the separate bus mapping and exposes the called API, but its identity with the exact library deployed in the recorded successful run remains unconfirmed. Follow [installation](../../docs/INSTALLATION.md), [wiring](../../docs/WIRING.md), [protocol](../../docs/SERIAL_PROTOCOL.md) and [validation status](../../docs/VALIDATION_STATUS.md).
