# Reproduction and next tests

[Back to project](../README.md)

## Recover before claiming a reproducible hardware release

Obtain the exact successful independent-SPI sketch, the complete installed Arducam library snapshot, board/core settings and original camera JPEGs. Record hashes and versions. Reconcile actual camera-2 wiring with the source. Retain the original implementation before introducing improvements.

## Proposed bench campaign

| Test | Procedure and record | Proposed completion evidence |
|---|---|---|
| Initialization | Cold-start each camera alone, then together; capture full logs and power configuration | Per-camera status for every attempt; explicit attempt count |
| Pair reception | Run a predeclared number of requests with matching firmware | Raw stream, original JPEGs, timestamps and failed attempts retained |
| Framing | Inject incomplete payload, wrong ID, invalid length and wrong END/DONE | Receiver rejects without claiming a complete pair |
| Integrity | Compare byte counts and hashes before/after transfer; decode images | Identical source/recovered JPEG where available; explanation for discarded bytes |
| Illumination | Same scene/distance with light off/on, recorded exposure | Image-quality comparison and electrical measurements |
| Orientation | Fixed mount and reference target | Camera identity and orientation independently checked |
| Inclination | Known reference positions and timestamped MPU6050 samples | Documented calibration and association with each capture |
| LoRa | Add buoy and receiving node; record fragments/retries/timing | Two images of the same pair reconstructed at the receiver |
| Energy | Measure supply voltage/current by operating mode | Energy per cycle and autonomy estimate from actual measurements |
| Enclosure | Separate seal/pressure test under an agreed protocol before electronics immersion | Logged duration, conditions and inspection outcome |

These are proposed tests, not achieved results. Pass thresholds and repetition count must be chosen before running them. No throughput, power, range or reliability numbers can be inferred from the present screenshots.

## Existing host tests

`tests/test_receiver.py` uses deterministic, synthetic serial streams to cover fragmented reads, full announced-payload consumption, ordered cameras, wrong framing, oversized payloads, missing markers, non-overwriting saves, numbering and partial/unconfirmed pairs. Marker-only fixtures are not claimed to be valid decodable photographs.
