# Installation and operation

[Back to project](../README.md)

## Choose the right artifact

| Goal | Artifact | Availability |
|---|---|---|
| Reproduce the successful independent-SPI photo pair | Exact final capture sketch + exact final library | Not recovered; no substitute supplied |
| Inspect/run the archived two-bus initialization experiment | `firmware/experimental_dual_bus/` | Source available, unverified; does not implement the photo protocol |
| Exercise a dual-ring example | `firmware/led_ring_demo/led_ring_demo.ino` | Reference example, not the exact lab sketch |
| Receive a pair from compatible capture firmware | `python/recibir_foto.py` | Supplied by author; hardware rerun still needed |

The observed IDE was Arduino IDE 2.3.11, ESP32S3 Dev Module, COM13 and 115200 baud. The exact Arduino-ESP32 core version, board memory/USB settings and final Arducam library version used in the successful capture were not recorded. Do not infer them from unrelated Heltec examples.

## Python receiver

Use Python 3 with f-string support. Host checks for this publication record the actual runtime in [validation](VALIDATION_STATUS.md); that runtime is not evidence of the original lab's Python version.

```bash
python -m venv .venv
```

Activate on Windows PowerShell:

```powershell
.venv\Scripts\Activate.ps1
```

Or on Linux/macOS:

```bash
source .venv/bin/activate
```

```bash
python -m pip install -r requirements.txt
python -m serial.tools.list_ports
```

Edit `PUERTO` in `python/recibir_foto.py` to match the board's serial device. Its default is the observed `COM13`; Unix device paths will differ. Keep baud rate consistent with the firmware.

**Before running Python, close Arduino IDE completely, including Serial Monitor and Serial Plotter. Close other terminal programs using the same port.** This avoids port contention. The actual resource conflict occurs when two applications open the serial device. Before uploading firmware again, exit Python to release that device.

```bash
python python/recibir_foto.py
```

Wait for `READY`. If opening the connection did not reset the board or the startup message was missed, reset the board while the receiver is waiting. Press Enter to request a pair; press S to leave. The receiver creates `imagenes/` in the current working directory and prints a path for each saved image.

If reception fails, the script stops the capture loop rather than attempting to resynchronize an unknown binary stream. Exit, reset the ESP32-S3 and restart the script. Confirm the selected port and exclusive access. An “Access denied” error is not evidence that the camera failed. If endpoint protection flags a script, review the exact alert; do not disable protection as a general installation step.

## Experimental Arduino library setup

This procedure is provided for source inspection and future validation, not as proof of a working independent-SPI build.

1. Read [the patch analysis](ARDUCAM_PATCH.md) and select the **experimental two-bus wiring** in [WIRING.md](WIRING.md).
2. Install the ESP32 board support through Arduino IDE's Boards Manager and select the actual ESP32-S3 board. Record the chosen core version and settings in the test log; there is no validated pinned core configuration in this archive.
3. Back up your existing Arducam library. Copy the bundled `firmware/experimental_dual_bus/Arducam_Mega` folder into the Arduino sketchbook's `libraries` directory. The package includes a `library.properties` file added for packaging. Ensure Arduino resolves exactly one `Arducam_Mega.h`; remove ambiguous duplicate installations from the active library path.
4. Open `firmware/experimental_dual_bus/dual_bus_init/dual_bus_init.ino`. The sketch was renamed to match its enclosing directory, as expected by Arduino IDE. Its source logic was not changed.
5. Compile first and retain the full build output. No compile success is asserted by this repository. Only upload after checking wiring and power.
6. The sketch prints initialization diagnostics and loops idle. It does **not** emit `READY`, process `F`, capture JPEGs or drive the Python receiver.

The archived library owns SPI initialization internally. A separate global `SPI.begin(...)` call does not configure both internal FSPI/HSPI objects; inspect the actual driver initialization.

## LED reference example

Install **Adafruit NeoPixel** through Library Manager and record its version. Open `firmware/led_ring_demo/led_ring_demo.ino`. It defines two eight-pixel rings on GPIO4/5 and cycles colors at reduced configured brightness. Its logic is not synchronized to a camera. Confirm a suitable power arrangement before running it; current was not measured in the archived lab evidence.

## Host-only checks

```bash
python -m unittest discover -s tests -v
```

Tests use synthetic marker-delimited payloads and a fake serial stream. They verify framing behavior and file protection, not valid JPEG decoding, camera hardware, electrical reliability or firmware compilation.
