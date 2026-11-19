# Experimental FSPI + HSPI initialization archive

**Status: historical initialization diagnostic plus author-identified Arducam Mega replacement source. No compilation or hardware rerun was performed for this publication.**

This snapshot is retained in the corrected `ESP32S3_DualCam_Portfolio_CORREGIDO(1).zip`, originating from the earlier `PI_Lom_Arducam_DualSPI.zip` diagnostic package. Its original [archive note](LEEME_PI_LOM.txt) records the diagnostic’s unverified status at packaging time. The author subsequently supplied `src(2).zip` and confirmed that this source replaces the installed Arducam Mega `src`; all 14 files match byte for byte. Follow the updated installation guide rather than installing a second copy of the library.

| Path | Purpose |
|---|---|
| [dual_bus_init/dual_bus_init.ino](dual_bus_init/dual_bus_init.ino) | Calls both `begin()` methods and prints status; no image capture or serial image protocol |
| [Arducam_Mega/src/Arducam/ArducamSpi.cpp](Arducam_Mega/src/Arducam/ArducamSpi.cpp) | Separate FSPI/HSPI driver routing |
| [Arducam_Mega/src/Arducam/ArducamCamera.c](Arducam_Mega/src/Arducam/ArducamCamera.c) | Modified initialization lifecycle |
| [experimental-vs-upstream.patch](experimental-vs-upstream.patch) | Exact source diff against pinned official upstream baseline |
| [Arducam_Mega/LICENSE](Arducam_Mega/LICENSE) | Original Arducam MIT license |

CAM1: SCK12 / MISO13 / MOSI11 / CS10. CAM2: SCK16 / MISO17 / MOSI18 / CS21. **Each camera uses its own SPI GPIOs, consistent with the author’s corrected architecture.** The source has now been identified by the author as the Arducam Mega modification required by the supplied capture sketch.

Read [installation](../../docs/INSTALLATION.md), [wiring](../../docs/WIRING.md) and [patch analysis](../../docs/ARDUCAM_PATCH.md) before using this source. The Python receiver cannot communicate with this initialization-only sketch because it does not emit `READY`, process `F` or transmit image frames.
