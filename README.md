Home Assistant MQTT Devices Mongoose OS library
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
The following configuration section is added to the `conf0.json` file in your mongoose-os project.
```json
"hass": {
  "mqtt": {
    "publish": {
      "state_topic": "",
      "attribs_topic": "",
      "attribs_pubf": "",
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
      "state_pubf": ""
    }
  }
}
```
|Property|Type|Default|Description|
|--|--|--|--|
|hass.mqtt.publish.**state_topic**|string||Default topic for publishing state|
|hass.mqtt.publish.**attribs_topic**|string||Default topic for publishing attributes|
|hass.mqtt.publish.**attribs_pubf**|string||The format string for publishig attributes json payload|
|hass.mqtt.publish.**retain**|boolean|`true`|If the published message should have the retain flag on or not|
|hass.mqtt.publish.**qos**|integer|`0`|The maximum QoS level|
|hass.mqtt.command.**topic**|string||Default topic for subscribing to command|
|hass.mqtt.command.**parsef**|string||The format string for parsing the command json payload|
|hass.mqtt.availability.**enable_birth_msg**|boolean|`true`|Enable/disable the MQTT birth message|
|hass.mqtt.availability.**state_on**|string|`'online'`|The value that represents the available state|
|hass.mqtt.availability.**state_off**|string|`'offline'`|The value that represents the unavailable state|
|hass.mqtt.availability.**state_pubf**|string||The format string for publishig the availability json payload|
# C/C++ API reference
## Binary sensors API
### mgos_hass_bsensor_create()
```c
HA_ENTITY_HANDLE mgos_hass_bsensor_create(ha_entity_cfg_t *entity_cfg,
                                          ha_mqtt_bsensor_cfg_t *mqtt_cfg);
```
**Creates a binary sensor handle providing entity configuration and, optionally, the MQTT configuration. Returns `NULL` in case of error.**

Example 1 - create a binary sensor that publishes its state according the `hass.publish.interval` config defined in the `mos.yml` file. If it is set to `0`(zero), the sensor publishes its state only when the MQTT connection is established.
```c
/* Create and initialze Home Assistant entity binary_sensor.my_first_sensor */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
HA_ENTITY_HANDLE h = mgos_hass_bsensor_create(&entity_cfg, NULL);
```
Example 2 - create a binary sensor that publishes its state when the MQTT connection is established and periodically every 2 seconds ignoring the `hass.publish.interval` config defined in the `mos.yml` file. 
```c
/* Create and initialze Home Assistant entity binary_sensor.my_first_sensor */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
ha_mqtt_bsensor_cfg_t mqtt_cfg = MK_HA_MQTT_BSENSOR_CFG();
mqtt_cfg.pub_cfg.timer_timeout = 2000; //milliseconds
HA_ENTITY_HANDLE h = mgos_hass_bsensor_create(&entity_cfg, &mqtt_cfg);
```
## Sensors API
### mgos_hass_sensor_create()
```c
HA_ENTITY_HANDLE mgos_hass_sensor_create(ha_entity_cfg_t *entity_cfg,
                                         ha_mqtt_sensor_cfg_t *mqtt_cfg);
```
**Creates a sensor handle providing entity configuration and, optionally, the MQTT configuration. Returns `NULL` in case of error.**

Example 1 - create a sensor that publishes its state according the `hass.publish.interval` config defined in the `mos.yml` file. If it is set to `0`(zero), the sensor publishes its state only when the MQTT connection is established.
```c
/* Create and initialze Home Assistant entity sensor.my_first_sensor */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
HA_ENTITY_HANDLE h = mgos_hass_sensor_create(&entity_cfg, NULL);
```
Example 2 - create a sensor that publishes its state when the MQTT connection is established and periodically every 2 seconds ignoring the `hass.publish.interval` config defined in the `mos.yml` file. 
```c
/* Create and initialze Home Assistant entity sensor.my_first_sensor */
ha_entity_cfg_t entity_cfg = HA_ENTITY_CFG("my_first_sensor");
ha_mqtt_sensor_cfg_t mqtt_cfg = MK_HA_MQTT_SENSOR_CFG();
mqtt_cfg.pub_cfg.timer_timeout = 2000; //milliseconds
HA_ENTITY_HANDLE h = mgos_hass_sensor_create(&entity_cfg, &mqtt_cfg);
```
# JS API
## Binary sensors API
### Hass.BSENSOR.create()
```js
Hass.BSENSOR.create(entity_cfg, mqtt_cfg);
```
**Creates a binary sensor instance providing entity configuration and, optionally, the MQTT configuration. Returns `null` in case of error.**

Example 1 - create a binary sensor that publishes its state according the `hass.publish.interval` config defined in the `mos.yml` file. If it is set to `0`(zero), the sensor publishes its state only when the MQTT connection is established.
```js
/* Create and initialze Home Assistant entity binary_sensor.my_first_sensor */
let entity_cfg = { object_id: "my_first_sensor" };
let s = Hass.BSENSOR.create(entity_cfg);
```
Example 2 - create a binary sensor that publishes its state when the MQTT connection is established and periodically every 2 seconds ignoring the `hass.publish.interval` config defined in the `mos.yml` file. 
```js
/* Create and initialze Home Assistant entity binary_sensor.my_first_sensor */
let entity_cfg = { object_id: "my_first_sensor" };
let mqtt_cfg = { pub_cfg: { timer_timeout: 2000 } }; //milliseconds
let s = Hass.BSENSOR.create(entity_cfg, mqtt_cfg);
```
## Sensors API
### Hass.SENSOR.create()
```js
Hass.SENSOR.create(entity_cfg, mqtt_cfg);
```
**Creates a sensor instance providing entity configuration and, optionally, the MQTT configuration. Returns `null` in case of error.**

Example 1 - create a sensor that publishes its state according the `hass.publish.interval` config defined in the `mos.yml` file. If it is set to `0`(zero), the sensor publishes its state only when the MQTT connection is established.
```js
/* Create and initialze Home Assistant entity sensor.my_first_sensor */
let entity_cfg = { object_id: "my_first_sensor" };
let s = Hass.SENSOR.create(entity_cfg);
```
Example 2 - create a sensor that publishes its state when the MQTT connection is established and periodically every 2 seconds ignoring the `hass.publish.interval` config defined in the `mos.yml` file. 
```js
/* Create and initialze Home Assistant entity sensor.my_first_sensor */
let entity_cfg = { object_id: "my_first_sensor" };
let mqtt_cfg = { pub_cfg: { timer_timeout: 2000 } }; //milliseconds
let s = Hass.SENSOR.create(entity_cfg, mqtt_cfg);
```
