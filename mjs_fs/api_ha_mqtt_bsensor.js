load ("api_sys.js");

let ha_mqtt_bsensor = {
  _crt: ffi('void *mgos_hass_bsensor_create(void *, void *)'),

  _ccrt: ffi('void *mjs_ha_mqtt_bsensor_cfg_mk()'),
  _cs1: ffi('void mjs_ha_mqtt_bsensor_cfg_set1(void *, bool, int, int)'),
  _cs2: ffi('void mjs_ha_mqtt_bsensor_cfg_set2(void *, void *, void *, void *, void *)'),
  _cs3: ffi('void mjs_ha_mqtt_bsensor_cfg_set3(void *, void *, void *)'),

  _create: function(ecfg, cfg) {
    let that = ha_mqtt_bsensor;
    let obj = Object.create(Hass.BSENSOR._proto);
    
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
      (cfg.pub_cfg ? (cfg.pub_cfg.timer_timeout || 0) : 0)
    );

    /* Set CS2 config settings */
    let cs2 = {
      p1: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.attribs_pubf) : null),
      p2: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.attribs_topic) : null),
      p3: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.state_pubf) : null),
      p4: (cfg.sens_cfg ? Sys.strdup(cfg.sens_cfg.state_topic) : null),
    };
    that._cs2(ccfg, cs2.p1, cs2.p2, cs2.p3, cs2.p4);

    /* Set CS3 config settings */
    let state_cfg = (cfg.switch_cfg ? cfg.bsensor_cfg.state_cfg : {});
    let cs3 = {
      p1: (state_cfg ? Sys.strdup(state_cfg.on) : null),
      p2: (state_cfg ? Sys.strdup(state_cfg.off) : null)
    };
    that._cs3(ccfg, cs3.p1, cs3.p2);

    /* Create the binary sensor */
    obj.handle = that._crt(cecfg, ccfg);
    
    // free duplicated strings
    Sys.free(mec.object_id);
    Sys.free(mec.device_id);
    Sys.free(cs2.p1);
    Sys.free(cs2.p2);
    Sys.free(cs2.p3);
    Sys.free(cs2.p4);
    Sys.free(cs3.p1);
    Sys.free(cs3.p2);
    
    // free allocated cfg structs
    Sys.free(cecfg);
    Sys.free(ccfg);

    return obj;
  },
};

Hass.BSENSOR.create = ha_mqtt_bsensor._create;