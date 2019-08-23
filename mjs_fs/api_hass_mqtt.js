load('api_hass.js');

let ha_mqtt = {

};

Hass.MQTT = ha_mqtt;

load('api_ha_mqtt_bsensor.js');
load('api_ha_mqtt_sensor.js');
load('api_ha_mqtt_switch.js');