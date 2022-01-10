#include "nrf_ble_utils.h"
#include "Uart.h"

struct {
    uint32_t attr_table_size;
    uint8_t  service_changed;
    uint8_t  uuid128_max;

    // Bandwidth configuration
    struct {
        uint16_t  mtu_max;
        uint16_t  event_len;
        uint8_t   hvn_qsize;
        uint8_t   wrcmd_qsize;
    }prph, central;
}_sd_cfg;


/*------------------------------------------------------------------*/
/* Error String
 *------------------------------------------------------------------*/
lookup_entry_t const _strerr_lookup[] =
{
    // General: 0x0000
    { .key = NRF_ERROR_SVC_HANDLER_MISSING                     , .data = "NRF_ERROR_SVC_HANDLER_MISSING"                     },
    { .key = NRF_ERROR_SOFTDEVICE_NOT_ENABLED                  , .data = "NRF_ERROR_SOFTDEVICE_NOT_ENABLED"                  },
    { .key = NRF_ERROR_INTERNAL                                , .data = "NRF_ERROR_INTERNAL"                                },
    { .key = NRF_ERROR_NO_MEM                                  , .data = "NRF_ERROR_NO_MEM"                                  },
    { .key = NRF_ERROR_NOT_FOUND                               , .data = "NRF_ERROR_NOT_FOUND"                               },
    { .key = NRF_ERROR_NOT_SUPPORTED                           , .data = "NRF_ERROR_NOT_SUPPORTED"                           },
    { .key = NRF_ERROR_INVALID_PARAM                           , .data = "NRF_ERROR_INVALID_PARAM"                           },
    { .key = NRF_ERROR_INVALID_STATE                           , .data = "NRF_ERROR_INVALID_STATE"                           },
    { .key = NRF_ERROR_INVALID_LENGTH                          , .data = "NRF_ERROR_INVALID_LENGTH"                          },
    { .key = NRF_ERROR_INVALID_FLAGS                           , .data = "NRF_ERROR_INVALID_FLAGS"                           },
    { .key = NRF_ERROR_INVALID_DATA                            , .data = "NRF_ERROR_INVALID_DATA"                            },
    { .key = NRF_ERROR_DATA_SIZE                               , .data = "NRF_ERROR_DATA_SIZE"                               },
    { .key = NRF_ERROR_TIMEOUT                                 , .data = "NRF_ERROR_TIMEOUT"                                 },
    { .key = NRF_ERROR_NULL                                    , .data = "NRF_ERROR_NULL"                                    },
    { .key = NRF_ERROR_FORBIDDEN                               , .data = "NRF_ERROR_FORBIDDEN"                               },
    { .key = NRF_ERROR_INVALID_ADDR                            , .data = "NRF_ERROR_INVALID_ADDR"                            },
    { .key = NRF_ERROR_BUSY                                    , .data = "NRF_ERROR_BUSY"                                    },
    { .key = NRF_ERROR_CONN_COUNT                              , .data = "NRF_ERROR_CONN_COUNT"                              },
    { .key = NRF_ERROR_RESOURCES                               , .data = "NRF_ERROR_RESOURCES"                               },

    // SDM: 0x1000
    { .key = NRF_ERROR_SDM_LFCLK_SOURCE_UNKNOWN                , .data = "NRF_ERROR_SDM_LFCLK_SOURCE_UNKNOWN"                },
    { .key = NRF_ERROR_SDM_INCORRECT_INTERRUPT_CONFIGURATION   , .data = "NRF_ERROR_SDM_INCORRECT_INTERRUPT_CONFIGURATION"   },
    { .key = NRF_ERROR_SDM_INCORRECT_CLENR0                    , .data = "NRF_ERROR_SDM_INCORRECT_CLENR0"                    },

    // SOC: 0x2000
    { .key = NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN                 , .data = "NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN"                 },
    { .key = NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE        , .data = "NRF_ERROR_SOC_NVIC_INTERRUPT_NOT_AVAILABLE"        },
    { .key = NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED , .data = "NRF_ERROR_SOC_NVIC_INTERRUPT_PRIORITY_NOT_ALLOWED" },
    { .key = NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN              , .data = "NRF_ERROR_SOC_NVIC_SHOULD_NOT_RETURN"              },
    { .key = NRF_ERROR_SOC_POWER_MODE_UNKNOWN                  , .data = "NRF_ERROR_SOC_POWER_MODE_UNKNOWN"                  },
    { .key = NRF_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN         , .data = "NRF_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN"         },
    { .key = NRF_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN         , .data = "NRF_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN"         },
    { .key = NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES              , .data = "NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES"              },
    { .key = NRF_ERROR_SOC_PPI_INVALID_CHANNEL                 , .data = "NRF_ERROR_SOC_PPI_INVALID_CHANNEL"                 },
    { .key = NRF_ERROR_SOC_PPI_INVALID_GROUP                   , .data = "NRF_ERROR_SOC_PPI_INVALID_GROUP"                   },

    // BLE Generic: 0x2000
    { .key = BLE_ERROR_NOT_ENABLED                             , .data = "BLE_ERROR_NOT_ENABLED"                             },
    { .key = BLE_ERROR_INVALID_CONN_HANDLE                     , .data = "BLE_ERROR_INVALID_CONN_HANDLE"                     },
    { .key = BLE_ERROR_INVALID_ATTR_HANDLE                     , .data = "BLE_ERROR_INVALID_ATTR_HANDLE"                     },
    { .key = BLE_ERROR_INVALID_ADV_HANDLE                      , .data = "BLE_ERROR_INVALID_ADV_HANDLE"                      },
    { .key = BLE_ERROR_INVALID_ROLE                            , .data = "BLE_ERROR_INVALID_ROLE"                            },
    { .key = BLE_ERROR_BLOCKED_BY_OTHER_LINKS                  , .data = "BLE_ERROR_BLOCKED_BY_OTHER_LINKS"                  },

    // BLE GAP: 0x2200
    { .key = BLE_ERROR_GAP_UUID_LIST_MISMATCH                  , .data = "BLE_ERROR_GAP_UUID_LIST_MISMATCH"                  },
    { .key = BLE_ERROR_GAP_DISCOVERABLE_WITH_WHITELIST         , .data = "BLE_ERROR_GAP_DISCOVERABLE_WITH_WHITELIST"         },
    { .key = BLE_ERROR_GAP_INVALID_BLE_ADDR                    , .data = "BLE_ERROR_GAP_INVALID_BLE_ADDR"                    },
    { .key = BLE_ERROR_GAP_WHITELIST_IN_USE                    , .data = "BLE_ERROR_GAP_WHITELIST_IN_USE"                    },
    { .key = BLE_ERROR_GAP_DEVICE_IDENTITIES_IN_USE            , .data = "BLE_ERROR_GAP_DEVICE_IDENTITIES_IN_USE"            },
    { .key = BLE_ERROR_GAP_DEVICE_IDENTITIES_DUPLICATE         , .data = "BLE_ERROR_GAP_DEVICE_IDENTITIES_DUPLICATE"         },

    // BLE GATTC: 0x2300
    { .key = BLE_ERROR_GATTC_PROC_NOT_PERMITTED                , .data = "BLE_ERROR_GATTC_PROC_NOT_PERMITTED"                },

    // BLE GATTS: 0x2400
    { .key = BLE_ERROR_GATTS_INVALID_ATTR_TYPE                 , .data = "BLE_ERROR_GATTS_INVALID_ATTR_TYPE"                 },
    { .key = BLE_ERROR_GATTS_SYS_ATTR_MISSING                  , .data = "BLE_ERROR_GATTS_SYS_ATTR_MISSING"                  },
};

lookup_table_t const _strerr_table =
{
  .count = arrcount(_strerr_lookup),
  .items = _strerr_lookup
};

void const * lookup_find(lookup_table_t const* p_table, uint32_t key)
{
    for(uint16_t i=0; i<p_table->count; i++)
    {
        if (p_table->items[i].key == key) return p_table->items[i].data;
    }

    return NULL;
}

const char* dbg_err_str(int32_t err_id)
{
  // TODO: verify it would be safe to change to int16_t
  const char * str = (const char *) lookup_find(&_strerr_table, err_id);
  static char unknown_str[7] = {0};

  if ( str == NULL )
  {
    sprintf(unknown_str, "0x%04lX", (uint32_t)err_id);
    str = unknown_str;
  }

  return str;
}


static void nrf_error_cb(uint32_t id, uint32_t pc, uint32_t info)
{
  Serial.println("NRF Error CB");
}

bool beacon_init(uint16_t manufacturer_id,
                 uint8_t beaconUuid[16],
                 uint16_t major,
                 uint16_t minor,
                 int8_t rssi_at_1m)
{
    varclr(&_sd_cfg);

    _sd_cfg.attr_table_size = CFG_SD_ATTR_TABLE_SIZE;
    _sd_cfg.uuid128_max     = BLE_UUID_VS_COUNT_DEFAULT;
    _sd_cfg.service_changed = 1;

    _sd_cfg.prph.mtu_max     = BLE_GATT_ATT_MTU_DEFAULT;
    _sd_cfg.prph.event_len   = BLE_GAP_EVENT_LENGTH_DEFAULT;
    _sd_cfg.prph.hvn_qsize   = BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT;
    _sd_cfg.prph.wrcmd_qsize = BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT;

    _sd_cfg.central.mtu_max     = BLE_GATT_ATT_MTU_DEFAULT;
    _sd_cfg.central.event_len   = BLE_GAP_EVENT_LENGTH_DEFAULT;
    _sd_cfg.central.hvn_qsize   = BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT;
    _sd_cfg.central.wrcmd_qsize = BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT;


    int _tx_power      = CFG_BLE_TX_POWER_LEVEL;

    // begin!

    // Configure Clock
#if defined( USE_LFXO )
    nrf_clock_lf_cfg_t clock_cfg =
        {
            // LFXO
            .source        = NRF_CLOCK_LF_SRC_XTAL,
            .rc_ctiv       = 0,
            .rc_temp_ctiv  = 0,
            .accuracy      = NRF_CLOCK_LF_ACCURACY_20_PPM
        };
#elif defined( USE_LFRC )
    nrf_clock_lf_cfg_t clock_cfg =
        {
            // LXRC
            .source        = NRF_CLOCK_LF_SRC_RC,
            .rc_ctiv       = 16,
            .rc_temp_ctiv  = 2,
            .accuracy      = NRF_CLOCK_LF_ACCURACY_250_PPM
        };
#else
#error Clock Source is not configured, define USE_LFXO or USE_LFRC according to your board in variant.h
#endif

    // Enable SoftDevice
#ifdef ANT_LICENSE_KEY
    VERIFY_STATUS( sd_softdevice_enable(&clock_cfg, nrf_error_cb, ANT_LICENSE_KEY), false );
#else
    VERIFY_STATUS( sd_softdevice_enable(&clock_cfg, nrf_error_cb), false );
#endif

    NVIC_EnableIRQ(SD_EVT_IRQn);

    /*------------- Configure BLE params  -------------*/
    extern uint32_t  __data_start__[]; // defined in linker
    uint32_t ram_start = (uint32_t) __data_start__;

    ble_cfg_t blecfg;

    // Vendor UUID count
    varclr(&blecfg);
    blecfg.common_cfg.vs_uuid_cfg.vs_uuid_count = _sd_cfg.uuid128_max;
    VERIFY_STATUS ( sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID, &blecfg, ram_start), false );

    // Roles
    varclr(&blecfg);
    blecfg.gap_cfg.role_count_cfg.periph_role_count  = 1;
    blecfg.gap_cfg.role_count_cfg.central_role_count = 0;
    blecfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
    VERIFY_STATUS( sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &blecfg, ram_start), false );

    // Device Name
//  varclr(&blecfg);
//  blecfg.gap_cfg.device_name_cfg =
//  VERIFY_STATUS( sd_ble_cfg_set(BLE_GAP_CFG_DEVICE_NAME, &blecfg, ram_start) );

    varclr(&blecfg);
    blecfg.gatts_cfg.service_changed.service_changed = _sd_cfg.service_changed;
    VERIFY_STATUS ( sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED, &blecfg, ram_start), false );

    // ATTR Table Size
    varclr(&blecfg);
    blecfg.gatts_cfg.attr_tab_size.attr_tab_size = _sd_cfg.attr_table_size;
    VERIFY_STATUS ( sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE, &blecfg, ram_start), false );

    /*------------- Event Length + MTU + HVN queue + WRITE CMD queue setting affecting bandwidth -------------*/
    int _prph_count = 1;
    if ( _prph_count )
    {
        // ATT MTU
        varclr(&blecfg);
        blecfg.conn_cfg.conn_cfg_tag = CONN_CFG_PERIPHERAL;
        blecfg.conn_cfg.params.gatt_conn_cfg.att_mtu = _sd_cfg.prph.mtu_max;
        VERIFY_STATUS ( sd_ble_cfg_set(BLE_CONN_CFG_GATT, &blecfg, ram_start), false );

        // Event length and max connection for this config
        varclr(&blecfg);
        blecfg.conn_cfg.conn_cfg_tag = CONN_CFG_PERIPHERAL;
        blecfg.conn_cfg.params.gap_conn_cfg.conn_count   = _prph_count;
        blecfg.conn_cfg.params.gap_conn_cfg.event_length = _sd_cfg.prph.event_len;
        VERIFY_STATUS ( sd_ble_cfg_set(BLE_CONN_CFG_GAP, &blecfg, ram_start), false );

        // HVN queue size
        varclr(&blecfg);
        blecfg.conn_cfg.conn_cfg_tag = CONN_CFG_PERIPHERAL;
        blecfg.conn_cfg.params.gatts_conn_cfg.hvn_tx_queue_size = _sd_cfg.prph.hvn_qsize;
        VERIFY_STATUS ( sd_ble_cfg_set(BLE_CONN_CFG_GATTS, &blecfg, ram_start), false );

        // WRITE COMMAND queue size
        varclr(&blecfg);
        blecfg.conn_cfg.conn_cfg_tag = CONN_CFG_PERIPHERAL;
        blecfg.conn_cfg.params.gattc_conn_cfg.write_cmd_tx_queue_size = _sd_cfg.prph.wrcmd_qsize;
        VERIFY_STATUS ( sd_ble_cfg_set(BLE_CONN_CFG_GATTC, &blecfg, ram_start), false );
    }

    // Enable BLE stack
    // The memory requirement for a specific configuration will not increase
    // between SoftDevices with the same major version number
    uint32_t err = sd_ble_enable(&ram_start);
    if ( err )
    {

        Serial.print("SoftDevice config require more SRAM than provided by linker.\n"
                     "App Ram Start must be at least ");
        Serial.print(ram_start, HEX);
        Serial.print(" (provided ");
        Serial.print((uint32_t)__data_start__, HEX);
        Serial.print(")\nPlease update linker file or re-config SoftDevice");
    }

    Serial.println("SoftDevice enabled");
    VERIFY_STATUS(err, false);

    /*------------- Configure BLE Option -------------*/
    ble_opt_t  opt;
    varclr(&opt);

    opt.common_opt.conn_evt_ext.enable = 1; // enable Data Length Extension
    VERIFY_STATUS( sd_ble_opt_set(BLE_COMMON_OPT_CONN_EVT_EXT, &opt), false );

    // Init Peripheral role
    ble_gap_conn_params_t _ppcp = ((ble_gap_conn_params_t) {
            .min_conn_interval = BLE_GAP_CONN_MIN_INTERVAL_DFLT,
            .max_conn_interval = BLE_GAP_CONN_MAX_INTERVAL_DFLT,
            .slave_latency     = 0,
            .conn_sup_timeout  = BLE_GAP_CONN_SUPERVISION_TIMEOUT_MS / 10 // in 10ms unit
        });
    // Peripheral Preferred Connection Parameters
    VERIFY_STATUS(sd_ble_gap_ppcp_set(&_ppcp), false);
    VERIFY_STATUS(sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN), false );


    // Default device name
    ble_gap_conn_sec_mode_t sec_mode = BLE_SECMODE_OPEN;
    VERIFY_STATUS(sd_ble_gap_device_name_set(&sec_mode, (uint8_t const *) CFG_DEFAULT_NAME, strlen(CFG_DEFAULT_NAME)), false);

    // Init beacon
    uint8_t *_uuid128 = beaconUuid;
    uint16_t _major_be = __swap16(major);
    uint16_t _minor_be = __swap16(minor);


    // Beacon start:
    uint16_t _count = 0;
    static uint8_t _data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
    varclr(_data);
    static struct ATTR_PACKED
    {
        uint16_t manufacturer;

        uint8_t  beacon_type;
        uint8_t  beacon_len;

        uint8_t  uuid128[16];
        uint16_t major;
        uint16_t minor;
        int8_t   rssi_at_1m;
    } beacon_data =
        {
            .manufacturer = manufacturer_id,
            .beacon_type = 0x02,
            .beacon_len  = sizeof(beacon_data) - 4, // len of uuid + major + minor + rssi
            .uuid128 = { 0 },
            .major = _major_be,
            .minor = _minor_be,
            .rssi_at_1m = rssi_at_1m
        };
    VERIFY_STATIC(sizeof(beacon_data) == 25);
    memcpy(beacon_data.uuid128, _uuid128, 16);

    // Add flags:
    {
        uint8_t data[] = {BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE};
        VERIFY( _count + 1 + 2 <= BLE_GAP_ADV_SET_DATA_SIZE_MAX );
        uint8_t* adv_data = &_data[_count];
        // len (1+data), type, data
        *adv_data++ = (1+1);
        *adv_data++ = BLE_GAP_AD_TYPE_FLAGS;
        memcpy(adv_data, data, 1);
        _count = _count + 1 + 2;
    }

    // Add data:
    {
        uint8_t* adv_data = &_data[_count];
        size_t len = sizeof(beacon_data);
        // len (1+data), type, data
        *adv_data++ = (len+1);
        *adv_data++ = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;
        memcpy(adv_data, &beacon_data, len);
        _count = _count + len + 2;
    }

    // Restart on Disconnect
    bool _start_if_disconnect = true;

    // Advertising timeout in seconds:
    uint16_t timeout = 3;

    // Advertising interval
    uint16_t interval = 160;

    // Really start this time:
    uint8_t _type             = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;


    // ADV Params
    ble_gap_adv_params_t adv_para =
        {
            .properties    = { .type = _type, .anonymous  = 0 },
            .p_peer_addr   = NULL                     , // Undirected advertisement
            .interval      = interval                 , // advertising interval (in units of 0.625 ms)
            .duration      = (uint16_t) (timeout*100) , // in 10-ms unit

            .max_adv_evts  = 0                        , // TODO can be used for fast/slow mode
            .channel_mask  = { 0, 0, 0, 0, 0 }        , // 40 channel, set 1 to disable
            .filter_policy = BLE_GAP_ADV_FP_ANY       ,

            .primary_phy   = BLE_GAP_PHY_AUTO         , // 1 Mbps will be used
            .secondary_phy = BLE_GAP_PHY_AUTO         , // 1 Mbps will be used
            // , .set_id, .scan_req_notification
        };

    // gap_adv long-live is required by SD v6

    static ble_gap_adv_data_t gap_adv =
        {
            .adv_data      = { .p_data = _data, .len = _count },
            .scan_rsp_data = { .p_data = NULL, .len = 0 } // No scan response data needed
        };
    uint8_t _hdl = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
    VERIFY_STATUS( sd_ble_gap_adv_set_configure(&_hdl, &gap_adv, &adv_para), false );
    VERIFY_STATUS( sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, _hdl, _tx_power ), false );
    VERIFY_STATUS( sd_ble_gap_adv_start(_hdl, CONN_CFG_PERIPHERAL), false );

    return true;
}

static volatile bool got_sd_events = false;

// Returns true on timeout
bool handle_sd_events() {

  if(!got_sd_events)
    return false;

  got_sd_events = false;

  uint32_t err = NRF_SUCCESS;

  // Until no pending events
  while( NRF_ERROR_NOT_FOUND != err )
  {
    uint16_t ev_len = BLE_EVT_LEN_MAX(BLE_GATT_ATT_MTU_MAX);

    static uint8_t ev_buf[(BLE_EVT_LEN_MAX(BLE_GATT_ATT_MTU_MAX))];
    ble_evt_t *evt = (ble_evt_t*)(void*)ev_buf;

    // Get BLE Event
    err = sd_ble_evt_get(ev_buf, &ev_len);

    // Handle valid event
    if( NRF_SUCCESS == err)
    {

      if(evt->header.evt_id == BLE_GAP_EVT_ADV_SET_TERMINATED) {
        Serial.println("Advertising timed out");
        return true;
      }

    } else if ( NRF_ERROR_NOT_FOUND != err )
    {
      Serial.print("Event error: ");
      Serial.println(dbg_err_str(err));
    }
  }

  return false;
}

extern "C" void SD_EVT_IRQHandler(void)
{
  got_sd_events = true;
}
