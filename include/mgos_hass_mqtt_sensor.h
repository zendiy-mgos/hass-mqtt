/*
 * Copyright (c) 2018 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Home Assistant | MQTT sensors library.
 */

#ifndef MGOS_HASS_MQTT_SENSOR_H
#define MGOS_HASS_MQTT_SENSOR_H

#include <stdbool.h>
#include "mgos_hass.h"
#include "mgos_hass_sensor.h"
#include "mg_ha_mqtt_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MK_HA_MQTT_SENSOR_CFG(void) {MK_HA_BASE_PUB_CFG(), MK_HA_MQTT_BASE_SENSOR_CFG()}
typedef struct ha_mqtt_sensor_cfg {
  ha_base_pub_cfg_t pub_cfg;
  ha_mqtt_base_sensor_cfg_t sens_cfg; 
} ha_mqtt_sensor_cfg_t;

HA_ENTITY_HANDLE mgos_hass_sensor_create(ha_entity_cfg_t *entity_cfg,      
                                         ha_mqtt_sensor_cfg_t *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_HASS_MQTT_SENSOR_H */
