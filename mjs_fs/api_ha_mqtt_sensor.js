load ("api_sys.js");

let ha_mqtt_sensor = {
  _crt: ffi('void *mgos_hass_sensor_create(void *, void *)'),

  _ccrt: ffi('void *mjs_ha_mqtt_sensor_cfg_mk()'),
  _cs1: ffi('void mjs_ha_mqtt_sensor_cfg_set1(void *, bool, int, int)'),
  _cs2: ffi('void mjs_ha_mqtt_sensor_cfg_set2(void *, void *, void *, void *, void *)'),

  _create: function(ecfg, cfg) {
    let that = ha_mqtt_sensor;
    let obj = Object.create(Hass.SENSOR._proto);

    // create entity cfg struct
    let mec = {
      object_id: Sys.strdup(ecfg.object_id),
      device_id: Sys.strdup(ecfg.device_id)
    };
    let cecfg = Hass.mkEntityCfg(mec.object_id, mec.device_id);

    // platform-specific cfg struct
    let ccfg = that._ccrt();

    /* Set CS1 config settings */
    let dpe = Hass.pubEvent.DEFAULT;
    that._cs1(ccfg,
      (cfg.pub_cfg ? (cfg.pub_cfg.lazy_pub || true) : true),
      (cfg.pub_cfg ? (cfg.pub_cfg.pub_on || dpe) : dpe),
      (cfg.pub_cfg ? (cfg.pub_cfg.timer_timeout || 0) : 0));

    /* Set CS2 config settings */
    let cs2 = {
      p1: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.attribs_pubf) : null),
      p2: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.attribs_topic) : null),
      p3: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.state_pubf) : null),
      p4: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.state_topic) : null),
    };
    that._cs2(ccfg, cs2.p1, cs2.p2, cs2.p3, cs2.p4);

    /* Create the sensor */
    obj.handle = that._crt(cecfg, ccfg);

    // free duplicated strings
    Sys.free(mec.object_id);
    Sys.free(mec.device_id);
    Sys.free(cs2.p1);
    Sys.free(cs2.p2);
    Sys.free(cs2.p3);
    Sys.free(cs2.p4);

    // free allocated cfg structs
    Sys.free(cecfg);
    Sys.free(ccfg);

    return obj;
  },
};

Hass.SENSOR.create = ha_mqtt_sensor._create;