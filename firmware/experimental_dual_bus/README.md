# Experimental FSPI + HSPI initialization archive

**Status: archived experiment. Not compiled or hardware validated for this publication. Not the recovered final independent-SPI implementation.**

This snapshot is retained in the corrected `ESP32S3_DualCam_Portfolio_CORREGIDO(1).zip`, originating from the earlier `PI_Lom_Arducam_DualSPI.zip` diagnostic package. Its original [archive note](LEEME_PI_LOM.txt) explicitly marks it unverified.

| Path | Purpose |
|---|---|
| [dual_bus_init/dual_bus_init.ino](dual_bus_init/dual_bus_init.ino) | Calls both `begin()` methods and prints status; no image capture or serial image protocol |
| [Arducam_Mega/src/Arducam/ArducamSpi.cpp](Arducam_Mega/src/Arducam/ArducamSpi.cpp) | Separate FSPI/HSPI driver routing |
| [Arducam_Mega/src/Arducam/ArducamCamera.c](Arducam_Mega/src/Arducam/ArducamCamera.c) | Modified initialization lifecycle |
| [experimental-vs-upstream.patch](experimental-vs-upstream.patch) | Exact source diff against pinned official upstream baseline |
| [Arducam_Mega/LICENSE](Arducam_Mega/LICENSE) | Original Arducam MIT license |

CAM1: SCK12 / MISO13 / MOSI11 / CS10. CAM2: SCK16 / MISO17 / MOSI18 / CS21. **Each camera uses its own SPI GPIOs, consistent with the author’s corrected architecture.** The archived source is still not confirmed as the exact build used in the successful capture.

Read [installation](../../docs/INSTALLATION.md), [wiring](../../docs/WIRING.md) and [patch analysis](../../docs/ARDUCAM_PATCH.md) before using this source. The Python receiver cannot communicate with this initialization-only sketch because it does not emit `READY`, process `F` or transmit image frames.
