#include "esp_http_client.h"
#include "sds.h"

#ifndef _TINY_WALLET_HTTP_H
#define _TINY_WALLET_HTTP_H

sds http_request(const char* host, int port, esp_http_client_method_t method, const char* path, const char* parameters, const char* data);

#endif
