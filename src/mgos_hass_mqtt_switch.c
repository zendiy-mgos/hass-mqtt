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
#include <string.h>
#include "mgos.h"
#include "mgos_mqtt.h"
#include "mg_ha_utils.h"
#include "mg_ha_mqtt_utils.h"
#include "mgos_hass_mqtt_switch.h"

ha_mqtt_switch_cfg_t switch_default_cfg = MK_HA_MQTT_SWITCH_CFG();

/* 
 * Representation of a switch that publishes
 * its state and receives ON/OFF commands.
 * 
 * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 *   - hass/common_h/ha_base_actuator_t
 *   - hass/switch_h/ha_switch_t
 */
typedef struct ha_mqtt_switch {
   HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_base_actuator_props_t actu_props;
  /* above inherited from 'ha_base_actuator_t' */
  ha_entity_xstate_history_t state_history;
  ha_switch_props_t switch_props;
  /* above inherited from 'ha_switch_t' */
  ha_mqtt_base_sensor_props_t mqtt_sens_props;
  ha_mqtt_base_actuator_props_t mqtt_actu_props;  
} ha_mqtt_switch_t;

#define GET_HA_MQTT_SWITCH_T(h) ((ha_mqtt_switch_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

void mg_ha_switch_on_cmd(struct mg_connection *nc, const char *topic,
                         int topic_len, const char *msg, int msg_len,
                         void *ud) {
  ha_mqtt_switch_t *e = GET_HA_MQTT_SWITCH_T(ud);
  if (e != NULL && msg != NULL && msg_len > 0) {
    enum ha_toggle_state state = UNKNOWN;

    if (e->mqtt_actu_props.command_parsef == NULL) {
      if (strncmp(e->switch_props.state_payload.on, msg, msg_len) == 0) {
        state = ON;
      } else if (strncmp(e->switch_props.state_payload.off, msg, msg_len) == 0) {
        state = OFF;
      }
    } else {
      char *desire_state = NULL;
      if (json_scanf(msg, msg_len, e->mqtt_actu_props.command_parsef, &desire_state) > 0) {
        if (strcmp(e->switch_props.state_payload.on, desire_state) == 0) {
          state = ON;
        } else if (strcmp(e->switch_props.state_payload.off, desire_state) == 0) {
          state = OFF;
        }
      }
      free(desire_state);
    }
    
    if (state != UNKNOWN) {
      mgos_hass_switch_state_set((HA_ENTITY_HANDLE)ud, state);
    } else {
      char *up = (char *)calloc((msg_len + 1), 1);
      strncpy(up, msg, msg_len);
      LOG(LL_ERROR, ("Unexpected command payload: '%s'.", up));
      free(up);
    }
  }
  (void) nc;
  (void) topic;
  (void) topic_len;
}

HA_ENTITY_HANDLE mgos_hass_switch_create(ha_entity_cfg_t *entity_cfg,
                                         ha_mqtt_switch_cfg_t *cfg) {
  cfg = (cfg == NULL ? &switch_default_cfg : cfg);

  ha_mqtt_switch_t *e = (ha_mqtt_switch_t *)calloc(sizeof(ha_mqtt_switch_t), 1);
  if (!mg_ha_switch_creating((ha_switch_t *)e, entity_cfg,
        &cfg->pub_cfg, &cfg->switch_cfg)) {
    free(e);
    return NULL;
  }
  
  bool success = mg_ha_mqtt_base_sensor_props_init(e->handle,
    &e->mqtt_sens_props, &cfg->sens_cfg);

  success = mg_ha_mqtt_base_actuator_props_init(e->handle,
    &e->mqtt_actu_props, &cfg->actu_cfg) && success;

  if (!success) {
    LOG(LL_ERROR, ("Failed to create the %s '%s'.",
      HA_SWITCH_TYPE, (entity_cfg != NULL ? entity_cfg->object_id : NULL)));
    mgos_hass_switch_close(e->handle);
    return NULL;
  }

  if (!mg_ha_entity_reg(e->handle)) {
    LOG(LL_ERROR, ("Failed to register entity '%s'. Something may not work as expected.",
      entity_cfg->object_id));
  }

  /* Subscribe to the commnad topic */
  if (e->mqtt_actu_props.command_topic != NULL) {
    mgos_mqtt_sub(e->mqtt_actu_props.command_topic,
      mg_ha_switch_on_cmd, e->handle);
  } else {
    LOG(LL_ERROR, ("The switch '%s' didn't subcribe to the command topic.",
      entity_cfg->object_id));
  }
  
  LOG(LL_DEBUG, ("The %s '%s' successfully created",
    e->handle->entity_type, e->handle->object_id));

  return e->handle;   
}

bool ha_switch_state_on_pub(HA_ENTITY_HANDLE handle,
                            ha_entity_bstate_t *state) {
  ha_mqtt_switch_t *e = GET_HA_MQTT_SWITCH_T(handle);
  if (e != NULL && state != NULL && state->value != UNKNOWN) {
    return mg_ha_mqtt_entity_state_pub(&e->mqtt_sens_props,
      (state->value == ON ? e->switch_props.state_payload.on : e->switch_props.state_payload.off),
      state->attribs);
  }
  LOG(LL_ERROR, ("Failed to publish '%s' state.", handle->object_id));
  return false;
}

void mgos_hass_switch_close(HA_ENTITY_HANDLE handle) {
  ha_mqtt_switch_t *e = GET_HA_MQTT_SWITCH_T(handle);  
  if (e == NULL) return;
  mg_ha_mqtt_base_sensor_props_free(e->mqtt_sens_props);
  mg_ha_mqtt_base_actuator_props_free(e->mqtt_actu_props);        
  mg_ha_switch_closing((ha_switch_t *)e);
}

/* BEGIN - MJS helper functions */

ha_mqtt_switch_cfg_t *mjs_ha_mqtt_switch_cfg_mk() { 
  return (ha_mqtt_switch_cfg_t *)calloc(sizeof(ha_mqtt_switch_cfg_t), 1);
}

void mjs_ha_mqtt_switch_cfg_set1(ha_mqtt_switch_cfg_t * c,
                                 bool lazy_pub,
                                 enum mgos_hass_pub_event pub_on,
                                 int timer_timeout) {
  mjs_ha_base_pub_cfg_set(&c->pub_cfg, lazy_pub, pub_on, timer_timeout);
}

void mjs_ha_mqtt_switch_cfg_set2(ha_mqtt_switch_cfg_t *c,
                                 const char *attribs_pubf,
                                 const char *attribs_topic,
                                 const char *state_pubf,
                                 const char *state_topic) {
  mjs_ha_mqtt_base_sensor_cfg_set(&c->sens_cfg, attribs_pubf,
                                  attribs_topic, state_pubf,
                                  state_topic);
}

void mjs_ha_mqtt_switch_cfg_set3(ha_mqtt_switch_cfg_t *c,
                                 const char *on, const char *off,
                                 enum ha_toggle_state normal_state,
                                 int inching_timeout) { 
  mjs_ha_switch_cfg_set(&c->switch_cfg, on, off,
   normal_state, inching_timeout);
}

void mjs_ha_mqtt_switch_cfg_set4(ha_mqtt_switch_cfg_t *c,
                                 const char *command_topic,
                                 const char *command_parsef) { 
  mjs_ha_mqtt_base_actuator_cfg_set(&c->actu_cfg, 
    command_topic, command_parsef);
}

/* END - MJS helper functions */