# TOLOSAT Thermal Library

The thermal library contains the DS18 temperature-sensor driver and the PUS 178 thermal service. Its public APIs are exported from `include/drivers/` and `include/service/`; implementations are separated into driver and service components.

From the parent flight-software repository:

```bash
make thermal
make thermal-verif
make thermal-clean
```

The archive is written to `build/libs/libthermal.a`.
