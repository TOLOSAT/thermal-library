# TOLOSAT Thermal Library

The thermal library contains the DS18 temperature-sensor driver and the PUS 178 thermal service. Its public APIs are exported from `include/drivers/` and `include/service/`; implementations are separated into driver and service components.

From the parent flight-software repository:

```bash
make thermal
make thermal-verif
make thermal-clean
```

The archive is written to `build/libs/libthermal.a`.

## Configuration

Library options belong in `gen/Kconfig.options`. The parent project sources this fragment and generates a library-scoped `autoconf.h`; the fragment is currently empty. A standalone checkout can maintain its own configuration with `make default_defconfig` and `make menuconfig`. Initialise the `tools/` submodule recursively before generating or building from a standalone checkout.

## License

This library is licensed under the Apache License 2.0. See [`LICENSE`](LICENSE)
for details.
