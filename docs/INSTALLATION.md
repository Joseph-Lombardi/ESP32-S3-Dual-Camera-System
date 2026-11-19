# Installation and operation

[Back to project](../README.md)

## Choose the right artifact

| Goal | Artifact | Availability |
|---|---|---|
| Capture a USB-serial photo pair | `firmware/dual_camera_usb_capture/dual_camera_usb_capture.ino` + matching Dual SPI library | Capture sketch and replacement library source supplied by author; hardware rerun pending |
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

## Author-supplied USB capture sketch

Open `firmware/dual_camera_usb_capture/dual_camera_usb_capture.ino` in Arduino IDE. It requires a modified `Arducam_Mega` library that routes CS10 to FSPI (SCK12/MISO13/MOSI11) and CS21 to HSPI (SCK16/MISO17/MOSI18). The author has supplied the matching replacement `src` in `src(2).zip`; its 14 files match the repository source byte for byte. The sketch itself sets CS pins; SPI signal pin initialization belongs to the library.

Compile using the actual ESP32-S3 board/core and record its settings before uploading. This repository does not claim a successful firmware build or hardware rerun. After uploading, close all serial clients and start the Python receiver. Reset the board while Python waits for `READY` if necessary, then press Enter in Python to send `F`.

The sketch captures CAM1 and CAM2 sequentially as VGA JPEGs, with a 500 ms delay between completed CAM1 transmission and the next capture. It sends at most 128 camera-buffer bytes per loop iteration and accepts a total length up to 2,000,000 bytes. It does not control the LED rings or MPU6050. The separate LED example remains independent.

## Install Arducam Mega and replace its src folder

1. Install **Arducam Mega** through Arduino IDE’s Library Manager, or locate your existing installation. Record the installed version; the source comparison baseline is not proof of the installed release.
2. Close Arduino IDE and back up the installed `Arducam_Mega/src` folder outside the active library directory.
3. Replace that entire `src` folder with the author-supplied `src` from `src(2).zip`. The same 14 files are available at [`firmware/experimental_dual_bus/Arducam_Mega/src`](../firmware/experimental_dual_bus/Arducam_Mega/src). Keep the library’s existing `library.properties`, license and other files. **Replace only `src`; do not create `src/src`.** Do not install the archived packaging as a second library.
4. The resulting paths must include `Arducam_Mega/src/Arducam_Mega.h` and `Arducam_Mega/src/Arducam/ArducamSpi.cpp`. For a default Windows sketchbook, the library is usually under `Documents/Arduino/libraries/Arducam_Mega/`; use the sketchbook location configured in Arduino IDE if it differs, including a OneDrive location.
5. Reopen Arduino IDE and open `firmware/dual_camera_usb_capture/dual_camera_usb_capture.ino`. Select the actual ESP32-S3 board and record the core version and board/USB settings. Ensure the build resolves a single `Arducam_Mega.h` from the modified library.
6. Compile, retain the full build output and upload after checking the [independent-bus wiring](WIRING.md). No compilation or hardware rerun is claimed here. Close serial clients before running `python/recibir_foto.py`.

The driver initializes its own FSPI and HSPI objects internally. A separate global `SPI.begin(...)` call does not configure both objects. The optional `firmware/experimental_dual_bus/dual_bus_init/dual_bus_init.ino` is an earlier initialization-only diagnostic; use the USB capture sketch to transmit photographs.

## LED reference example

Install **Adafruit NeoPixel** through Library Manager and record its version. Open `firmware/led_ring_demo/led_ring_demo.ino`. It defines two eight-pixel rings on GPIO4/5 and cycles colors at reduced configured brightness. Its logic is not synchronized to a camera. Confirm a suitable power arrangement before running it; current was not measured in the archived lab evidence.

## Host-only checks

```bash
python -m unittest discover -s tests -v
```

Tests use synthetic marker-delimited payloads and a fake serial stream. They verify framing behavior and file protection, not valid JPEG decoding, camera hardware, electrical reliability or firmware compilation.
