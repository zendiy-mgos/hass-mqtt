Home Assistant MQTT Devices Mongoose-OS library
===============================================
# Overview
The [Mongoose-OS](https://mongoose-os.com/) library for making [Home Assistant](https://www.home-assistant.io/) compatible MQTT devices. Following Home Assistant MQTT entities are currently supported:
 - Binary sensors
 - Sensors
 - Switches
# GET STARTED
Build up your device in few minutes just starting from one of the following samples.

|Sample|Notes|
|--|--|
|[hass-mqtt-demo](https://github.com/zendiy-mgos/hass-mqtt-demo)|Shows you how to build firmware for managing sensors, binary sensors, switches, etc. integrated in an all-in-one device.|
# Usage
Include the library into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/hass-mqtt
```
If you are developing a JavaScript firmware load into your `init.js` the `api_hass_mqtt.js` file.
```js
load('api_hass_mqtt.js');
```
# Configuration
The following configuration section is added to the `conf0.json` file in your Mongoose-OS project.
```json
"hass": {
  "mqtt": {
    "publish": {
      "state_topic": null,
      "attribs_topic": null,
      "attribs_pubf": null,
      "retain": true,
      "qos": 0
    },
    "command": {
      "topic": "",
      "parsef": ""
    },
    "availability": {
      "enable_birth_msg": true,
      "state_on": "online",
      "state_off": "offline",
      "state_pubf": null
    }
  }
}
```
|Property|Type|Default|Description|
|--|--|--|--|
|<a name="hass.mqtt.publish.state_topic"></a>hass.mqtt.publish.**state_topic**|string||Default topic for publishing state|
|<a name="hass.mqtt.publish.attribs_topic"></a>hass.mqtt.publish.**attribs_topic**|string||Default topic for publishing attributes|
|<a name="hass.mqtt.publish.attribs_pubf"></a>hass.mqtt.publish.**attribs_pubf**|string||The format string for publishig attributes json-payload|
|<a name="hass.mqtt.publish.retain"></a>hass.mqtt.publish.**retain**|boolean|`true`|If the published message should have the retain flag on or not|
|<a name="hass.mqtt.publish.qos"></a>hass.mqtt.publish.**qos**|integer|`0`|The maximum QoS level|
|<a name="hass.mqtt.command.topic"></a>hass.mqtt.command.**topic**|string||Default topic for subscribing to command|
|<a name="hass.mqtt.command.parsef"></a>hass.mqtt.command.**parsef**|string||The format string for parsing the command json-payload|
|<a name="hass.mqtt.availability.enable_birth_msg"></a>hass.mqtt.availability.**enable_birth_msg**|boolean|`true`|Enable/disable the MQTT birth message|
|<a name="hass.mqtt.availability.state_on"></a>hass.mqtt.availability.**state_on**|string|`'online'`|The value that represents the available state|
|<a name="hass.mqtt.availability.state_off"></a>hass.mqtt.availability.**state_off**|string|`'offline'`|The value that represents the unavailable state|
|<a name="hass.mqtt.availability.state_pubf"></a>hass.mqtt.availability.**state_pubf**|string||The format string for publishig the availability json-payload|
# Session variables
# C/C++ API reference
TABLE OF CONTENTS
 - [Common data types and API](#C_Common_data_types_and_API)
 - [Binary sensors API](#C_Binary_sensors_API)
 - [Sensors API](#C_Sensors_API)
 - [Switches API](#C_Switches_API)
## <a name="C_Common_data_types_and_API"></a>Common data types and API
### ha_mqtt_base_sensor_cfg_t
```c
typedef struct ha_mqtt_base_sensor_cfg {
  const char *state_topic;
  const char *state_pubf;
  const char *attribs_topic;
  const char *attribs_pubf;
} ha_mqtt_base_sensor_cfg_t;
```
Configuration parameters for generic MQTT sensors.

|Field||
|--|--|
|state_topic|MQTT topic for publishing state. Allowed session variables: `${object_id}`, `${device_id}`. If `NULL`, the [hass.mqtt.publish.state_topic](#hass.mqtt.publish.state_topic) config is used.|
|state_pubf|Format string for publishing state. It uses `json_vasprintf()`-like [format](https://github.com/cesanta/frozen/blob/master/README.md#json_asprintf-json_vasprintf). Allowed session variables: `${state.attribs}`. It can be `NULL` if the state is a string value.|
|attribs_topic|MQTT topic for publishing attributes. Allowed session variables: `${object_id}`, `${device_id}`. If `NULL`, the [hass.mqtt.publish.attribs_topic](#hass.mqtt.publish.attribs_topic) config is used.|
|attribs_pubf|(Optional) Format string for publishing attributes json-payload. Only `%Q` format ia allowed (e.g., `'{ state: { attributes:%Q' } }`). If `NULL`, the [hass.mqtt.publish.attribs_pubf](#hass.mqtt.publish.attribs_pubf) config is used.|

## <a name="C_Binary_sensors_API"></a>Binary sensors API
### mgos_hass_bsensor_create()
```c
HA_ENTITY_HANDLE mgos_hass_bsensor_create(ha_entity_cfg_t *entity_cfg,
                                          ha_mqtt_bsensor_cfg_t *mqtt_cfg);
```
Creates a binary sensor and returns its HANDLE. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Entity configuration parameters. See [ha_entity_cfg_t](https://github.com/zendiy-mgos/hass/blob/master/README.md#ha_entity_cfg_t) type for more details.|
|mqtt_cfg|(Optional) MQTT configuration parameters. See [ha_mqtt_bsensor_cfg_t](#ha_mqtt_bsensor_cfg_t) type for more details.|

**Example** - Create a binary sensor that publishes its state when the MQTT connection is established and every 2 seconds, ignoring the `hass.publish.interval` [config](https://github.com/zendiy-mgos/hass/blob/master/README.md#hass.publish.interval) defined in the `mos.yml` file. The system uptime is published as entity's attribute as well.
```c
/* state-get handler for reading binary sensor state */
bool my_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_BSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;
  /* Read binary sensor state here and set <sensor_state> variable below
     with the proper value instead of UNKNOWN */
  enum ha_toggle_state sensor_state = UNKNOWN;
  
  return mgos_hass_entity_bstate_setf(entity_state, sensor_state,
    "{ sys_uptime:%f }", mgos_uptime());
}

/* Set configuration parameters */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
ha_mqtt_bsensor_cfg_t mqtt_cfg = MK_HA_MQTT_BSENSOR_CFG();
mqtt_cfg.pub_cfg.timer_timeout = 2000; //milliseconds

/* Create the binary sensor */
HA_ENTITY_HANDLE h = mgos_hass_bsensor_create(&entity_cfg, &mqtt_cfg);
if (h != NULL) {
  /* Register the state-get handler */
  mgos_hass_bsensor_on_state_get(h, my_on_state_get, NULL);
}
```
### ha_mqtt_bsensor_cfg_t
```c
typedef struct ha_mqtt_bsensor_cfg {
  ha_base_pub_cfg_t pub_cfg;
  ha_bsensor_cfg_t bsensor_cfg;
  ha_mqtt_base_sensor_cfg_t sens_cfg;
} ha_mqtt_bsensor_cfg_t;
```
Configuration parameters for creating binary sensors (see the [mgos_hass_bsensor_create](#mgos_hass_bsensor_create) API).

|Field||
|--|--|
|pub_cfg|Basic configuration parameters for publishing state. See [ha_base_pub_cfg_t](https://github.com/zendiy-mgos/hass/blob/master/README.md#ha_base_pub_cfg_t) type for more details.|
|bsensor_cfg|Binary sensors configuration parameters. See [ha_bsensor_cfg_t](https://github.com/zendiy-mgos/hass/blob/master/README.md#ha_bsensor_cfg_t) type for more details.|
|sens_cfg|Configuration parameters for generic MQTT sensors. See [ha_mqtt_base_sensor_cfg_t](#ha_mqtt_base_sensor_cfg_t) type for more details.|

## <a name="C_Sensors_API"></a>Sensors API
### mgos_hass_sensor_create()
```c
HA_ENTITY_HANDLE mgos_hass_sensor_create(ha_entity_cfg_t *entity_cfg,
                                         ha_mqtt_sensor_cfg_t *mqtt_cfg);
```
Creates a sensor and returns its HANDLE. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Entity configuration parameters. See [ha_entity_cfg_t](https://github.com/zendiy-mgos/hass/blob/master/README.md#ha_entity_cfg_t) type for more details.|
|mqtt_cfg|MQTT configuration parameters. See [ha_mqtt_sensor_cfg_t](#ha_mqtt_sensor_cfg_t) type for more details.|

**Example** - Create a sensor that publishes its state when the MQTT connection is established and every 2 seconds, ignoring the `hass.publish.interval` [config](https://github.com/zendiy-mgos/hass/blob/master/README.md#hass.publish.interval) defined in the `mos.yml` file. The system uptime is published as entity's attribute as well.
```c
/* state-get handler for reading sensor value */
bool my_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_XSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;
  /* Read sensor here and set <sensor_value> variable below
     with the proper value instead of 0.0 */
  double sensor_value = 0.0;
  
  return mgos_hass_entity_fstate_setf(entity_state, sensor_value,
    "{ sys_uptime:%f }", mgos_uptime());
}

/* Set configuration parameters */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
ha_mqtt_sensor_cfg_t mqtt_cfg = MK_HA_MQTT_SENSOR_CFG();
mqtt_cfg.sens_cfg.state_pubf = "%.1f";
mqtt_cfg.pub_cfg.timer_timeout = 2000; //milliseconds

/* Create the sensor */
HA_ENTITY_HANDLE h = mgos_hass_sensor_create(&entity_cfg, &mqtt_cfg);
if (h != NULL) {
  /* Register the state-get handler */
  mgos_hass_sensor_on_state_get(h, my_on_state_get, NULL);
}
```
## <a name="C_Switches_API"></a>Switches API
### mgos_hass_switch_create()
```c
HA_ENTITY_HANDLE mgos_hass_switch_create(ha_entity_cfg_t *entity_cfg,
                                         ha_mqtt_switch_cfg_t *mqtt_cfg);
```
Creates a switch and returns its HANDLE. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Entity configuration parameters. See [ha_entity_cfg_t](https://github.com/zendiy-mgos/hass/blob/master/README.md#ha_entity_cfg_t) type for more details.|
|mqtt_cfg|(Optional) MQTT configuration parameters. See [ha_mqtt_switch_cfg_t](#ha_mqtt_switch_cfg_t) type for more details.|

**Example** - Create a switch that publishes its state when the MQTT connection is established, turns on the built-in LED when the `'ON'` command is received, and turns it off automatically after 5 seconds. The system uptime is published as entity's attribute as well. 
```c
/* state-get handler for reading switch state */
bool my_on_state_get(HA_ENTITY_HANDLE handle,
                     HA_ENTITY_BSTATE entity_state,
                     void *user_data) {
  (void) handle;
  (void) user_data;
  
  enum ha_toggle_state switch_state;
  bool gpio_value = mgos_gpio_read(mgos_sys_config_get_board_led1_pin());
  if (mgos_sys_config_get_board_led1_active_high()) {
    switch_state = (gpio_value ? ON : OFF);
  } else {
    switch_state = (gpio_value ? OFF : ON);
  }
  
  return mgos_hass_entity_bstate_setf(entity_state, switch_state,
    "{ sys_uptime:%f }", mgos_uptime());
}

/* state-set handler for changing switch state */
bool my_on_state_set(HA_ENTITY_HANDLE handle, 
                     enum ha_toggle_state state,
                     void *user_data) {
  (void) handle;
  (void) user_data;

  if (state != UNKNOWN) {
    bool led_ah = mgos_sys_config_get_board_led1_active_high();
    mgos_gpio_write(mgos_sys_config_get_board_led1_pin(),
      (state == ON ? led_ah : !led_ah));
  }
  return (state != UNKNOWN);
}

/* Set configuration parameters */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_switch");
ha_mqtt_switch_cfg_t mqtt_cfg = MK_HA_MQTT_SWITCH_CFG();
mqtt_cfg.switch_cfg.inching_timeout = 5000; // (milliseconds) turn off after 5 secs.

/* Create the switch */ 
HA_ENTITY_HANDLE h = mgos_hass_switch_create(&entity_cfg, &mqtt_cfg);
if (h != NULL) {  
  /* Register the state-get handler */
  mgos_hass_switch_on_state_get(h, my_on_state_get, NULL);
  /* Register the state-set handler */
  mgos_hass_switch_on_state_set(h, my_on_state_set, NULL);
}
```
# JS API
TABLE OF CONTENTS
 - [Binary sensors API](#JS_Binary_sensors_API)
 - [Sensors API](#JS_Sensors_API)
 - [Switches API](#JS_Switches_API)
## <a name="JS_Binary_sensors_API"></a>Binary sensors API
### Hass.BSENSOR.create()
```js
Hass.BSENSOR.create(entity_cfg, mqtt_cfg);
```
Creates a binary sensor object. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Configuration parameters.|
|mqtt_cfg|(Optional) MQTT configuration parameters.|

**Example** - Create a binary sensor that publishes its state when the MQTT connection is established and every 2 seconds, ignoring the `hass.publish.interval` [config](https://github.com/zendiy-mgos/hass/blob/master/README.md#hass.publish.interval) defined in the `mos.yml` file. The system uptime is published as entity's attribute as well.
```js
/* Set configuration parameters */
let entity_cfg = { object_id: "my_first_sensor" };
let mqtt_cfg = { pub_cfg: { timer_timeout: 2000 } }; //milliseconds

/* Create the binary sensor */
let s = Hass.BSENSOR.create(entity_cfg, mqtt_cfg);
if (s) {
  /* Register the state-get handler */
  s.onStateGet(function(handle, entity_state, userdata) {
    /* Read binary sensor state here and set <sensor_state> variable below
       with the proper value instead of Hass.toggleState.UNKNOWN */
    let sensor_state = Hass.toggleState.UNKNOWN;
    
    return Hass.entityToggleStateSet(entity_state, sensor_state,
      JSON.stringify({ sys_uptime: Sys.uptime() }));
  }, null);
}
```
## <a name="JS_Sensors_API"></a>Sensors API
### Hass.SENSOR.create()
```js
Hass.SENSOR.create(entity_cfg, mqtt_cfg);
```
Creates a sensor object. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Configuration parameters.|
|mqtt_cfg|MQTT configuration parameters.|

**Example** - Create a sensor that publishes its state when the MQTT connection is established and every 2 seconds, ignoring the `hass.publish.interval` [config](https://github.com/zendiy-mgos/hass/blob/master/README.md#hass.publish.interval) defined in the `mos.yml` file. The system uptime is published as entity's attribute as well.
```js
/* Set configuration parameters */
let entity_cfg = { object_id: "my_first_sensor" };
let mqtt_cfg = {
  sens_cfg: { state_pubf: "%.1f" },
  pub_cfg: { timer_timeout: 2000 } //milliseconds
};

/* Create the sensor */
let s = Hass.SENSOR.create(entity_cfg, mqtt_cfg);
if (s) {
  /* Register the state-get handler */
  s.onStateGet(function(handle, entity_state, userdata) {
    /* Read sensor here and set <sensor_value> variable below
       with the proper value instead of 0.0 */
    let sensor_value = 0.0;
    
    return Hass.entityXStateSet(entity_state, sensor_value,
      JSON.stringify({ sys_uptime: Sys.uptime() }));
  }, null);
}
```
## <a name="JS_Switches_API"></a>Switches API
### Hass.SWITCH.create()
```js
Hass.SWITCH.create(entity_cfg, mqtt_cfg);
```
Creates a switch object. Returns `NULL` in case of error.

|Parameter||
|--|--|
|entity_cfg|Configuration parameters.|
|mqtt_cfg|(Optional) MQTT configuration parameters.|

**Example** - Create a switch that publishes its state when the MQTT connection is established, turns on the built-in LED when the `'ON'` command is received, and turns it off automatically after 5 seconds. The system uptime is published as entity's attribute as well. 
```js
/* Set configuration parameters */
let entity_cfg = { object_id: "my_first_switch" };
let mqtt_cfg = {
  switch_cfg: { inching_timeout: 5000 } // (milliseconds) turn off after 5 secs
};

/* Create the switch */
let s = Hass.SWITCH.create(entity_cfg, mqtt_cfg);
if (s) {
  /* Register the state-get handler */
  s.onStateGet(function(handle, entity_state, userdata) {
    let switch_state;
    let gpio_value = GPIO.read(Cfg.get('board.led1.pin'));
    if (Cfg.get('board.led1.active_high')) {
      switch_state = (gpio_value ? Hass.toggleState.ON : Hass.toggleState.OFF);
    } else {
      switch_state = (gpio_value ? Hass.toggleState.OFF : Hass.toggleState.ON);
    }
    
    return Hass.entityToggleStateSet(entity_state, switch_state,
      JSON.stringify({ sys_uptime: Sys.uptime() }));
  }, null);
  /* Register the state-set handler */
  s.onStateSet(function(handle, state, userdata) {
    if (state !== Hass.toggleState.UNKNOWN) {
      let led_ah = Cfg.get('board.led1.active_high');
      GPIO.write(Cfg.get('board.led1.pin'),
        (state === Hass.toggleState.ON ? led_ah : !led_ah));
    }
    return (state !== Hass.toggleState.UNKNOWN);
  }, null);
}
```
