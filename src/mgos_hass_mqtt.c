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

#include "mgos.h"
#include "mgos_mqtt.h"
#include "mgos_hass_mqtt.h"

static void mg_ha_on_mqtt_event(struct mg_connection *nc, int ev,
                                void *ev_data, void *user_data) {  
  (void) ev_data;
  (void) nc;
  (void) user_data;
  if (ev == MG_EV_MQTT_CONNACK) {
    /* Try to publish the BIRTH message */
    mg_ha_mqtt_try_pub_birth_message();   
    /* Starts triggers for publishing the state of subscribed entities */
    mg_ha_state_pub_triggers_start();
  } else if (ev == MG_EV_MQTT_DISCONNECT) {
    /* Kills triggers for publishing the state of subscribed entities */
    mg_ha_state_pub_triggers_clear();
  }
}

bool mgos_hass_mqtt_init(void) {
  /* update will topic and set the LWT message if not already configured */
  const char *will_topic = mgos_sys_config_get_mqtt_will_topic();
  if (will_topic != NULL) {
    /* update the will_topic */
    char *nwt = NULL;
    if (mg_ha_sprintf(will_topic, HA_ENV_VAR_DEV_ID, mgos_sys_config_get_device_id(), &nwt)) {
      /* replace $device_id placehoder/s in will_topic */
      mgos_sys_config_set_mqtt_will_topic(nwt);
      LOG(LL_DEBUG, ("mqtt.will_topic changed to %s", nwt));
      free(nwt);
    }
 
    /* set the LWT message if not already configured */
    if (mgos_sys_config_get_mqtt_will_message() == NULL) {
      const char *state_off = mgos_sys_config_get_hass_mqtt_availability_state_off();
      if (state_off != NULL) {
        const char *state_pubf = mgos_sys_config_get_hass_mqtt_availability_state_pubf();
        char *will_message = state_pubf == NULL ? NULL : json_asprintf(state_pubf, state_off);
        mgos_sys_config_set_mqtt_will_message((will_message == NULL ? state_off : will_message));
        LOG(LL_DEBUG, ("mqtt.will_message changed to %s", will_message));
        free(will_message);
      }
    }
  } else {
    LOG(LL_DEBUG, ("MQTT 'availability' features wont work"));
  }
  
  mgos_mqtt_add_global_handler(mg_ha_on_mqtt_event, NULL);

  return true;
}