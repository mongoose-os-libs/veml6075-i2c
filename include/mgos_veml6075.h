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

#ifndef _VEML6075_H
#define _VEML6075_H

#include "mgos.h"

#define MGOS_VEML6075_READ_DELAY (2)

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_veml6075;

/*
 * Initialize a VEML6075 on the master I2C bus at address specified in `i2caddr`
 * parameter (default VEML607 is on address 0x10). The sensor will be polled for
 * its deviceid, upon success a new `struct mgos_veml6075` is allocated and
 * returned. If the device could not be found, NULL is returned.
 */
struct mgos_veml6075 *mgos_veml6075_create(uint8_t i2caddr);

/*
 * Destroy the data structure associated with a VEML6075 device. The reference
 * to the pointer of the `struct mgos_veml6075` has to be provided, and upon
 * successful destruction, its associated memory will be freed and the pointer
 * set to NULL.
 */
void mgos_veml6075_destroy(struct mgos_veml6075 **sensor);

/*
 * The sensor will be polled for its light counter registers. If the poll has
 * occured in the last `MGOS_VEML6075_READ_DELAY` seconds, the cached data is
 * used (so as not to repeatedly poll the bus upon subsequent calls).
 *
 * The UVA band intensity is returned as a float (0.0 means "no light", higher
 * numbers mean more light was measured).
 */
float mgos_veml6075_getUVA(struct mgos_veml6075 *sensor);

/*
 * The sensor will be polled for its light counter registers. If the poll has
 * occured in the last `MGOS_VEML6075_READ_DELAY` seconds, the cached data is
 * used (so as not to repeatedly poll the bus upon subsequent calls).
 *
 * The UVB band intensity is returned as a float (0.0 means "no light", higher
 * numbers mean more light was measured).
 */
float mgos_veml6075_getUVB(struct mgos_veml6075 *sensor);

/*
 * The sensor will be polled for its light counter registers. If the poll has
 * occured in the last `MGOS_VEML6075_READ_DELAY` seconds, the cached data is
 * used (so as not to repeatedly poll the bus upon subsequent calls).
 *
 * Please see https://www.vishay.com/docs/84339/designingveml6075.pdf for
 * details.
 *
 * The UVIndex is computed from UVA, UVB, IR and Visual counters and returned.
 * *   <2.0  - Low UVIndex intensity
 * *   <5.5  - Moderate
 * *   <7.5  - High
 * *   <10.5 - Very High
 * *   >10.5 - Extreme UVIndex intensity
 *
 */
float mgos_veml6075_getUVIndex(struct mgos_veml6075 *sensor);

#ifdef __cplusplus
}
#endif

#endif // _VEML6075_H

