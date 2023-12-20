/*
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_system.h"

#include "nvs_flash.h"
#include "mqtt_client.h"

#include "wifi.h"
#include "hw_ctrl.h"
#include "wqtt_client.h"
#include "smartRelay.h"

/********************************************************
 *  STATIC PROTOTYPES
 ********************************************************/

static void tick_task(void *arg);

/*******************************************************
 *  STATIC VARIABLES
 *******************************************************/

static uint32_t     fan_speed = HW_LVL_LOW;
static hw_state_t   light_state = HW_OFF;
static hw_state_t   heater_state = HW_OFF;
static uint32_t     current = 0;


static const char *TAG = "SMART RELAY";

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;



/**********************
 *   APPLICATION MAIN
 **********************/


void app_main(void)
{
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    // xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);

    wifi_start();

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "Connecting to WiFi..");

    while( wifi_get_ip()[0] == 'N')
    {
        vTaskDelay(pdMS_TO_TICKS(10));        
    }

    ESP_LOGI(TAG, "IP address=%s", wifi_get_ip() );

    hw_ctrl_start();
    wqtt_client_start();

    // Initiazlize UI controls
    ui_set_fan_speed(HW_LVL_OFF);
    ui_set_current_value(0);
    ui_set_heater_state(HW_OFF);
    ui_set_light_state(HW_OFF);

    // Initialize HW
    hw_ctrl_set_Fan_level(HW_LVL_OFF);
    hw_ctrl_set_Heater_state(HW_OFF);
    hw_ctrl_set_Light_state(HW_LVL_OFF);
}


/**
 * @brief Calls periodically
 * 
 * @param arg Arguments
 */
static void tick_task(void *arg) 
{
    (void) arg;


}


/**********************************************************
 UI FUNCTIONS
 **********************************************************/

void ui_set_fan_speed(uint32_t new_fan_speed)
{
    if(new_fan_speed > 5) {
        new_fan_speed = 5;
    }

}

void ui_set_light_state(hw_state_t new_state)
{
    if(new_state == HW_OFF)
    {
        light_state = HW_OFF;

    } else {
        light_state = HW_ON;

    }
}

void ui_set_heater_state(hw_state_t new_state)
{
    if(new_state == HW_OFF)
    {
        heater_state = HW_OFF;

    } else {
        heater_state = HW_ON;

    }

}

void ui_set_current_value(uint32_t new_current_value)
{
    char str[16];

    current = new_current_value;
    sprintf(str, "%d", current);

}
