# VEML6075 I2C Driver

A Mongoose library for Vishay Technologies UVA/UVB intensity sensor.

## Implementation details

The VEML6075 sensor can be configured in continuous or one-shot measurement
modes, and upon each measurement returns the 16-bit raw counters for UVA, UVB,
visual and IR bands. Using conversion coefficients, the UVIndex can be returned
using these data points.

An accurate VEML6075 UVI sensing system requires visible and infrared noise
compensation and a teflon diffusor for cosine angular response correction. The
UVI formulas and related UVI formula coefficients are discussed here in detail.

This library implements the application [notes](https://www.vishay.com/docs/84339/designingveml6075.pdf)

### Limitations

The sensor measures for a configurable interval and integrates the total light
received for each band. Although its sensitivity can easily be adjusted with
selecting the proper integration times (from 50ms through to 800ms), the
algorithm provided in the Vishay paper is only applicable to 100ms integration
times. Therefore, the library currently does not support other integration
times.

## Example application

An example program using a timer to read data from the sensor every 5 seconds:

```
#include "mgos.h"
#include "mgos_veml6075.h"

static struct mgos_veml6075 *s_veml6075;

static void timer_cb(void *user_data) {
  float uva, uvb, uvindex;

  uva=mgos_veml6075_getUVA(s_veml6075);
  uvb=mgos_veml6075_getUVB(s_veml6075);
  uvindex=mgos_veml6075_getUVIndex(s_veml6075);

  LOG(LL_INFO, ("VEML6075 uva=%.1f uvb=%.1f uvindex=%.2f usecs=%u", uva, uvb, uvindex));

  (void) user_data;
}

enum mgos_app_init_result mgos_app_init(void) {
  s_veml6075=mgos_veml6075_create(0x10); // Default I2C address
  if (s_veml6075) {
    mgos_set_timer(5000, true, timer_cb, NULL);
  } else {
    LOG(LL_ERROR, ("Could not initialize sensor"));
  }
  return MGOS_APP_INIT_SUCCESS;
}
```


# Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
