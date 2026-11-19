# Validation status and source recovery

[Back to project](../README.md)

## Evidence versus supplied source

| Item | Evidence | Source status | What is not established |
|---|---|---|---|
| Two-camera initialization | Both status values `0`; `AMBAS CAMARAS LISTAS` | Exact flashed sketch missing | Stable cold starts or root cause of earlier failures |
| Dual-camera local reception | One pair and byte counts in Python terminal; two previews | Receiver now supplied directly by author in chat | Exact original file bytes or hardware rerun of normalized receiver |
| Shared-SPI arrangement | Author's wiring report, reflected in TRL4 | Final shared-SPI firmware/library not found | Exact deployed bus initialization and driver patch |
| FSPI/HSPI patch | Actual source snapshot | Preserved, diffed against official upstream | Compile success or successful physical use |
| Dual-ring illumination | Bench photograph | ZIP contains reference-only sketch | Exact illumination firmware, capture synchronization or optical performance |
| MPU6050 | Earlier I²C detection/readings reported | No synchronized run supplied | Per-image angle/accuracy |
| Dual cameras over LoRa | No integrated evidence | Other LoRaVision code is a different configuration | Dual-camera end-to-end wireless operation |
| Marine deployment / energy / app / AI | Future architecture | Not demonstrated by these records | Autonomy, underwater use or classification performance |

## Recovery performed on 2026-11-19

- Read the staging ZIP's work prompt, context, validation notes and modified source.
- Reviewed university TRL4 on `main`, at inspected commit `d82dc33b5488d713b2adbacd183367a1f41dd70f`. Its README explicitly records missing final firmware/library and originally missing Python source.
- Inspected the university repository's current tracked paths and available fetched branch/history path names for Arducam, dual-camera and receiver artifacts. No final camera sketch or library source was found there.
- Inspected the related personal [LoRaVision repository](https://github.com/Joseph-Lombardi/LoRaVision). Its single-camera Heltec capture and LoRa receiver use different pin mappings and protocol; they are not substituted for this dual-camera firmware.
- Recovered the receiver from the author's new chat paste. Normalized Markdown escapes, whitespace, `__name__` formatting and a visibly corrupted filename regex to `r"camara[12]_(\d+)\.jpg"`. Preserved protocol, constants, Spanish console output and error-handling logic. The result is not a byte-for-byte copy of an original uploaded `.py` file.
- Compared the 14 archived library source files to official upstream commit `146e058b1b5e57bd0d2c64885d0642390e11b6b1`: 12 identical, two modified. The final shared-bus patch remains unrecovered.

The university documentation is preserved as a group record. This personal repository updates source availability locally without rewriting that historical README.

## Publication checks

The host-only test results and runtime are appended after execution below. These checks do not validate firmware, GPIO behavior, timing, electrical design or a physical camera capture.

## Missing for a turnkey release

1. Exact final shared-SPI Arduino capture sketch.
2. Complete exact library snapshot used for the successful run, including version and patch provenance.
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
