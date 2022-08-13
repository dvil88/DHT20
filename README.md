# DHT20 Sensor Driver for Mongoose OS

## Overview

[DHT20](https://cdn.sparkfun.com/assets/8/a/1/5/0/DHT20.pdf) is a temperature and relative humidity sensor by ASAIR.

This library provides a driver for this device.

This driver is being ported from [https://github.com/RobTillaart/DHT20](https://github.com/RobTillaart/DHT20)

## API documentation

See `include/dht20.h`.

## Example


```c
#include "mgos.h"

#include "dht20.h"

static void temp_timer_cb(void *arg) {
  dht20_read();
  float temp = dht20_get_temperature();
  float rh = dht20_get_humidity();
  LOG(LL_INFO, ("T: %.2f*C\tRH: %.2f%%", temp, rh));
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  mgos_set_timer(1000, MGOS_TIMER_REPEAT, temp_timer_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}
```

_Note:_ You need to make sure that I2C is enabled. This can be achieved by adding
```yaml
config_schema:
  - ["i2c.enable", true]
```
to mos.yml. You may need to adjust SDA and SCL pins as well.
See [I2C](https://github.com/mongoose-os-libs/i2c) library for details.
