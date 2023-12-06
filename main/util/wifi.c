#include "wifi.h"
#include <string.h>
#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "http.h"
#include "cJSON.h"

#define DEFAULT_SCAN_LIST_SIZE 20

static const char *TAG = "wifi";
static int s_retry_num = 0;
static esp_event_handler_t user_handler = NULL;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    printf("Event: %s\n", event_base);
    printf("Event: %d\n", event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "connect to the AP");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP) {
        ESP_LOGI(TAG, "disconnect from the AP");
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    if (user_handler != NULL) {
        user_handler(arg, event_base, event_id, event_data);
    };
}

static void task_connect_wifi(void *pvParameters) {
    wifi_config_t wifi_config = {
            .sta = {
                    .threshold.authmode = WIFI_AUTH_WPA_PSK
            },
    };
    strcpy((char *) wifi_config.sta.ssid, (char *) ssidName);
    strcpy((char *) wifi_config.sta.password, (char *) ssidPassword);

    printf("Connecting to %s\n", wifi_config.sta.ssid);
    printf("Password: %s\n", wifi_config.sta.password);
    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ssidName, ssidPassword);
        save_credentials(ssidName, ssidPassword);

    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ssidName, ssidPassword);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    vTaskDelete(NULL);
}

static void task_wifi_scan(void *pvParameters) {
    printf("Scanning... 1\n");
    xEventGroupSetBits(s_wifi_event_group, WIFI_SCANNING_BIT);

    listRelease(availableNetworks);
    availableNetworks = listCreate();

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_cfg_empty;
    memset(&wifi_cfg_empty, 0, sizeof(wifi_config_t));
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_cfg_empty);
    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_scan_start(NULL, true);
    ESP_LOGI(TAG, "Max AP number ap_info can hold = %u", number);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, number);

    availableNetworksCount = ap_count;
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        wifi_ap_record_t *ap = &ap_info[i];
        char *ssid = (char *) malloc(33);
        memcpy(ssid, ap->ssid, 32);
        ssid[32] = '\0';
        listAddNodeTail(availableNetworks, ssid);
    }
    esp_wifi_stop();

    vTaskDelete(NULL);

}

void wifi_scan() {
    xTaskCreate(task_wifi_scan,
                "ScanWIFITask",
                4096,
                NULL,
                1,
                NULL);
}

void save_credentials(char* ssid, char* password) {
    cJSON *doc = cJSON_CreateObject();
    cJSON_AddStringToObject(doc, "ssid", ssid);
    cJSON_AddStringToObject(doc, "password", password);
    printf("Saving credentials\n");
    char *json = cJSON_Print(doc);
    int f = open("/data/wifi.json", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    write(f, json, strlen(json));
    printf("Saving credentials: %s\n", json);
    close(f);
    free(json);
    cJSON_Delete(doc);
}

static bool load_credentials() {
    FILE *f = fopen("/data/wifi.json", "r");
    if (f == NULL) {
        printf("No credentials\n");
        return false;
    }
    char *json = malloc(1024);

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    printf("File size: %d\n", (int) fsize);

    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    cJSON *doc = cJSON_Parse(string);
    free(string);

    if (doc == NULL) {
        printf("Invalid JSON\n");
        return false;
    }

    cJSON *ssid = cJSON_GetObjectItem(doc, "ssid");
    cJSON *password = cJSON_GetObjectItem(doc, "password");
    if (ssid == NULL || password == NULL) {
        printf("Invalid JSON\n");
        return false;
    }
    strcpy(ssidName, ssid->valuestring);
    strcpy(ssidPassword, password->valuestring);
    return true;
}

static void try_previous_network() {
    if (load_credentials()) {
        wifi_connect();
    } else {
        wifi_scan();
    }
}


void wifi_init(esp_event_handler_t user_event_handler) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));


    user_handler = user_event_handler;
    availableNetworksCount = 0;
    availableNetworks = listCreate();
    try_previous_network();
}

void wifi_connect() {
    xTaskCreate(task_connect_wifi,
                "task_connect_wifi",
                1024 * 8,
                NULL,
                1,
                NULL);
}

