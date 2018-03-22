#ifndef _VEML6075_H
#define _VEML6075_H

#include "mgos.h"

#define MGOS_VEML6075_READ_DELAY (2)

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_veml6075;

struct mgos_veml6075 *mgos_veml6075_create(uint8_t i2caddr);
void mgos_veml6075_destroy(struct mgos_veml6075 **sensor);
float mgos_veml6075_getUVA(struct mgos_veml6075 *sensor);
float mgos_veml6075_getUVB(struct mgos_veml6075 *sensor);
float mgos_veml6075_getUVIndex(struct mgos_veml6075 *sensor);

#ifdef __cplusplus
}
#endif

#endif // _VEML6075_H

