#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "core2forAWS.h"
#include "ui.h"
#include "ui_comp_coin.h"
#include "util/http.h"
#include <stdio.h>
#include <freertos/event_groups.h>
#include <esp_spiffs.h>
#include <esp_wifi.h>
#include "util/wifi.h"
#include "flow.h"
#include "cJSON.h"
#include "file.h"
#include "my.h"

static void load_fungible(void* arg);
static void load_nonfungible(void* arg);

extern SemaphoreHandle_t xGuiSemaphore;

static int last_available_count = 0;

sds mykey;
sds myAddress;
sds myAddressSans;


void btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {

        if (btn == apSettingsCloseBtn) {
            lv_obj_move_background(apSettings);
            lv_obj_add_flag(apSettings, LV_OBJ_FLAG_HIDDEN);
        }

        if (btn == mboxCloseBtn) {
            lv_obj_move_background(mboxConnect);
            lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
        }

        if (btn == mboxConnectBtn) {
            lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_background(mboxConnect);
            const char *_ssidPassword = lv_textarea_get_text(mboxPassword);
            memset(ssidPassword, 0, sizeof(ssidPassword));
            strncpy(ssidPassword, _ssidPassword, strlen(_ssidPassword));
            wifi_connect();
        }


    } else if (code == LV_EVENT_VALUE_CHANGED) {
        if (btn == settingWiFiSwitch) {

            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                last_available_count = -1;
                esp_wifi_disconnect();
                esp_wifi_stop();
                wifi_scan();
            } else {
                esp_wifi_disconnect();
                esp_wifi_stop();
            }
        }
    }
}

void text_input_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED) {
        lv_obj_move_foreground(keyboard);
        lv_keyboard_set_textarea(keyboard, ta);
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}


cJSON *cJSON_Select(cJSON *o, const char *fmt, ...);




static void list_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        if (e->user_data == wfList){
            const char *selectedItem = lv_list_get_btn_text(wfList, obj);
            printf("Selected: %s\n", selectedItem);
            memset(ssidName, 0, sizeof(ssidName));
            strncpy(ssidName, selectedItem, strlen(selectedItem));
            lv_obj_clear_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(mboxConnect);
        }else if (e->user_data==nftList){
            printf("NFT Selected\n");
            sds path = sdsnew("MomentCollection");
            xTaskCreate(load_nonfungible, "taskLoadCollection", 4096 * 4, path, 2, NULL);
        }
    }

}

static void showingFoundWiFiList() {
    if (availableNetworksCount == 0 || availableNetworksCount == last_available_count)
        return;

    lv_obj_clear_flag(apSettings, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(apSettings);
    xEventGroupClearBits(s_wifi_event_group, WIFI_SCANNING_BIT);

    lv_obj_clean(wfList);
    last_available_count = 0;

    listIter *iter = listGetIterator(availableNetworks, AL_START_HEAD);
    listNode *node;

    while ((node = listNext(iter)) != NULL) {
        char *item = (char *) listNodeValue(node);
        lv_obj_t *btn = lv_list_add_btn(wfList, LV_SYMBOL_WIFI, item);
        lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, wfList);
        last_available_count++;
    }

}


static void taskUpdateUI(void *arg) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        EventBits_t uxBits = xEventGroupGetBits(s_wifi_event_group);
        xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
        lv_obj_set_style_text_opa(
                wifiIcon,
                uxBits & WIFI_CONNECTED_BIT ? 0xFF : 0x90,
                LV_PART_MAIN | LV_STATE_DEFAULT);

        if (uxBits & WIFI_SCANNING_BIT) {
            showingFoundWiFiList();
        }

        if (uxBits & WIFI_CONNECTED_BIT) {
            lv_obj_move_background(apSettings);
            lv_obj_add_flag(apSettings, LV_OBJ_FLAG_HIDDEN);
        }

        xSemaphoreGive(xGuiSemaphore);
        taskYIELD();
    }
    vTaskDelete(NULL);
}

static void load_fungible(void* arg) {

    printf("taskReloadFungible\n");
    flowClient *client = flow_create_client("rest-mainnet.onflow.org", 443);

    char *ix_ft_list = read_file_all("/data/ix/ft_list.cdc");
    flowArgument *script_arg = flow_create_argument("Address", myAddress);
    cJSON *result = flow_execute_script(
            client,
            ix_ft_list,
            script_arg,
            NULL);
    free(ix_ft_list);

    int size = cJSON_GetArraySize( cJSON_Select(result, ".value"));

    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    for (int i = 0; i < size; i++) {
        cJSON *vault = cJSON_Select(result, ".value[*]", i);

        lv_obj_t *coin = lv_list_add_text(ftList, "");
        lv_obj_t *ui_coin_temp = ui_coin_create(coin);
        lv_obj_clear_flag(coin, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(ui_coin_temp, list_event_handler, LV_EVENT_CLICKED, ftList);
        lv_label_set_text(ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_DESCRIPTION_PRICE), "$0.00");
        lv_label_set_text(ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_ITEMS_VALUE), "$0.00");

        for (int j = 0; j <  cJSON_GetArraySize(cJSON_Select(vault, ".value")); j++) {
            cJSON *key = cJSON_Select(vault, ".value[*].key.value", j);
            cJSON *value = cJSON_Select(vault, ".value[*].value.value", j);

            if (strcmp(key->valuestring, "identifier") == 0) {
                sds imageURL = sdsnew("A:data/ft/");
                imageURL = sdscat(imageURL, value->valuestring);
                imageURL = sdscatlen(imageURL, ".png", 4);
                printf("Image URL: %s\n", imageURL);
                lv_img_set_src(ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_IMAGE), imageURL);
                sdsfree(imageURL);
            } else if (strcmp(key->valuestring, "name") == 0) {
                lv_label_set_text(
                        ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_NAME),
                        value->valuestring
                );
            } else if (strcmp(key->valuestring, "balance") == 0) {
                lv_label_set_text_fmt(
                        ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_COUNT),
                        "%.02f",
                        atof(value->valuestring));
            } else if (strcmp(key->valuestring, "price") == 0) {
                lv_label_set_text_fmt(
                        ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_DESCRIPTION_PRICE),
                        "$%.02f",
                        atof(value->valuestring));
            } else if (strcmp(key->valuestring, "total") == 0) {
                lv_label_set_text_fmt(
                        ui_comp_get_child(ui_coin_temp, UI_COMP_COIN_ITEMS_VALUE),
                        "$%.02f",
                        atof(value->valuestring));
            }

        }

    }
    xSemaphoreGive(xGuiSemaphore);
    cJSON_Delete(result);
    flow_destroy_argument(script_arg);
    flow_destroy_client(client);

    xTaskCreate(load_nonfungible, "taskLoadNonFungible", 4096 * 4, NULL, 2, NULL);

    vTaskDelete(NULL);

}

static void load_nonfungible(void* arg) {
    printf("taskReloadNonFungible\n");
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    lv_obj_clean(nftList);
    xSemaphoreGive(xGuiSemaphore);

    flowClient *client = flow_create_client("rest-mainnet.onflow.org", 443);

    flowArgument *script_arg = flow_create_argument("Address", myAddress);
    flowArgument *script_arg2 = NULL;

    char *script = read_file_all("/data/ix/nft_list.cdc");

    if (arg!=NULL){
        script_arg2 = flow_create_argument("String", arg);
        free(script);
        script = read_file_all("/data/ix/nft_item_list.cdc");
    }

    cJSON *resultNFT = flow_execute_script(
            client,
            script,
            script_arg,
            script_arg2,
            NULL);
    free(script);


    cJSON *collections = cJSON_Select(resultNFT, ".value");
    int size = cJSON_GetArraySize(collections);
    printf("NFTs: %d\n", size);
    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);

    for (int i = 0; i < size; i++) {
        cJSON *collection = cJSON_Select(resultNFT, ".value[*]", i);

        lv_obj_t *nft = lv_list_add_text(nftList, "");
        lv_obj_clear_flag(nft, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *ui_nft_temp = ui_coin_create(nft);

        lv_label_set_text(ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_DESCRIPTION_PRICE), "");
        lv_label_set_text(ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_ITEMS_VALUE), "");
        lv_label_set_text(ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_COUNT), "");

        cJSON *fields = cJSON_Select(collection, ".value");
        int sizeFields = cJSON_GetArraySize(fields);

        for (int j = 0; j < sizeFields; j++) {
            cJSON *field = cJSON_GetArrayItem(fields, j);
            cJSON *key = cJSON_Select(field, ".key.value");
            cJSON *value = cJSON_Select(field, ".value.value");

            if (strcmp(key->valuestring, "identifier") == 0) {
                sds imageURL = sdsnew("A:data/nft/");
                imageURL = sdscatlen(imageURL, value->valuestring, strlen(value->valuestring));
                imageURL = sdscatlen(imageURL, ".png", 4);
                printf("Image URL: %s\n", imageURL);
                lv_img_set_src(ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_IMAGE), imageURL);
                sdsfree(imageURL);
            } else if (strcmp(key->valuestring, "name") == 0) {
                lv_label_set_text(
                        ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_NAME),
                        value->valuestring
                );
            }  else if (strcmp(key->valuestring, "count") == 0) {
                lv_label_set_text_fmt(
                        ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_DESCRIPTION_PRICE),
                        "%d %s",
                        atoi(value->valuestring),
                        atoi(value->valuestring) == 1 ? "Item" : "Items"
                );
            } else if (strcmp(key->valuestring, "id") == 0) {
                lv_label_set_text_fmt(
                        ui_comp_get_child(ui_nft_temp, UI_COMP_COIN_DESCRIPTION_PRICE),
                        "ID: %s",
                        value->valuestring
                );
            } else if (strcmp(key->valuestring, "path") == 0) {
                lv_obj_add_event_cb(ui_nft_temp, list_event_handler, LV_EVENT_CLICKED, value->valuestring);
            }



        }

    }
    xSemaphoreGive(xGuiSemaphore);

    cJSON_Delete(resultNFT);
    flow_destroy_argument(script_arg);
    flow_destroy_client(client);


    vTaskDelete(NULL);

}




static void taskTransaction(void *arg) {
    printf("taskTransaction\n");

    flowClient *client = flow_create_client("rest-mainnet.onflow.org", 443);

    cJSON* latest_block = flow_get_latest_block(client);
    printf("Latest block: %s\n", cJSON_Print(latest_block));
    cJSON* block_id = cJSON_Select(latest_block, "[0].header.id");
    printf("Block ID: %s\n", block_id->valuestring);

    cJSON* account = flow_get_account(client, myAddressSans);
    printf("Account: %s\n", cJSON_Print(account));
    cJSON* account_keys = cJSON_Select(account, ".keys");

    int sequenceNumber = -1;
    int keyIndex = -1;

    for(int i = 0; i < cJSON_GetArraySize(account_keys); i++){
        cJSON* key = cJSON_Select(account, ".keys[*]", i);
        cJSON* key_index = cJSON_Select(key, ".index");
        cJSON* key_sequence_number = cJSON_Select(key, ".sequence_number");
        cJSON* key_public_key = cJSON_Select(key, ".public_key");
        printf("Key %d: %s %s %s\n", i, key_index->valuestring, key_sequence_number->valuestring, key_public_key->valuestring);

        if (strcmp(&key_public_key->valuestring[2], mykey) == 0) {
            sequenceNumber = atoi(key_sequence_number->valuestring);
            keyIndex = atoi(key_index->valuestring);
            continue;
        }
    }

    printf("Chosen Sequence number: %d\n", sequenceNumber);
    printf("Chosen Key index: %d\n", keyIndex);

    flowProposer* proposer =  flow_create_proposer(
            myAddressSans,
            keyIndex,
            sequenceNumber);


    flowArgument* arg1 =  flow_create_argument("UFix64","0.1");
    flowArgument* arg2 = flow_create_argument("Address", myAddress);
    list* arguments = listCreate();
    listAddNodeTail(arguments, arg1);
    listAddNodeTail(arguments, arg2);

    sds tx_send_flow = sdsnewlen(tx_send_flow_start, tx_send_flow_end - tx_send_flow_start);
    flowTransaction* transaction = flow_create_transaction(
            tx_send_flow,
            arguments,
            (unsigned char*)block_id->valuestring,
            proposer
    );

    //sign transaction
    flowSignature* signature = flow_create_signature(
            myAddressSans,
            keyIndex,
            signTransactionWithSecureElement(transaction, 0)
            );

    listAddNodeHead(transaction->envelope_signatures, signature);

    printf("transaction made\n");

    cJSON* result = flow_send_transaction(client, transaction, NULL);
    printf("Result: %s\n", cJSON_Print(result));


    vTaskDelete(NULL);
}



static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        printf("Got IP\n");
        xTaskCreate(load_fungible, "loadFungible", 4096 * 4, NULL, 2, NULL);
    }
}


esp_err_t init_fs(void) {
    const char *TAG = "spiffs";
    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/data",
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }


    return ESP_OK;
}


void app_main(void) {
    mykey = sdsnew(
            "d45cea885bdc7a5e81db420c6ee12db2242c7390e1bd36b6ebe789ac6a18d2f6e9797f63ec574167caf3f9a51d5226caa9e72e6bd12bbdcff78ccb481d893769");
    myAddress = sdsnew("0x73e4a1094d0bcab6");
    myAddressSans = sdsnew("73e4a1094d0bcab6");

    esp_log_level_set("gpio", ESP_LOG_NONE);
    esp_log_level_set("ILI9341", ESP_LOG_NONE);

    Core2ForAWS_Init();

    init_fs();

    xSemaphoreTake(xGuiSemaphore, portMAX_DELAY);
    ui_init();
    xSemaphoreGive(xGuiSemaphore);

    xTaskCreate(taskUpdateUI, "taskUpdateUI", 1024 * 8, NULL, 2, NULL);
    wifi_init(&event_handler);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}





