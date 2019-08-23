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
 * Home Assistant | MQTT Common APIs, data types, and structures.
 */

#ifndef MG_HA_MQTT_COMMON_H
#define MG_HA_MQTT_COMMON_H

#include <stdio.h>
#include "mgos_hass.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Base configuration settings for sensors.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_MQTT_BASE_SENSOR_CFG>, and
 * the function mjs_ha_mqtt_base_sensor_cfg_set(...) as well.
 */
#define MK_HA_MQTT_BASE_SENSOR_CFG(void) {NULL, NULL, NULL, NULL}
typedef struct ha_mqtt_base_sensor_cfg {
  const char *state_topic;
  const char *state_pubf;
  const char *attribs_topic;
  const char *attribs_pubf;
} ha_mqtt_base_sensor_cfg_t;

typedef struct ha_mqtt_base_sensor_props {
  char *state_topic;
  char *state_pubf;
  char *attribs_topic;
  char *attribs_pubf;
  enum mgos_hass_pub_event pub_on;
} ha_mqtt_base_sensor_props_t;

/* 
 * Base configuration settings for actuators.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_MQTT_BASE_ACTUATOR_CFG>, and
 * the function mjs_ha_mqtt_base_actuator_cfg_set(...) as well.
 */
#define MK_HA_MQTT_BASE_ACTUATOR_CFG(void) {NULL, NULL}
typedef struct ha_mqtt_base_actuator_cfg {
  const char *command_topic;
  const char *command_parsef;
} ha_mqtt_base_actuator_cfg_t;

typedef struct ha_mqtt_base_actuator_props {
  char *command_topic;
  char *command_parsef;
} ha_mqtt_base_actuator_props_t;

void mg_ha_mqtt_base_actuator_props_free(ha_mqtt_base_actuator_props_t props);
void mg_ha_mqtt_base_sensor_props_free(ha_mqtt_base_sensor_props_t props);

bool mg_ha_mqtt_try_pub_birth_message();

/* BEGIN - MJS helper functions */

void mjs_ha_mqtt_base_sensor_cfg_set(ha_mqtt_base_sensor_cfg_t *s,
                                     const char *attribs_pubf,
                                     const char *attribs_topic,
                                     const char *state_pubf,
                                     const char *state_topic);

void mjs_ha_mqtt_base_actuator_cfg_set(ha_mqtt_base_actuator_cfg_t *s,
                                       const char *command_topic,
                                       const char *command_parsef);

/* END - MJS helper functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_HA_MQTT_COMMON_H */