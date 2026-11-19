# Debugging log and engineering decisions

[Back to project](../README.md)

The order below reconstructs the supplied evidence; there are no per-attempt timestamps or controlled changes recorded.

| Observation | Investigation / response | Verified outcome | Remaining uncertainty |
|---|---|---|---|
| Reset or hang reported around camera `begin()` | Add before/after messages; review wiring, power and SPI/CS selection | Screenshot ends at `[CAM1] Inicializando` | The image alone cannot establish duration or root cause |
| Two cameras need independent selection | Inspect driver initialization and chip-select state; inspect the preserved independent FSPI/HSPI implementation | Code shows deferred bus startup, deselection and active-bus routing | This experimental variant is not proven to be the final fix |
| Later initialization completes | Check per-camera return values | Both return `0`; console says `AMBAS CAMARAS LISTAS` | Capture sketch now supplied; exact flashed build and library unconfirmed |
| Announced payload includes bytes outside JPEG | Drain entire frame before parsing END; extract SOI-to-EOI slice | One pair saved and opened | Origin of the 1597/1507 discarded bytes unknown |
| Camera images dark / oriented differently | Treat transport and optics as separate tests | Both previews viewable | No controlled exposure, mounting or LED comparison |
| Port cannot be opened | Release serial client before changing camera code | Operational instruction supplied by author | No port-contention reproduction performed here |

## Why the library investigation matters

The upstream C++ constructor creates a camera object through `createArducamCamera`, which calls `cameraInit`. In the inspected upstream implementation, that routine initializes SPI and pulls the camera CS low. The supplied experimental variant changes it to configure CS as high and leaves bus initialization to a later transaction. This separates object construction from first SPI use and keeps an unselected device inactive.

The two-bus driver routes transfers through an `activeSpi` pointer, selected from CS10/21. It initializes FSPI and HSPI once and explicitly deselects both devices before selecting one. This is a useful driver-level experiment to inspect; the archive does not show it being compiled, and the later successful independent-bus run cannot be attributed to this exact archived source without the flashed version.

See the [exact comparison and provenance](ARDUCAM_PATCH.md). No unique causal diagnosis is claimed: wiring and firmware changed during troubleshooting without a complete change log.
