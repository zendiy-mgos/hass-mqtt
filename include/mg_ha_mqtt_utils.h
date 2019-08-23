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
 * Home Assistant | MQTT utilities and helpers.
 */

#ifndef MG_HA_MQTT_UTILS_H
#define MG_HA_MQTT_UTILS_H

#include <stdio.h>
#include "mg_ha_mqtt_common.h"

#ifdef __cplusplus
extern "C" {
#endif

bool mg_ha_mqtt_fpub(const char *topic, const char *fmt, ...);

bool mg_ha_mqtt_pub(const char *topic,
                    const char *value,
                    const char *fmt);

bool mg_ha_mqtt_entity_state_pubf(ha_mqtt_base_sensor_props_t *props,
                                  const char *attribs, ...);

bool mg_ha_mqtt_entity_state_pub(ha_mqtt_base_sensor_props_t *props,
                                 const char *state,
                                 const char *attribs);

bool mg_ha_mqtt_entity_xstate_pub(ha_mqtt_base_sensor_props_t *props,
                                  ha_entity_xstate_t *state);

bool mg_ha_mqtt_base_actuator_props_init(HA_ENTITY_HANDLE handle, 
                                         ha_mqtt_base_actuator_props_t *props,
                                         ha_mqtt_base_actuator_cfg_t *cfg);

bool mg_ha_mqtt_base_sensor_props_init(HA_ENTITY_HANDLE handle,
                                       ha_mqtt_base_sensor_props_t *props,
                                       ha_mqtt_base_sensor_cfg_t *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_HA_MQTT_UTILS_H */