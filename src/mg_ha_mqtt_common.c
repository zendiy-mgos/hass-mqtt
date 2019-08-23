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

#include <stdlib.h>
#include <stdarg.h>
#include "mgos.h"
#include "mgos_mqtt.h"
#include "mg_ha_common.h"
#include "mg_ha_mqtt_common.h"
#include "mg_ha_mqtt_utils.h"

void mg_ha_mqtt_base_actuator_props_free(ha_mqtt_base_actuator_props_t props) {
  free(props.command_topic);
  free(props.command_parsef);  
}

void mg_ha_mqtt_base_sensor_props_free(ha_mqtt_base_sensor_props_t props) {  
  free(props.state_topic);
  free(props.state_pubf);
  free(props.attribs_topic);
  free(props.attribs_pubf);
}

bool mg_ha_mqtt_try_pub_birth_message() {  
  if (mgos_sys_config_get_hass_mqtt_availability_enable_birth_msg()) {
    if (mg_ha_mqtt_pub(mgos_sys_config_get_mqtt_will_topic(),
          mgos_sys_config_get_hass_mqtt_availability_state_on(),
          mgos_sys_config_get_hass_mqtt_availability_state_pubf())) {
      LOG(LL_DEBUG, ("Device-birth message has been published"));
      return true;
    }
  }
  return false;
}

/* BEGIN - MJS helper functions */

void mjs_ha_mqtt_base_sensor_cfg_set(ha_mqtt_base_sensor_cfg_t *s,
                                     const char *attribs_pubf,
                                     const char *attribs_topic,
                                     const char *state_pubf,
                                     const char *state_topic) {
  s->attribs_pubf = attribs_pubf;
  s->attribs_topic = attribs_topic;
  s->state_pubf = state_pubf;
  s->state_topic = state_topic;
}

void mjs_ha_mqtt_base_actuator_cfg_set(ha_mqtt_base_actuator_cfg_t *s,
                                       const char *command_topic,
                                       const char *command_parsef) {
  s->command_topic = command_topic;
  s->command_parsef = command_parsef;
}

/* END - MJS helper functions */