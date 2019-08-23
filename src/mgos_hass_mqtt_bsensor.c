

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
#include "mgos_hass_mqtt_bsensor.h"

ha_mqtt_bsensor_cfg_t bsensor_default_cfg = MK_HA_MQTT_BSENSOR_CFG();

/* 
 * Representation of a binary sensor that
 * publishes its state.
 * 
 * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 *   - hass/bsensor_h/ha_bsensor_t
 */
typedef struct ha_mqtt_bsensor {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_entity_xstate_history_t state_history;
  ha_bsensor_props_t bsensor_props;
  /* above inherited from 'ha_bsensor_t' */
  ha_mqtt_base_sensor_props_t mqtt_sens_props;
} ha_mqtt_bsensor_t;

#define GET_HA_MQTT_BSENSOR_T(h) ((ha_mqtt_bsensor_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

HA_ENTITY_HANDLE mgos_hass_bsensor_create(ha_entity_cfg_t *entity_cfg,
                                          ha_mqtt_bsensor_cfg_t *cfg) {
  cfg = (cfg == NULL ? &bsensor_default_cfg : cfg);

  ha_mqtt_bsensor_t *e = (ha_mqtt_bsensor_t *)calloc(sizeof(ha_mqtt_bsensor_t), 1);
  if (!mg_ha_bsensor_creating((ha_bsensor_t *)e, entity_cfg,
        &cfg->pub_cfg, &cfg->bsensor_cfg)) {
    free(e);
    return NULL;
  }

  bool success = mg_ha_mqtt_base_sensor_props_init(e->handle,
    &e->mqtt_sens_props, &cfg->sens_cfg);

  if (!success) {
    LOG(LL_ERROR, ("Failed to create the %s '%s'.",
      HA_BSENSOR_TYPE, (entity_cfg != NULL ? entity_cfg->object_id : NULL)));
    mgos_hass_bsensor_close(e->handle);
    return NULL;
  }
 
  if (!mg_ha_entity_reg(e->handle)) {
    LOG(LL_ERROR, ("Failed to register entity '%s'. Something may not work as expected.",
      entity_cfg->object_id));
  }

  LOG(LL_DEBUG, ("The %s '%s' successfully created",
    e->handle->entity_type, e->handle->object_id));
  return e->handle;
}

bool ha_bsensor_state_on_pub(HA_ENTITY_HANDLE handle,
                             ha_entity_bstate_t *state) {
  ha_mqtt_bsensor_t *e = GET_HA_MQTT_BSENSOR_T(handle);
  if (e != NULL && state->value != UNKNOWN) {
    const char *ss = (state->value == ON ? e->bsensor_props.state_payload.on :
      e->bsensor_props.state_payload.off);
    return mg_ha_mqtt_entity_state_pub(&e->mqtt_sens_props,
      ss, state->attribs);
  }
  LOG(LL_ERROR, ("Failed to publish '%s' state.", handle->object_id));
  return false;
}

void mgos_hass_bsensor_close(HA_ENTITY_HANDLE handle) {
  ha_mqtt_bsensor_t *e = GET_HA_MQTT_BSENSOR_T(handle);
  if (e == NULL) return;
  mg_ha_mqtt_base_sensor_props_free(e->mqtt_sens_props);
  mg_ha_bsensor_closing((ha_bsensor_t *)e);
}

/* BEGIN - MJS helper functions */

ha_mqtt_bsensor_cfg_t *mjs_ha_mqtt_bsensor_cfg_mk() { 
  return (ha_mqtt_bsensor_cfg_t *)calloc(sizeof(ha_mqtt_bsensor_cfg_t), 1);
}

void mjs_ha_mqtt_bsensor_cfg_set1(ha_mqtt_bsensor_cfg_t *c,
                                  bool lazy_pub,
                                  enum mgos_hass_pub_event pub_on,
                                  int timer_timeout) {
  mjs_ha_base_pub_cfg_set(&c->pub_cfg, lazy_pub, pub_on, timer_timeout);
}

void mjs_ha_mqtt_bsensor_cfg_set2(ha_mqtt_bsensor_cfg_t *c,
                                  const char *attribs_pubf,
                                  const char *attribs_topic,
                                  const char *state_pubf,
                                  const char *state_topic) {
  mjs_ha_mqtt_base_sensor_cfg_set(&c->sens_cfg, attribs_pubf,
                                  attribs_topic, state_pubf,
                                  state_topic);
}

void mjs_ha_mqtt_bsensor_cfg_set3(ha_mqtt_bsensor_cfg_t *c,
                                  const char *on, const char *off) { 
  mjs_ha_bsensor_cfg_set(&c->bsensor_cfg, on, off);
}

/* END - MJS helper functions */