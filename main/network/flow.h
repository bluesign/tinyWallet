#ifndef NETWORK_FLOW_H
#define NETWORK_FLOW_H

#include "sds.h"
#include "cJSON.h"
#include "adlist.h"

//resources
extern const uint8_t tx_send_flow_start[] asm("_binary_tx_send_flow_cdc_start");
extern const uint8_t tx_send_flow_end[] asm("_binary_tx_send_flow_cdc_end");

extern const uint8_t flow_png_start[] asm("_binary_flow_png_start");
extern const uint8_t flow_png_end[] asm("_binary_flow_png_end");


typedef struct flowClient
{
    char* host;
    int port;
} flowClient;

typedef unsigned char flowAddress[8];
typedef unsigned char flowIdentifier[32];

typedef struct flowProposer{
    flowAddress address;
    uint64_t key_index;
    uint64_t sequence_number;
} flowProposer;

typedef struct flowSignature{
    flowAddress address;
    uint64_t key_index;
    sds signature;
} flowSignature;

typedef struct flowArgument{
    char* type;
    char* value;
} flowArgument;

typedef struct flowTransaction{
    char* script;
    list* arguments;
    flowIdentifier reference_block_id;
    flowAddress payer;
    flowProposer proposer;
    list* authorizers;
    uint32_t gas_limit;
    list* payload_signatures;
    list* envelope_signatures;
} flowTransaction;

sds flow_get(flowClient* client, const char* path,  const char* query, const char* data);
sds flow_post(flowClient* client, const char* path,  const char* query, const char* data);

cJSON* flow_get_latest_block(flowClient* client);
cJSON* flow_get_account(flowClient* client, const char* address);
cJSON* flow_execute_script(flowClient* client, const char* script, ...);
cJSON* flow_send_transaction(flowClient* client, flowTransaction* tx, ...);

flowAddress* flow_create_address(char* address);
void flow_destroy_address(flowAddress* address);

flowIdentifier* flow_create_identifier(char* identifier);
void flow_destroy_identifier(flowIdentifier* identifier);

flowProposer * flow_create_proposer(char* address, uint64_t key_index, uint64_t sequence_number);
void flow_destroy_proposer(flowProposer* proposer);

flowSignature * flow_create_signature(char* address, uint64_t key_index, char* signature);
void flow_destroy_signature(flowSignature* signature);

flowClient* flow_create_client(char* host, int port);
void flow_destroy_client(flowClient* client);

flowArgument * flow_create_argument(char* type, char* value);
char* flow_encode_argument(flowArgument* argument);
void flow_destroy_argument(flowArgument* argument);

flowTransaction * flow_create_transaction_advanced(
        sds script,
        list* arguments,
        unsigned char* reference_block_id,
        unsigned char* payer,
        flowProposer* proposer,
        list* authorizers,
        list* payload_signatures,
        uint32_t gas_limit);

flowTransaction * flow_create_transaction(
        sds script,
        list* arguments,
        unsigned char* reference_block_id,
        flowProposer* proposer);

void flow_destroy_transaction(flowTransaction* transaction);

sds signTransactionWithSecureElement(flowTransaction* tx, int keySlot);



#endif
