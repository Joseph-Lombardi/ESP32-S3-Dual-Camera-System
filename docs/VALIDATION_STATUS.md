# Validation status and source recovery

[Back to project](../README.md)

## Evidence versus supplied source

| Item | Evidence | Source status | What is not established |
|---|---|---|---|
| Two-camera initialization | Both status values `0`; `AMBAS CAMARAS LISTAS` | Capture sketch supplied later by author; exact flashed build unconfirmed | Stable cold starts or root cause of earlier failures |
| Dual-camera local reception | One pair and byte counts in Python terminal; two previews | Receiver now supplied directly by author in chat | Exact original file bytes or hardware rerun of normalized receiver |
| Independent SPI buses | Explicit author correction in the replacement ZIP; separate GPIO maps in archived driver | Capture sketch and replacement library source supplied by author | Exact final pin map, initialization and deployed patch |
| FSPI/HSPI patch | Actual source snapshot | Preserved, diffed against official upstream | Compile success or successful physical use |
| Dual-ring illumination | Bench photograph | ZIP contains reference-only sketch | Exact illumination firmware, capture synchronization or optical performance |
| MPU6050 | Earlier I²C detection/readings reported | No synchronized run supplied | Per-image angle/accuracy |
| Dual cameras over LoRa | No integrated evidence | Other LoRaVision code is a different configuration | Dual-camera end-to-end wireless operation |
| Marine deployment / energy / app / AI | Future architecture | Not demonstrated by these records | Autonomy, underwater use or classification performance |

## Architecture correction — 2026-11-19

The replacement `ESP32S3_DualCam_Portfolio_CORREGIDO(1).zip` explicitly corrects the earlier package: **the cameras use independent SPI GPIOs**. CAM1 is documented as SCK12/MISO13/MOSI11/CS10 and CAM2 as SCK16/MISO17/MOSI18/CS21 in the preserved FSPI/HSPI driver. Earlier portfolio statements describing a shared signal bus were incorrect and are superseded by this correction. The README, diagram, wiring and supporting guides have been corrected consistently.

All firmware bytes and the eight retained evidence images were compared with the replacement ZIP and match. The additional TRL3 page screenshot is background context about a separate telemetry experiment, rather than dual-camera test evidence. The receiver previously supplied separately by the author is retained; its absence from this staging ZIP does not invalidate that source recovery. The capture sketch was subsequently supplied by the author, as recorded below. The author subsequently supplied and identified the matching replacement `src`, as recorded below.

## Capture source supplied — 2026-11-19

The author pasted the complete Arduino capture sketch after the architecture correction. It is preserved at `firmware/dual_camera_usb_capture/dual_camera_usb_capture.ino`, with no functional edits. It explicitly uses the independent FSPI/HSPI pin maps, VGA JPEG, 128-byte camera reads, a 2,000,000-byte length limit and `READY` / `F` / `JPEG` / `END` / `DONE` records. These records agree with the supplied Python receiver on static inspection.

The available archived library exposes the methods called by the sketch and implements the documented separate SPI mappings. The author subsequently identified the replacement source through `src(2).zip`. Source identification is separate from a successful compilation or hardware rerun here. The capture sketch contains no LED-ring or MPU6050 control.

## Arducam Mega source identified — 2026-11-19

The author supplied `src(2).zip` and specified replacing the installed Arducam Mega library’s `src` folder with it. All 14 supplied files match `firmware/experimental_dual_bus/Arducam_Mega/src` byte for byte. The capture sketch and required modified source are therefore now available and identified by the author. The earlier archive note is retained as historical context, rather than the current source-availability status.

Use the existing installed library and replace only `src`, retaining its metadata. No source logic was changed during this identification. Board/core and installed library versions remain to be recorded, and no firmware compilation or hardware rerun was performed here.

## Recovery performed on 2026-11-19

- Read the staging ZIP's work prompt, context, validation notes and modified source.
- Reviewed university TRL4 on `main`, at inspected commit `d82dc33b5488d713b2adbacd183367a1f41dd70f`. Its README explicitly records missing final firmware/library and originally missing Python source.
- Inspected the university repository's current tracked paths and available fetched branch/history path names for Arducam, dual-camera and receiver artifacts. No final camera sketch or library source was found there.
- Inspected the related personal [LoRaVision repository](https://github.com/Joseph-Lombardi/LoRaVision). Its single-camera Heltec capture and LoRa receiver use different pin mappings and protocol; they are not substituted for this dual-camera firmware.
- Recovered the receiver from the author's new chat paste. Normalized Markdown escapes, whitespace, `__name__` formatting and a visibly corrupted filename regex to `r"camara[12]_(\d+)\.jpg"`. Preserved protocol, constants, Spanish console output and error-handling logic. The result is not a byte-for-byte copy of an original uploaded `.py` file.
- Compared the 14 archived library source files to official upstream commit `146e058b1b5e57bd0d2c64885d0642390e11b6b1`: 12 identical, two modified. At that earlier recovery step the final patch had not yet been identified; the author’s later `src(2).zip` identification supersedes that source-availability status.

The university documentation is preserved as a group record. This personal repository updates source availability locally without rewriting that historical README.

## Publication checks

The host-only test results and runtime are appended after execution below. These checks do not validate firmware, GPIO behavior, timing, electrical design or a physical camera capture.

## Missing for a turnkey release

1. Confirmation that the supplied capture sketch matches the flashed version used in the recorded successful run.
2. Installed Arducam Mega version and metadata outside the supplied `src` folder; the modified source itself is now identified.
3. Board/core versions, USB settings and a confirmed schematic.
4. Original `camara1_0001.jpg` / `camara2_0001.jpg`, raw serial dumps and per-attempt logs.
5. Exact illumination-test sketch and a controlled illumination/capture run.

A more recent file outside the inspected sources may exist; the recovery above is scoped to accessible project material and does not assert exhaustive knowledge of every local machine.

### Executed host checks

- Runtime: Python 3.12.14 on Linux, pySerial 3.5.
- `python -m unittest discover -s tests -v`: **13 tests passed**, using synthetic data only.
- `git apply --check` against the pinned upstream comparison commit: passed.
- 63 local Markdown link/image references resolve; all eight evidence images decode and match the supplied ZIP byte for byte.
- All 14 bundled library source files match the supplied experimental archive byte for byte; the initialization sketch is unchanged apart from its path/name.
- No Arduino compilation, flashing, hardware rerun or field trial was performed.
