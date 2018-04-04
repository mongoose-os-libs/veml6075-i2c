/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _VEML6075_INTERNAL_H
#define _VEML6075_INTERNAL_H

#include "mgos.h"
#include "mgos_veml6075.h"
#include <math.h>

#define VEML6075_DEVID                    (0x26)

#define VEML6075_REGISTER_CONF            (0x00)
#define VEML6075_REGISTER_UVA             (0x07)
#define VEML6075_REGISTER_DARK            (0x08)
#define VEML6075_REGISTER_UVB             (0x09)
#define VEML6075_REGISTER_VIS             (0x0a)
#define VEML6075_REGISTER_IR              (0x0b)
#define VEML6075_REGISTER_DEVID           (0x0c)

#define VEML6075_CONFIGURATION_HD_NORM    (0x00)
#define VEML6075_CONFIGURATION_HD_HIGH    (0x08)
#define VEML6075_CONFIGURATION_TRIG       (0x04)
#define VEML6075_CONFIGURATION_AF_OFF     (0x00)
#define VEML6075_CONFIGURATION_AF_ON      (0x02)
#define VEML6075_CONFIGURATION_SD_OFF     (0x00)
#define VEML6075_CONFIGURATION_SD_ON      (0x01)

#define VEML6075_INTEGRATION_50MS         (0x00)
#define VEML6075_INTEGRATION_100MS        (0x10)
#define VEML6075_INTEGRATION_200MS        (0x20)
#define VEML6075_INTEGRATION_400MS        (0x30)
#define VEML6075_INTEGRATION_800MS        (0x40)
#define VEML6075_INTEGRATION_MASK         (0x8F)

// Taken from application note:
// http://www.vishay.com/docs/84339/designingveml6075.pdf

#define VEML6075_UVI_UVA_VIS_COEFF (2.22)
#define VEML6075_UVI_UVA_IR_COEFF  (1.33)
#define VEML6075_UVI_UVB_VIS_COEFF (2.95)
#define VEML6075_UVI_UVB_IR_COEFF  (1.74)
#define VEML6075_UVI_UVA_RESPONSE (1.0 / 909.0)
#define VEML6075_UVI_UVB_RESPONSE (1.0 / 800.0)

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_veml6075 {
  struct mgos_i2c *i2c;
  uint8_t i2caddr;
  uint8_t config;
  struct mgos_veml6075_stats stats;

  uint16_t raw_uva, raw_uvb, raw_dark, raw_vis, raw_ir;
};

static bool mgos_veml6075_read(struct mgos_veml6075 *sensor);
static uint16_t mgos_veml6075_getDevID(struct mgos_veml6075 *sensor);

#ifdef __cplusplus
}
#endif

#endif // _VEML6075_INTERNAL_H

