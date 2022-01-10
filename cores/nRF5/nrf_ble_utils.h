#ifndef NRF_BLE_UTILS_H
#define NRF_BLE_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "nrf_sdm.h"
#include "ble.h"
#include "verify.h"



#define ATTR_PACKED   __attribute__ ((packed))
#define memclr(buffer, size)  memset(buffer, 0, size)
#define varclr(_var)          memclr(_var, sizeof(*(_var)))
#define arrclr(_arr)          memclr(_arr, sizeof(_arr[0]) * arrcount(_arr)) // adds type-safety
#define __swap32(x)    __REV(x)                   ///< built-in function to swap Endian of 32-bit number
#define __swap16(u16)  ((uint16_t) __REV16(u16))  ///< built-in function to swap Endian of 16-bit number


#define CFG_BLE_TX_POWER_LEVEL    4
#define CFG_DEFAULT_NAME    "Covid Test nRF52810"

// Bluetooth settings:

#define CFG_MAX_DEVNAME_LEN                     32

#define BLE_GENERIC_TIMEOUT                     100

#define BLE_GAP_CONN_SUPERVISION_TIMEOUT_MS     2000
#define BLE_GAP_CONN_SLAVE_LATENCY              0

#define MS100TO125(ms100) (((ms100)*4)/5)
#define BLE_GAP_CONN_MIN_INTERVAL_DFLT          MS100TO125(20)
#define BLE_GAP_CONN_MAX_INTERVAL_DFLT          MS100TO125(30)

#define BLE_ADV_INTERVAL_FAST_DFLT       32  // 20    ms (in 0.625 ms unit)
#define BLE_ADV_INTERVAL_SLOW_DFLT       244 // 152.5 ms (in 0.625 ms unit)
#define BLE_ADV_FAST_TIMEOUT_DFLT        30  // in seconds

#define BLE_GATT_ATT_MTU_MAX      247


// ???
#define CFG_SD_ATTR_TABLE_SIZE 0xC00



#define BLE_SECMODE_NO_ACCESS        ((ble_gap_conn_sec_mode_t) { .sm = 0, .lv = 0 })
#define BLE_SECMODE_OPEN             ((ble_gap_conn_sec_mode_t) { .sm = 1, .lv = 1 })
#define BLE_SECMODE_ENC_NO_MITM      ((ble_gap_conn_sec_mode_t) { .sm = 1, .lv = 2 })
#define BLE_SECMODE_ENC_WITH_MITM    ((ble_gap_conn_sec_mode_t) { .sm = 1, .lv = 3 })
#define BLE_SECMODE_SIGNED_NO_MITM   ((ble_gap_conn_sec_mode_t) { .sm = 2, .lv = 1 })
#define BLE_SECMODE_SIGNED_WITH_MITM ((ble_gap_conn_sec_mode_t) { .sm = 2, .lv = 2 })


//------------- Min, Max -------------//
static inline uint8_t  min8 (uint8_t  x, uint8_t  y) { return (x < y) ? x : y; }
static inline uint16_t min16(uint16_t x, uint16_t y) { return (x < y) ? x : y; }
static inline uint32_t min32(uint32_t x, uint32_t y) { return (x < y) ? x : y; }

static inline uint8_t  max8 (uint8_t  x, uint8_t  y) { return (x > y) ? x : y; }
static inline uint16_t max16(uint16_t x, uint16_t y) { return (x > y) ? x : y; }
static inline uint32_t max32(uint32_t x, uint32_t y) { return (x > y) ? x : y; }


enum
{
  BANDWIDTH_AUTO = 0,
  BANDWIDTH_LOW,
  BANDWIDTH_NORMAL,
  BANDWIDTH_HIGH,
  BANDWIDTH_MAX,
};

enum
{
  CONN_CFG_PERIPHERAL = 1,
  CONN_CFG_CENTRAL = 2,
};


typedef struct
{
  uint32_t key;
  void const * data;
}lookup_entry_t;

typedef struct
{
  uint16_t count;
  lookup_entry_t const* items;
} lookup_table_t;


/**
 * Check if status is success (zero), otherwise return
 * - status value if called with 1 parameter e.g VERIFY_STATUS(status)
 * - 2 parameter if called with 2 parameters e.g VERIFY_STATUS(status, errorcode)
 */
#define VERIFY_STATUS(...)      _GET_3RD_ARG(__VA_ARGS__, VERIFY_ERR_2ARGS, VERIFY_ERR_1ARGS)(__VA_ARGS__, dbg_err_str)


const char* dbg_err_str(int32_t err_id);

extern lookup_table_t const _strerr_table;

void const *  lookup_find(lookup_table_t const* p_table, uint32_t key);

bool beacon_init(uint16_t manufacturer_id,
                 uint8_t beaconUuid[16],
                 uint16_t major,
                 uint16_t minor,
                 int8_t rssi_at_1m);

extern "C" void SD_EVT_IRQHandler(void);

bool handle_sd_events();
#endif
