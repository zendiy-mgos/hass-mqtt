# Home Assistant MQTT Devices Mongoose OS library
## Overview
The [Mongoose-OS](https://mongoose-os.com/) library for making [Home Assistant](https://www.home-assistant.io/) compatible MQTT devices. Following Home Assistant MQTT entities are currently supported:
 - Binary sensors
 - Sensors
 - Switches
## GET STARTED
Build up your device in few minutes just starting from one of the following samples.

|Sample|Notes|
|--|--|
|[hass-mqtt-demo](https://github.com/zendiy-mgos/hass-mqtt-demo)|Shows you how to build firmware for managing sensors, binary sensors, switches, etc. integrated in an all-in-one device.|
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
