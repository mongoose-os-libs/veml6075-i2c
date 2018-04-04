/*
 * Copyright 2018 Google Inc.
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

#include "mgos.h"
#include "mgos_veml6075.h"
#include "mgos_veml6075_internal.h"
#include "mgos_i2c.h"

// Private functions start
static uint16_t mgos_veml6075_i2c_read16(struct mgos_veml6075 *sensor, uint8_t reg) {
  uint8_t data[2];
  uint16_t value;
  bool ret;

  if (!sensor || !sensor->i2c) {
    LOG(LL_ERROR, ("Sensor or I2C bus is not configured"));
    return 0;
  }
  ret = mgos_i2c_write(sensor->i2c, sensor->i2caddr, &reg, 1, false);
  if (!ret) {
    LOG(LL_ERROR, ("I2C=0x%02x reg=%u (0x%02x) write error", sensor->i2caddr, reg, reg));
    return 0;
  }
  ret = mgos_i2c_read(sensor->i2c, sensor->i2caddr, data, 2, true);
  value = (data[1]<<8) | data[0];
  if (!ret) {
    LOG(LL_ERROR, ("I2C=0x%02x reg=0x%02x val=%u (0x%04x) read error", sensor->i2caddr, reg, value, value));
    return 0;
  }
  LOG(LL_DEBUG, ("I2C=0x%02x reg=0x%02x val=%u (0x%04x) read success", sensor->i2caddr, reg, value, value));

  return value;
}

static bool mgos_veml6075_i2c_write16(struct mgos_veml6075 *sensor, uint8_t reg, uint16_t value) {
  uint8_t data[3];
  bool ret;

  if (!sensor || !sensor->i2c) {
    LOG(LL_ERROR, ("Sensor or I2C bus is not configured"));
    return 0;
  }
  data[0]=reg;
  data[1]=(uint8_t)(0xFF & (value >> 0));
  data[2]=(uint8_t)(0xFF & (value >> 8));
  ret = mgos_i2c_write(sensor->i2c, sensor->i2caddr, data, 3, true);
  if (!ret) {
    LOG(LL_ERROR, ("I2C=0x%02x reg=0x%02x val=%u (0x%04x) write error", sensor->i2caddr, reg, value, value));
    return false;
  }
  LOG(LL_DEBUG, ("I2C=0x%02x reg=0x%02x val=%u (0x%04x) write success", sensor->i2caddr, reg, value, value));

  return true;
}

static uint16_t mgos_veml6075_getDevID(struct mgos_veml6075 *sensor) {
  if (!sensor) return 0;
  return mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_DEVID);
}

static bool mgos_veml6075_read(struct mgos_veml6075 *sensor) {
  double start = mg_time();

  if (!sensor || !sensor->i2c) return false;
  sensor->stats.read++;

  if (start - sensor->stats.last_read_time < MGOS_VEML6075_READ_DELAY) {
    sensor->stats.read_success_cached++;
    return true;
  }

  sensor->raw_uva = mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_UVA);
  sensor->raw_uvb = mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_UVB);
  sensor->raw_dark = mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_DARK);
  sensor->raw_vis = mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_VIS);
  sensor->raw_ir = mgos_veml6075_i2c_read16(sensor, VEML6075_REGISTER_IR);

  LOG(LL_DEBUG, ("raw_uva=%u, raw_uvb=%u, raw_dark=%u, raw_vis=%u, raw_ir=%u", sensor->raw_uva, sensor->raw_uvb, sensor->raw_dark, sensor->raw_vis, sensor->raw_ir));
  sensor->stats.read_success++;
  sensor->stats.read_success_usecs+=1000000*(mg_time()-start);
  sensor->stats.last_read_time=start;
  return true;
}

// Private functions end

// Public functions start
struct mgos_veml6075 *mgos_veml6075_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  struct mgos_veml6075 *sensor = calloc(1, sizeof(struct mgos_veml6075));
  uint16_t devid = 0;

  if (!sensor) return NULL;
  sensor->i2caddr=i2caddr;
  sensor->i2c=i2c;
  memset(sensor, 0, sizeof(struct mgos_veml6075));
  sensor->config|=VEML6075_CONFIGURATION_SD_OFF;
  sensor->config|=VEML6075_INTEGRATION_100MS;

  devid=mgos_veml6075_getDevID(sensor);
  if (devid != VEML6075_DEVID) {
    LOG(LL_ERROR, ("Failed to create VEML6075 sensor at I2C address 0x%02x, DevID mismatch (got 0x%04x, want 0x%04x)", sensor->i2caddr, devid, VEML6075_DEVID));
    free(sensor);
    return NULL;
  }

  // Write config to make sure device is enabled
  if (!mgos_veml6075_i2c_write16(sensor, VEML6075_REGISTER_CONF, sensor->config)) {
    LOG(LL_ERROR, ("Failed to create VEML6075 sensor at I2C address 0x%02x, write error setting conf register", sensor->i2caddr));
    free(sensor);
    return NULL;
  }
  LOG(LL_INFO, ("Created VEML6075 sensor at I2C address 0x%02x", sensor->i2caddr));
  return sensor;
}

void mgos_veml6075_destroy(struct mgos_veml6075 **sensor) {
  if (!*sensor) return;
  free (*sensor);
  *sensor=NULL;
  return;
}

float mgos_veml6075_getUVA(struct mgos_veml6075 *sensor) {
  float comp_vis;
  float comp_ir;
  float comp_uva;

  if (!mgos_veml6075_read(sensor)) return NAN;

  comp_vis = sensor->raw_vis - sensor->raw_dark;
  comp_ir = sensor->raw_ir - sensor->raw_dark;
  comp_uva = sensor->raw_uva - sensor->raw_dark;

  comp_uva -= (VEML6075_UVI_UVA_VIS_COEFF * comp_vis) - (VEML6075_UVI_UVA_IR_COEFF * comp_ir);

  return comp_uva;
}

float mgos_veml6075_getUVB(struct mgos_veml6075 *sensor) {
  float comp_vis;
  float comp_ir;
  float comp_uvb;

  if (!mgos_veml6075_read(sensor)) return NAN;

  comp_vis = sensor->raw_vis - sensor->raw_dark;
  comp_ir = sensor->raw_ir - sensor->raw_dark;
  comp_uvb = sensor->raw_uvb - sensor->raw_dark;

  comp_uvb -= (VEML6075_UVI_UVB_VIS_COEFF * comp_vis) - (VEML6075_UVI_UVB_IR_COEFF * comp_ir);

  return comp_uvb;
}

float mgos_veml6075_getUVIndex(struct mgos_veml6075 *sensor) {
  float uva_weighted;
  float uvb_weighted;

  if (!mgos_veml6075_read(sensor)) return NAN;

  uva_weighted = mgos_veml6075_getUVA(sensor) * VEML6075_UVI_UVA_RESPONSE;
  uvb_weighted = mgos_veml6075_getUVB(sensor) * VEML6075_UVI_UVB_RESPONSE;
  return (uva_weighted + uvb_weighted) / 2.0;
}

uint16_t mgos_veml6075_getRawUVA(struct mgos_veml6075 *sensor) {
  if (!mgos_veml6075_read(sensor)) return 0;
  return sensor->raw_uva;
}

uint16_t mgos_veml6075_getRawUVB(struct mgos_veml6075 *sensor) {
  if (!mgos_veml6075_read(sensor)) return 0;
  return sensor->raw_uvb;
}

uint16_t mgos_veml6075_getRawDark(struct mgos_veml6075 *sensor) {
  if (!mgos_veml6075_read(sensor)) return 0;
  return sensor->raw_dark;
}

uint16_t mgos_veml6075_getRawVisComp(struct mgos_veml6075 *sensor) {
  if (!mgos_veml6075_read(sensor)) return 0;
  return sensor->raw_vis;
}

uint16_t mgos_veml6075_getRawIRComp(struct mgos_veml6075 *sensor) {
  if (!mgos_veml6075_read(sensor)) return 0;
  return sensor->raw_ir;
}

bool mgos_veml6075_getStats(struct mgos_veml6075 *sensor, struct mgos_veml6075_stats *stats) {
  if (!sensor || !stats)
    return false;

  memcpy((void *)stats, (const void *)&sensor->stats, sizeof(struct mgos_veml6075_stats));
  return true;
}

bool mgos_veml6075_i2c_init(void) {
  return true;
}
// Public functions end
