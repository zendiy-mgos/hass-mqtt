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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "mgos.h"
#include "mgos_mqtt.h"
#include "mg_ha_common.h"
#include "mg_ha_utils.h"
#include "mg_ha_mqtt_common.h"
#include "mg_ha_mqtt_utils.h"

bool mg_ha_mqtt_vapubf(const char *topic, const char *fmt, va_list ap) {
  if (topic == NULL || fmt == NULL) {
    LOG(LL_ERROR, ("Error publishing message. Topic and message format cannot be NULL."));
    return false;
  }
  /* The format string has json format,
     so I publish a json formatted message */
  return mgos_mqtt_pubv(topic,
    mgos_sys_config_get_hass_mqtt_publish_qos(),
    mgos_sys_config_get_hass_mqtt_publish_retain(),
    fmt, ap);
}

bool mg_ha_mqtt_fpub(const char *topic, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  bool success = mg_ha_mqtt_vapubf(topic, fmt, ap);
  va_end(ap);
  return success;
}

bool mg_ha_mqtt_pub(const char *topic, 
                    const char *msg,
                    const char *fmt) {
  if (topic == NULL || msg == NULL) {
    LOG(LL_ERROR, ("Error publishing message. Topic and message cannot be NULL."));
    return false;
  }

  if (fmt == NULL) {
    /* I publish a plain message */
    return mgos_mqtt_pub(topic, msg, strlen(msg),
      mgos_sys_config_get_hass_mqtt_publish_qos(),
      mgos_sys_config_get_hass_mqtt_publish_retain());
  }

  /* The format string has json format,
      so I publish json formatted message */
  return mgos_mqtt_pubf(topic,
    mgos_sys_config_get_hass_mqtt_publish_qos(),
    mgos_sys_config_get_hass_mqtt_publish_retain(),
    fmt, msg);
}

bool mg_ha_mqtt_entity_state_pub(ha_mqtt_base_sensor_props_t *props,
                                 const char *state,
                                 const char *attribs) {
  if (props == NULL || state == NULL) return false;

  /* Publish the current state  */
  char *state_pubf = NULL;
  mg_ha_sprintf(props->state_pubf,
    HA_ENV_VAR_STATE_ATTRIBS, attribs, &state_pubf);
  bool success = mg_ha_mqtt_pub(props->state_topic, state,
    (state_pubf != NULL ? state_pubf : props->state_pubf));
  free(state_pubf);

  if (success == true && 
      attribs != NULL && props->attribs_topic != NULL) {
    /* Publish attributes  */
    mg_ha_mqtt_pub(props->attribs_topic, attribs, props->attribs_pubf);
  }
  return success;
}

bool mg_ha_mqtt_entity_state_pubf(ha_mqtt_base_sensor_props_t *props,
                                  const char *attribs, ...) {
  bool success = false;
  if (props != NULL) {   
    /* Publish the current state  */
    va_list ap;      
    va_start(ap, attribs);
    char *state_pubf = NULL;
    mg_ha_sprintf(props->state_pubf, HA_ENV_VAR_STATE_ATTRIBS,
      attribs, &state_pubf);
    success = mg_ha_mqtt_vapubf(props->state_topic,
      (state_pubf != NULL ? state_pubf : props->state_pubf), ap);
    free(state_pubf);
    va_end(ap);

    if (success == true &&
        attribs != NULL && props->attribs_topic != NULL) {
      /* Publish attributes  */
      mg_ha_mqtt_pub(props->attribs_topic,
        attribs, props->attribs_pubf);
    }
  }
  return success;
}

bool mg_ha_mqtt_entity_xstate_pub(ha_mqtt_base_sensor_props_t *props,
                                  ha_entity_xstate_t *state) {
  if (state == NULL) return false;
  switch(state->v_type) {
    case STRING:
      return mg_ha_mqtt_entity_state_pubf(props, state->attribs, state->s_value);
    case FLOATING_POINT:
      return mg_ha_mqtt_entity_state_pubf(props, state->attribs, state->f_value); 
    case INTEGER:
    case TOGGLE:
      return mg_ha_mqtt_entity_state_pubf(props, state->attribs, state->d_value);
    default:
      LOG(LL_ERROR, ("Invalid state data-type %d", state->v_type));
      break;
  };
  return false;
}

bool mg_ha_mqtt_base_actuator_props_init(HA_ENTITY_HANDLE handle, 
                                         ha_mqtt_base_actuator_props_t *props,
                                         ha_mqtt_base_actuator_cfg_t *cfg) {
  if (props != NULL) {
    props->command_topic = mg_ha_entity_sprintf(handle,
      (cfg != NULL ? cfg->command_topic : NULL),
      mgos_sys_config_get_hass_mqtt_command_topic());

    const char *tmp = (cfg != NULL ? cfg->command_parsef : NULL);
    tmp = (tmp != NULL ? tmp : mgos_sys_config_get_hass_mqtt_command_parsef());
    props->command_parsef = HA_SAFE_STRDUP(tmp);
  }   
  return (props != NULL);
}

bool mg_ha_mqtt_base_sensor_props_init(HA_ENTITY_HANDLE handle,
                                       ha_mqtt_base_sensor_props_t *props,
                                       ha_mqtt_base_sensor_cfg_t *cfg) {
  if (props != NULL) {
    const char *tmp = NULL;
    props->state_topic = mg_ha_entity_sprintf(handle,
      (cfg != NULL ? cfg->state_topic : NULL),
      mgos_sys_config_get_hass_mqtt_publish_state_topic());

    props->state_pubf = HA_SAFE_STRDUP(cfg->state_pubf);

    props->attribs_topic = mg_ha_entity_sprintf(handle,
      (cfg != NULL ? cfg->attribs_topic : NULL),
      mgos_sys_config_get_hass_mqtt_publish_attribs_topic());

    tmp = (cfg != NULL ? cfg->attribs_pubf : NULL);
    tmp = (tmp != NULL ? tmp : mgos_sys_config_get_hass_mqtt_publish_attribs_pubf());
    props->attribs_pubf = HA_SAFE_STRDUP(tmp);
  }
  return (props != NULL);
}

/* BEGIN - MJS helper functions */

/* END - MJS helper functions */