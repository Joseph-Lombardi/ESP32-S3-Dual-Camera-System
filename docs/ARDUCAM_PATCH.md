# Arducam library modification: exact archived experiment

[Back to project](../README.md)

## Provenance and validation boundary

The supplied package contains a locally modified Arducam source snapshot and an initialization-only sketch. Its `LEEME_PI_LOM.txt` explicitly says it was **not physically validated or compiled in Arduino IDE**. It uses two independent SPI buses; it must not be presented as the final successful independent-SPI firmware.

For an auditable comparison, the snapshot was compared with official upstream **ArduCAM/Arducam_Mega** at commit [`146e058b1b5e57bd0d2c64885d0642390e11b6b1`](https://github.com/ArduCAM/Arducam_Mega/tree/146e058b1b5e57bd0d2c64885d0642390e11b6b1), retrieved on 2026-11-19. Upstream `library.properties` identifies version 3.0.0. This is a comparison baseline; it does not prove which release the original local installation came from.

Of the 14 supplied source files, **12 match that baseline byte for byte** and two differ. The [unified diff](../firmware/experimental_dual_bus/experimental-vs-upstream.patch) records those two differences. Paths in the diff are relative to the upstream repository and can be inspected or applied there with `git apply` after checking the baseline.

| File | Exact local behavior | Interpretation |
|---|---|---|
| `src/Arducam/ArducamCamera.c` | Removes `arducamSpiBegin()` from `cameraInit`; changes CS low to CS high | Defers SPI startup out of object construction; leaves camera unselected |
| `src/Arducam/ArducamSpi.cpp` | Adds FSPI/HSPI objects, GPIO maps, one-time initialization and active bus pointer | Routes byte/block transfers according to selected camera |
| Other supplied source files | Identical to the inspected upstream baseline | Retained third-party implementation; not claimed as local modification |

## Selection and bus lifecycle

CAM1 uses FSPI with SCK12/MISO13/MOSI11/CS10; CAM2 uses HSPI with SCK16/MISO17/MOSI18/CS21. `arducamSpiCsLow` initializes buses if necessary, raises both CS signals, assigns `activeSpi` for the requested CS, then lowers that line. `arducamSpiCsHigh` clears the pointer when deselecting the active camera. Reads/transfers use that pointer. An unset pointer returns zero for a byte transfer or performs no block transfer.

This is one active selection at a time. The code does not establish thread safety, transaction isolation, timing guarantees or simultaneous exposure. Frequency/mode handling and compatibility with the selected Arduino core require validation. Returning zero without an active bus may conceal a selection error during diagnosis; a future instrumented version could expose that condition explicitly.

## Packaging changes made for publication

- The 14 supplied `src` files are preserved byte for byte.
- The sketch is renamed from `prueba_dos_camaras.ino` to `dual_bus_init/dual_bus_init.ino` to match Arduino's sketch-directory naming convention; its contents are unchanged.
- The library source is under `Arducam_Mega/src/`. An upstream MIT `LICENSE` is included, since the staging ZIP omitted that license file despite referring to it in source headers.
- `library.properties` was added from the inspected upstream baseline with a distinct experimental display name and description. This is packaging metadata, not a claim that the archive has been built or tested.
- `LEEME_PI_LOM.txt` is retained as the original Spanish archive note.

The independent FSPI/HSPI architecture agrees with the corrected project notes. No final capture implementation was synthesized to fill the missing-source gap. The final flashed sketch, final patch and board settings still need to be supplied before an exact reproduction is possible.
