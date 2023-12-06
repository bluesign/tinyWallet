#include "./http.h"

#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_tls.h"
#include "sds.h"

static const char *TAG = "HTTP";

#define MAX_HTTP_RECV_BUFFER 1024

sds http_request(const char *host, int port, esp_http_client_method_t method, const char *path, const char *parameters,
                 const char *data) {
    printf("http_request\n");
    char *buffer = malloc(MAX_HTTP_RECV_BUFFER + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Cannot malloc http receive buffer");
        return NULL;
    }

    sds result = sdsnew("");
    esp_http_client_config_t config = {
            .host = host,
            .method = method,
            .port = port,
            .path = path,
            .query = parameters,
            .transport_type = port == 80 ? HTTP_TRANSPORT_OVER_TCP : HTTP_TRANSPORT_OVER_SSL,
            .disable_auto_redirect = false,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (method==HTTP_METHOD_POST){
        esp_http_client_set_header(client, "Content-Type", "application/json");
    }
    printf("http_request2\n");

    esp_err_t err;
    if ((err = esp_http_client_open(client, method==HTTP_METHOD_POST?strlen(data):0)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        free(result);
        return NULL;
    }
    printf("http_request33\n");
    if (method==HTTP_METHOD_POST){
        esp_http_client_write(client, data, strlen(data));
    }
    printf("http_request3\n");

    int content_length =  esp_http_client_fetch_headers(client);
    ESP_LOGI(TAG, "HTTP connection opened, content_length=%d", content_length);
    int read_len;
    while (true) {
        read_len = esp_http_client_read(client, buffer, MAX_HTTP_RECV_BUFFER);
        result = sdscatlen(result, buffer, read_len);
        if (read_len <= 0) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            free(buffer);
            return result;
        }
    }

}