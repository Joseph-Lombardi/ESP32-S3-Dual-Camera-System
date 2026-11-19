# Dual-camera USB capture

[Back to project](../../README.md)

This sketch was supplied directly by the project author on 2026-11-19 after the initial portfolio publication. The source is preserved without functional changes. It is the supplied capture implementation; it has not been compiled, flashed or independently hardware tested for this publication.

- CAM1: FSPI, SCK12 / MISO13 / MOSI11 / CS10.
- CAM2: HSPI, SCK16 / MISO17 / MOSI18 / CS21.
- Serial: 115200 baud; `F` requests one sequential VGA JPEG pair.
- Framing: `READY`, `JPEG <id> <bytes>`, raw bytes, `END <id>`, `DONE`.
- No LED or accelerometer control is included in this sketch.

It requires the modified Dual SPI Arducam library. The author has identified the required library modification by supplying `src(2).zip`: replace the installed `Arducam_Mega/src` folder with [this source](../experimental_dual_bus/Arducam_Mega/src). All 14 files match the repository copy byte for byte. The capture sketch and library have not been compiled or rerun on hardware here. Follow [installation](../../docs/INSTALLATION.md), [wiring](../../docs/WIRING.md), [protocol](../../docs/SERIAL_PROTOCOL.md) and [validation status](../../docs/VALIDATION_STATUS.md).
