#ifndef TINY_WALLET_WIFI_H
#define TINY_WALLET_WIFI_H

#include <esp_event.h>
#include <freertos/event_groups.h>
#include "adlist.h"
#include "sds.h"


int availableNetworksCount;
char ssidPassword[64];
char ssidName[32];
list* availableNetworks;

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define WIFI_SCANNING_BIT  BIT2
#define WIFI_SCAN_DONE_BIT BIT3

void wifi_init(esp_event_handler_t user_handler);
void wifi_scan();
void wifi_connect();
void wifi_init_sta();

void save_credentials(char* ssid, char* password);



#endif