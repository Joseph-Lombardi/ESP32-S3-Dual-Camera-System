# Current integrated acquisition

[Back to project](../README.md)

## Matching source files

- [Arduino sketch](../firmware/LanternGuard_DualCam_LED_MPU6050/LanternGuard_DualCam_LED_MPU6050.ino)
- [Python receiver](../python/recibir_foto.py)

Both came from `LanternGuard_Codigos_Actuales(1).zip`. Arduino is preserved byte for byte. Python has one functional correction: check `IMU ERROR` before the general `IMU ` prefix, so unavailable inclination is recorded as blank angles rather than rejected as malformed numbers. No other acquisition logic was changed.

## Wiring and dependencies

| Device | ESP32-S3 signals |
|---|---|
| CAM1, FSPI | SCK12, MISO13, MOSI11, CS10 |
| CAM2, HSPI | SCK16, MISO17, MOSI18, CS21 |
| LED ring 1 / ring 2 | GPIO4 / GPIO5, eight pixels each |
| MPU6050 | SDA8, SCL7, address 0x68; project notes specify 3.3 V and common ground |

Install Arducam Mega and replace its `src` as described in [installation](INSTALLATION.md). The modified Dual SPI source already in this repository remains required. Install Adafruit NeoPixel; `Wire` comes with the board core. Python requires pySerial from `requirements.txt`. Record the actual board/core, library versions and USB settings used.

Open the Arduino sketch in its matching folder, compile and upload to the ESP32-S3. Close other serial clients, set `PUERTO` in `python/recibir_foto.py` (default `COM13`) and run `python python/recibir_foto.py`. Wait for `READY`; reset the board if necessary. Enter requests a pair and S exits.

## Capture sequence

On `F`, firmware samples the accelerometer once, turns both rings white at configured brightness 128/255 and waits 500 ms. CAM1 captures VGA JPEG and transmits it, then firmware waits another 500 ms before CAM2 capture. Both rings are switched off after CAM2’s `takePicture` returns, before its image transmission. Failure paths also request lights off. This is sequential imaging, not simultaneous exposure. Brightness is a software setting, not a measured optical output.

The previously sampled roll/pitch is sent after both images as `IMU <roll> <pitch>` or `IMU ERROR`, followed by `DONE`. It is one inclination sample associated with the pair, taken before either exposure; it is not a separate measurement at each camera’s exposure. Angles use accelerometer-only formulas without calibration or sensor fusion.

## Outputs and known limits

Images are written to `imagenes/camara1_NNNN.jpg` and `camara2_NNNN.jpg`. After `DONE`, Python appends `imagenes/mediciones_mpu6050.csv` with `numero_captura`, `fecha_hora`, `roll_grados`, `pitch_grados`, `camara1`, `camara2`. The timestamp is the host’s local time at CSV writing, without a timezone offset. Missing inclination produces blank angles; images can still complete. Older USB-only firmware that sends `DONE` without IMU is also accepted with blank angles.

An initialization failure of the MPU6050 does not prevent camera `READY`; firmware reports `ERROR MPU6050_NO_DETECTADO` separately. Reset and restart after a protocol failure. Image saves are not atomic as a pair, and no CRC or retransmission is implemented. Discarded bytes outside JPEG markers remain unexplained.

The package reports capture 0010 with both images saved, roll 66.52°, pitch 1.31°, CSV written and completion. This update did not independently repeat that run. Sixteen synthetic host tests passed, including numerical IMU data, missing IMU, CSV append behavior and malformed/incomplete records. No firmware compilation or hardware test was performed here. LoRa transmission of this integrated system, underwater deployment and AI classification remain future work.
