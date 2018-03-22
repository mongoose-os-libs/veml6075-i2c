#ifndef _VEML6075_INTERNAL_H
#define _VEML6075_INTERNAL_H

#include "mgos.h"
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
  uint8_t i2caddr;
  uint8_t config;
  uint16_t raw_uva, raw_uvb, raw_dark, raw_vis, raw_ir;
  double last_read_time;
};

struct mgos_veml6075 *mgos_veml6075_create(uint8_t i2caddr);
void mgos_veml6075_destroy(struct mgos_veml6075 **sensor);
bool mgos_veml6075_read(struct mgos_veml6075 *sensor);
float mgos_veml6075_getUVA(struct mgos_veml6075 *sensor);
float mgos_veml6075_getUVB(struct mgos_veml6075 *sensor);
float mgos_veml6075_getUVIndex(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getDevID(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getRawUVA(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getRawUVB(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getRawDark(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getRawVisComp(struct mgos_veml6075 *sensor);
uint16_t mgos_veml6075_getRawIRComp(struct mgos_veml6075 *sensor);

#ifdef __cplusplus
}
#endif

#endif // _VEML6075_INTERNAL_H

