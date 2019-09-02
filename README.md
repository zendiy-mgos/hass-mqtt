# Home Assistant MQTT Devices Mongoose OS library
## Overview
The [Mongoose-OS](https://mongoose-os.com/) library for making [Home Assistant](https://www.home-assistant.io/) compatible MQTT devices. Following Home Assistant MQTT entities are currently supported:
 - BINARY SENSORS
 - SENSORS
 - SWITCHES
## Usage
Include the library into your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
## Configuration
The following configuration section is added to the `conf0.json` file in your mongoose-os project.

    "hass": {
	  "mqtt": {
	    "publish": {
	      "state_topic": "",
	      "attribs_topic": "",
	      "attribs_pubf": "",
	      "retain": true,
	      "qos": 0
	    {,
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
## GET STARTED
Build up you device in few minutes following samples below. Mind that your device, like ESP8266 or ESP32, can host more than one of supported Home Assistant MQTT entity simultaneously.
### Make a BINARY SENSOR
#### 1. Configure your Home Assistant
Add following configuration lines to the `configuration.yaml` file.

    binary_sensor:
      - platform: mqtt
        name: "My first sensor"
        state_topic: "hass/things/MYDEVICE_01/my_first_sensor/state"
        availability_topic: "hass/things/MYDEVICE_01/availability"
        json_attributes_topic: "hass/things/MYDEVICE_01/my_first_sensor/attributes"
        device_class: door
#### 2. Implement your Mongoose-OS firmware
Include the library in your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
Set device ID and MQTT settings in your `mos.yml` file.

    config_schema:
      - ["device.id", "MYDEVICE_01"]
      - ["mqtt.server", "<your_hass_ip_address>:8883"]
      - ["mqtt.user", "<your_hass_mqtt_broker_username>"]
      - ["mqtt.pass", "your_hass_mqtt_broker_password"]
      - ["mqtt.keep_alive", 15]
      - ["mqtt.will_topic", "hass/things/${device_id}/availability"]
      - ["mqtt.will_message", "offline"]
Set library settings in your `mos.yml` file.

    config_schema:
      - ["hass.mqtt.publish.attribs_topic", "hass/things/${device_id}/${object_id}/attributes"]
      - ["hass.mqtt.publish.state_topic", "hass/things/${device_id}/${object_id}/state"]
      - ["hass.mqtt.command.topic", "hass/things/${device_id}/${object_id}/cmd"]
        # Publish binary-sensor value every 10 seconds
      - ["hass.publish.interval", 10000]
**C/C++ CODE**: write the `main.c` file if you are using C/C++ language for implementing your firmware.

	#include  <stdbool.h> 
	#include  "mgos.h"
	#include  "mgos_hass_mqtt.h"
	
	enum ha_toggle_state sensor_state_read() {       
	  /* Read binary-sensor here and return ON,
	     OFF or UNKNOWN according sensor's readings */
	  
	  return UNKNOWN;
	}
	
	bool on_state_get(HA_ENTITY_HANDLE handle,
	                  HA_ENTITY_BSTATE entity_state,
	                  void *user_data) {
	  (void) handle;
	  (void) user_data;

	  /* Read binary-sensor value */
	  enum ha_toggle_state state = sensor_state_read();
	
	  return mgos_hass_entity_bstate_set(entity_state, state, NULL);
	}
	
	enum mgos_app_init_result mgos_app_init(void) {
	  /* Set binary-sensor configurations */
	  ha_entity_cfg_t e = HA_ENTITY_CFG("my_first_sensor");   
	  ha_mqtt_bsensor_cfg_t cfg = MK_HA_MQTT_BSENSOR_CFG();	  
	  
	  /* Create and initialze 'binary_sensor.my_first_sensor' */ 
	  HA_ENTITY_HANDLE h = mgos_hass_bsensor_create(&e, &cfg);
	  if (h == NULL) return MGOS_APP_INIT_ERROR;	  
	  
	  mgos_hass_bsensor_on_state_get(h, on_state_get, NULL);
	  
	  return MGOS_APP_INIT_SUCCESS;
	}
**JavaScript CODE**: write the `init.js` file if you are implementing a JavaScript firmware.

	load('api_hass_mqtt.js');
	
	function sensorStateRead() {
	  /* Read binary-sensor here and return Hass.toggleState.ON,
	     Hass.toggleState.OFF or Hass.toggleState.UNKNOWN
	     according sensor's readings */
	     
	  return Hass.toggleState.UNKNOWN;
	}
	
	/* Set binary-sensor configurations */
	let e = { object_id: "my_first_sensor" };
	
	/* Create and initialze 'binary_sensor.my_first_sensor' */
	let s = Hass.BSENSOR.create(e);
	if (s) {
	  s.onStateGet(function(handle, entity_state, userdata) {
	    let state = sensorStateRead();
	    return Hass.entityToggleStateSet(entity_state, state);
	  }, null);
	}
### Make a SENSOR
#### 1. Configure your Home Assistant
Add following configuration lines to the `configuration.yaml` file.

    sensor:
      - platform: mqtt
        name: "My first sensor"
        state_topic: "hass/things/MYDEVICE_01/my_first_sensor/state"
        availability_topic: "hass/things/MYDEVICE_01/availability"
        json_attributes_topic: "hass/things/MYDEVICE_01/my_first_sensor/attributes"
        unit_of_measurement: "%"
#### 2. Implement your Mongoose-OS firmware
Include the library in your `mos.yml` file.

    libs:
      - origin: https://github.com/zendiy-mgos/hass-mqtt
Set device ID and MQTT settings in your `mos.yml` file.

    config_schema:
      - ["device.id", "MYDEVICE_01"]
      - ["mqtt.server", "<your_hass_ip_address>:8883"]
      - ["mqtt.user", "<your_hass_mqtt_broker_username>"]
      - ["mqtt.pass", "your_hass_mqtt_broker_password"]
      - ["mqtt.keep_alive", 15]
      - ["mqtt.will_topic", "hass/things/${device_id}/availability"]
      - ["mqtt.will_message", "offline"]
Set library settings in your `mos.yml` file.

    config_schema:
      - ["hass.mqtt.publish.attribs_topic", "hass/things/${device_id}/${object_id}/attributes"]
      - ["hass.mqtt.publish.state_topic", "hass/things/${device_id}/${object_id}/state"]
      - ["hass.mqtt.command.topic", "hass/things/${device_id}/${object_id}/cmd"]
        # Publish sensor value every 10 seconds
      - ["hass.publish.interval", 10000]
**C/C++ CODE**: write the `main.c` file if you are using C/C++ language for implementing your firmware.

	#include  <stdbool.h> 
	#include  "mgos.h"
	#include  "mgos_hass_mqtt.h"
	
	double sensor_state_read() {       
	  /* Read sensor here and return measured value */
	  return 0.0;
	}
	
	bool on_state_get(HA_ENTITY_HANDLE handle,
	                  HA_ENTITY_XSTATE entity_state,
	                  void *user_data) {
	  (void) handle;
	  (void) user_data;

	  /* Read sensor value */
	  double state = sensor_state_read();
	
	  return mgos_hass_entity_fstate_set(entity_state, state, NULL);
	}
	
	enum mgos_app_init_result mgos_app_init(void) {
	  /* Set binary-sensor configurations */
	  ha_entity_cfg_t e = HA_ENTITY_CFG("my_first_sensor");   
	  ha_mqtt_sensor_cfg_t cfg = MK_HA_MQTT_SENSOR_CFG();	  
	  
	  /* Create and initialze 'binary_sensor.my_first_sensor' */ 
	  HA_ENTITY_HANDLE h = mgos_hass_sensor_create(&e, &cfg);
	  if (h == NULL) return MGOS_APP_INIT_ERROR;	  
	  
	  mgos_hass_sensor_on_state_get(h, on_state_get, NULL);
	  
	  return MGOS_APP_INIT_SUCCESS;
	}
**JavaScript CODE**: write the `init.js` file if you are implementing a JavaScript firmware.

	load('api_hass_mqtt.js');
	
	function sensorStateRead() {
	  /* Read sensor here and return measured value */
	  return 0.0;
	}
	
	/* Set binary-sensor configurations */
	let e = { object_id: "my_first_sensor" };
	
	/* Create and initialze 'binary_sensor.my_first_sensor' */
	let s = Hass.SENSOR.create(e);
	if (s) {
	  s.onStateGet(function(handle, entity_state, userdata) {
	    let state = sensorStateRead();
	    return Hass.entityXStateSet(entity_state, state);
	  }, null);
	}
