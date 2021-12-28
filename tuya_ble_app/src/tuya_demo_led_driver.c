/**
 * @file tuya_demo_led_driver.c
 * @author lifan
 * @brief led driver demo source file
 * @version 1.0
 * @date 2021-09-23
 *
 * @copyright Copyright (c) tuya.inc 2021
 *
 */

#include "tuya_demo_led_driver.h"
#include "tuya_led.h"
#include "tuya_ble_log.h"

/***********************************************************
************************micro define************************
***********************************************************/
/* LED handle */
#define LED1_HANDLE             0
#define LED2_HANDLE             1

/* LED flash settings */
#define LED_FLASH_SAME_INTV     300
#define LED_FLASH_ON_TIME       400
#define LED_FLASH_OFF_TIME      200
#define LED_FLASH_TOTAL_TIME    1800
#define LED_FLASH_TOTAL_COUNT   3

/* DP ID */
#define DP_ID_LED1_MODE         101
#define DP_ID_LED2_MODE         102

/***********************************************************
***********************typedef define***********************
***********************************************************/
typedef BYTE_T LED_MODE_E;
#define LED_LIGHT_ON            0x00
#define LED_LIGHT_OFF           0x01
#define LED_FLASH_TIME_SAME     0x02
#define LED_FLASH_TIME_DIFF     0x03
#define LED_FLASH_COUNT_SAME    0x04
#define LED_FLASH_COUNT_DIFF    0x05
#define LED_FLASH_FOREVER_SAME  0x06
#define LED_FLASH_FOREVER_DIFF  0x07

/***********************************************************
***********************variable define**********************
***********************************************************/
/* LED user define */
STATIC TY_GPIO_PORT_E sg_user_led_pin[] = {
    TY_GPIOD_4,   /* LED1 */
    TY_GPIOB_6,   /* LED2 */
};
LED_HANDLE g_user_led_handle[(SIZEOF(sg_user_led_pin) / SIZEOF(sg_user_led_pin[0]))];

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief led driver init
 * @param[in] none
 * @return none
 */
VOID_T tuya_led_driver_init(VOID_T)
{
    UCHAR_T i;
    LED_RET ret = LED_OK;

    for (i = 0; i < (SIZEOF(sg_user_led_pin) / SIZEOF(sg_user_led_pin[0])); i++) {
        ret = tuya_create_led_handle(sg_user_led_pin[i], FALSE, &g_user_led_handle[i]);
        if (ret != LED_OK) {
            TUYA_APP_LOG_ERROR("led init err:%d", ret);
        }
        tuya_set_led_light(g_user_led_handle[i], TRUE);
    }
}

/**
 * @brief led flash end callback function
 * @param[in] none
 * @return none
 */
STATIC VOID_T __led_flash_end_cb()
{
    TUYA_APP_LOG_INFO("LED flash end");
}

/**
 * @brief led control
 * @param[in] led_num: led handle num
 * @param[in] mode: led driver mode
 * @return none
 */
STATIC VOID_T __led_ctrl(IN CONST UCHAR_T led_num, IN CONST LED_MODE_E mode)
{
    switch (mode) {
    case LED_LIGHT_ON:
        tuya_set_led_light(g_user_led_handle[led_num], TRUE);
        TUYA_APP_LOG_INFO("LED%d mode is LED_LIGHT_ON", led_num+1);
        break;
    case LED_LIGHT_OFF:
        tuya_set_led_light(g_user_led_handle[led_num], FALSE);
        TUYA_APP_LOG_INFO("LED%d mode is LED_LIGHT_OFF", led_num+1);
        break;
    case LED_FLASH_TIME_SAME:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_SPEC_TIME, LFT_STA_ON_END_ON, LED_FLASH_SAME_INTV, LED_FLASH_SAME_INTV, LED_FLASH_TOTAL_TIME, NULL);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_TIME_SAME", led_num+1);
        break;
    case LED_FLASH_TIME_DIFF:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_SPEC_TIME, LFT_STA_ON_END_OFF, LED_FLASH_ON_TIME, LED_FLASH_OFF_TIME, LED_FLASH_TOTAL_TIME, __led_flash_end_cb);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_TIME_DIFF", led_num+1);
        break;
    case LED_FLASH_COUNT_SAME:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_SPEC_COUNT, LFT_STA_OFF_END_ON, LED_FLASH_SAME_INTV, LED_FLASH_SAME_INTV, LED_FLASH_TOTAL_COUNT, __led_flash_end_cb);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_COUNT_SAME", led_num+1);
        break;
    case LED_FLASH_COUNT_DIFF:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_SPEC_COUNT, LFT_STA_OFF_END_OFF, LED_FLASH_ON_TIME, LED_FLASH_OFF_TIME, LED_FLASH_TOTAL_COUNT, NULL);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_COUNT_DIFF", led_num+1);
        break;
    case LED_FLASH_FOREVER_SAME:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_FOREVER, LFT_STA_ON_END_ON, LED_FLASH_SAME_INTV, LED_FLASH_SAME_INTV, 0, NULL);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_FOREVER_SAME", led_num+1);
        break;
    case LED_FLASH_FOREVER_DIFF:
        tuya_set_led_flash(g_user_led_handle[led_num], LFM_FOREVER, LFT_STA_OFF_END_OFF, LED_FLASH_ON_TIME, LED_FLASH_OFF_TIME, 0, NULL);
        TUYA_APP_LOG_INFO("LED%d mode is LED_FLASH_FOREVER_DIFF", led_num+1);
        break;
    default:
        break;
    }
}

/**
 * @brief dp data handler
 * @param[in] dp_data: dp data array
 * @return none
 */
VOID_T tuya_ble_dp_write_handler(IN UCHAR_T *dp_data)
{
    switch (dp_data[0]) {
    case DP_ID_LED1_MODE:
        __led_ctrl(LED1_HANDLE, dp_data[3]);
        break;
    case DP_ID_LED2_MODE:
        __led_ctrl(LED2_HANDLE, dp_data[3]);
        break;
    default:
        break;
    }
}
