# Debugging log and engineering decisions

[Back to project](../README.md)

> **Current version (2026-11-19):** the [integrated acquisition guide](CURRENT_ACQUISITION.md) supersedes the earlier USB-only/LED-only descriptions below. Current firmware includes two-camera capture, both LED rings and MPU6050 sampling; Python saves JPEGs plus inclination CSV. The package reports capture 0010; this update did not rerun hardware.

The order below reconstructs the supplied evidence; there are no per-attempt timestamps or controlled changes recorded.

| Observation | Investigation / response | Verified outcome | Remaining uncertainty |
|---|---|---|---|
| Reset or hang reported around camera `begin()` | Add before/after messages; review wiring, power and SPI/CS selection | Screenshot ends at `[CAM1] Inicializando` | The image alone cannot establish duration or root cause |
| Two cameras need independent selection | Inspect driver initialization and chip-select state; inspect the preserved independent FSPI/HSPI implementation | Code shows deferred bus startup, deselection and active-bus routing | Author has now identified this replacement source; no controlled isolation of which change resolved the fault |
| Later initialization completes | Check per-camera return values | Both return `0`; console says `AMBAS CAMARAS LISTAS` | Capture sketch and replacement library source supplied; board/core settings and hardware rerun pending |
| Announced payload includes bytes outside JPEG | Drain entire frame before parsing END; extract SOI-to-EOI slice | One pair saved and opened | Origin of the 1597/1507 discarded bytes unknown |
| Camera images dark / oriented differently | Treat transport and optics as separate tests | Both previews viewable | No controlled exposure, mounting or LED comparison |
| Port cannot be opened | Release serial client before changing camera code | Operational instruction supplied by author | No port-contention reproduction performed here |

## Why the library investigation matters

The upstream C++ constructor creates a camera object through `createArducamCamera`, which calls `cameraInit`. In the inspected upstream implementation, that routine initializes SPI and pulls the camera CS low. The supplied experimental variant changes it to configure CS as high and leaves bus initialization to a later transaction. This separates object construction from first SPI use and keeps an unselected device inactive.

The two-bus driver routes transfers through an `activeSpi` pointer, selected from CS10/21. It initializes FSPI and HSPI once and explicitly deselects both devices before selecting one. This is a useful driver-level experiment to inspect; the archive does not show it being compiled, and the author has since identified this same source as the required Arducam Mega replacement. This identifies the implementation without establishing a unique cause of the original fault.

See the [exact comparison and provenance](ARDUCAM_PATCH.md). No unique causal diagnosis is claimed: wiring and firmware changed during troubleshooting without a complete change log.
