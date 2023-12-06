#include <stdio.h>
#include "flow.h"
#include "http.h"
#include "cJSON.h"
#include "sds.h"
#include "hex.h"
#include "base64.h"

#include "cryptoauthlib.h"
#include "esp_log.h"

#include "rlp.h"

sds prepareEnvelopeWithDomainTag(flowTransaction* tx){

    //domain tag
    sds rlpEncodedResult = sdsnewlen("FLOW-V0.0-transaction\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32);

    //fixed members
    sds rlpScript = rlp_encode_element((unsigned char*)tx->script, strlen(tx->script));
    sds rlpReferenceBlockId = rlp_encode_element((unsigned char *)tx->reference_block_id, 32);
    sds rlpGasLimit = rlp_encode_number((uint32_t)tx->gas_limit);
    sds rlpProposerAddress = rlp_encode_element((unsigned char *)tx->proposer.address, 8);
    sds rlpProposerKeyIndex = rlp_encode_number(tx->proposer.key_index);
    sds rlpProposerSequenceNumber = rlp_encode_number(tx->proposer.sequence_number);
    sds rlpPayerAddress = rlp_encode_element((unsigned char *)tx->payer, 8);

    //list members
    listNode *node;

    //arguments
    sds rlpArgumentsContent = sdsempty();
    listIter* iterArgs = listGetIterator(tx->arguments, AL_START_HEAD);
    while ((node = listNext(iterArgs)) != NULL) {
        flowArgument *argument =  (flowArgument *)listNodeValue(node);
        char* encoded = flow_encode_argument(argument);
        sds rlpArgument = rlp_encode_element((unsigned char*) encoded, strlen((char*)encoded));
        free(encoded);
        rlpArgumentsContent = sdscatlen(rlpArgumentsContent, rlpArgument, sdslen(rlpArgument));
        sdsfree(rlpArgument);
    }
    listReleaseIterator(iterArgs);
    sds rlpArguments = rlp_encode_list(rlpArgumentsContent, sdslen(rlpArgumentsContent));
    sdsfree(rlpArgumentsContent);

    //authorizers
    sds rlpAuthorizerContent = sdsempty();
    listIter* iterAuth = listGetIterator(tx->authorizers, AL_START_HEAD);
    while ((node = listNext(iterAuth)) != NULL) {
        flowAddress *address =  (flowAddress *)listNodeValue(node);
        sds rlpAuthorizerAddress = rlp_encode_element((unsigned char *)address, 8);
        rlpAuthorizerContent = sdscatlen(rlpAuthorizerContent, rlpAuthorizerAddress, sdslen(rlpAuthorizerAddress));
        sdsfree(rlpAuthorizerAddress);
    }
    listReleaseIterator(iterAuth);
    sds rlpAuthorizers = rlp_encode_list(rlpAuthorizerContent, sdslen(rlpAuthorizerContent));

    //payload signatures
    sds rlpPayloadSignaturesContent = sdsempty();
    listIter* iterSignature = listGetIterator(tx->payload_signatures, AL_START_HEAD);
    while ((node = listNext(iterSignature)) != NULL) {
        flowSignature *signature =  (flowSignature *)listNodeValue(node);
        sds rlpSignatureContent = sdsempty();

        sds rlpSignatureBytes = rlp_encode_element((unsigned char*) signature->signature, sdslen(signature->signature));
        rlpSignatureContent = sdscatlen(rlpSignatureContent, rlpSignatureBytes, sdslen(rlpSignatureBytes));
        sdsfree(rlpSignatureBytes);

        sds rlpSignatureAddress = rlp_encode_element((unsigned char *)signature->address, 8);
        rlpSignatureContent = sdscatlen(rlpSignatureContent, rlpSignatureAddress, sdslen(rlpSignatureAddress));
        sdsfree(rlpSignatureAddress);

        sds rlpSignatureKeyIndex = rlp_encode_number(signature->key_index);
        rlpSignatureContent = sdscatlen(rlpSignatureContent, rlpSignatureKeyIndex, sdslen(rlpSignatureKeyIndex));
        sdsfree(rlpSignatureKeyIndex);

        sds rlpSignature = rlp_encode_list(rlpSignatureContent, sdslen(rlpSignatureContent));
        sdsfree(rlpSignatureContent);

        rlpPayloadSignaturesContent = sdscatlen(rlpPayloadSignaturesContent, rlpSignature, sdslen(rlpSignature));
        sdsfree(rlpSignature);
    }
    listReleaseIterator(iterSignature);

    sds rlpPayloadSignatures = rlp_encode_list(rlpPayloadSignaturesContent, sdslen(rlpPayloadSignaturesContent));
    sdsfree(rlpPayloadSignaturesContent);

    sds rlpPayloadContents = sdsempty();
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpScript, sdslen(rlpScript));
    sdsfree(rlpScript);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpArguments, sdslen(rlpArguments));
    sdsfree(rlpArguments);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpReferenceBlockId, sdslen(rlpReferenceBlockId));
    sdsfree(rlpReferenceBlockId);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpGasLimit, sdslen(rlpGasLimit));
    sdsfree(rlpGasLimit);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpProposerAddress, sdslen(rlpProposerAddress));
    sdsfree(rlpProposerAddress);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpProposerKeyIndex, sdslen(rlpProposerKeyIndex));
    sdsfree(rlpProposerKeyIndex);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpProposerSequenceNumber, sdslen(rlpProposerSequenceNumber));
    sdsfree(rlpProposerSequenceNumber);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpPayerAddress, sdslen(rlpPayerAddress));
    sdsfree(rlpPayerAddress);
    rlpPayloadContents = sdscatlen(rlpPayloadContents, rlpAuthorizers, sdslen(rlpAuthorizers));
    sdsfree(rlpAuthorizers);

    sds rlpPayload = rlp_encode_list(rlpPayloadContents, sdslen(rlpPayloadContents));
    sdsfree(rlpPayloadContents);

    sds rlpEnvelopeContents = sdsempty();
    rlpEnvelopeContents = sdscatlen(rlpEnvelopeContents, rlpPayload, sdslen(rlpPayload));
    sdsfree(rlpPayload);
    rlpEnvelopeContents = sdscatlen(rlpEnvelopeContents, rlpPayloadSignatures, sdslen(rlpPayloadSignatures));
    sdsfree(rlpPayloadSignatures);

    sds rlpEnvelope = rlp_encode_list(rlpEnvelopeContents, sdslen(rlpEnvelopeContents));
    sdsfree(rlpEnvelopeContents);

    rlpEncodedResult = sdscatlen(rlpEncodedResult, rlpEnvelope, sdslen(rlpEnvelope));
    sdsfree(rlpEnvelope);

    return rlpEncodedResult;
}


sds bytes_to_hex(unsigned char* bytes, int length){
    char* hex = malloc(length*2+1);
    for (int i=0;i<length;i++){
        sprintf(&hex[i*2], "%02x", bytes[i]);
    }
    hex[length*2] = '\0';
    sds result = sdsnew(hex);
    free(hex);
    return result;
}

flowAddress* flow_create_address(char* address){
    flowAddress* flowAddress = malloc(sizeof(flowAddress));
    unsigned char* address_bytes = hex_to_byte(address);
    memcpy(flowAddress, address_bytes, 8);
    free(address_bytes);
    return flowAddress;
}

void flow_destroy_address(flowAddress* address){
    free(address);
}

flowIdentifier* flow_create_identifier(char* identifier){
    flowIdentifier* flow_identifier = malloc(sizeof(flowIdentifier));
    unsigned char* identifier_bytes = hex_to_byte(identifier);
    memcpy(flow_identifier, identifier_bytes, 32);
    free(identifier_bytes);
    return flow_identifier;
}

flowProposer* flow_create_proposer(char* address, uint64_t key_index, uint64_t sequence_number){
    flowProposer* proposer = malloc(sizeof(flowProposer));
    unsigned char* address_bytes = hex_to_byte(address);
    memcpy(proposer->address, address_bytes, 8);
    free(address_bytes);
    proposer->key_index = key_index;
    proposer->sequence_number = sequence_number;
    return proposer;
}

void flow_destroy_proposer(flowProposer* proposer){
    free(proposer);
}

flowSignature* flow_create_signature(char* address, uint64_t key_index, char* signature){
    flowSignature* flowSignature = malloc(sizeof(flowSignature));
    unsigned char* address_bytes = hex_to_byte(address);
    memcpy(flowSignature->address, address_bytes, 8);
    free(address_bytes);
    flowSignature->key_index = key_index;
    flowSignature->signature = signature;
    return flowSignature;
}

void flow_destroy_signature(flowSignature* signature){
    sdsfree(signature->signature);
    free(signature);
}

flowArgument * flow_create_argument(char* type, char* value){
    flowArgument* argument = malloc(sizeof(flowArgument));
    argument->type = type;
    argument->value = value;
    return argument;
}

char* flow_encode_argument(flowArgument* argument){

    cJSON* doc = cJSON_CreateObject();
    cJSON_AddStringToObject(doc,"type", argument->type);
    cJSON_AddStringToObject(doc,"value", argument->value);
    char* encodedRequest = cJSON_PrintUnformatted(doc);
    char* copy = malloc(strlen(encodedRequest)+1);
    strcpy(copy, encodedRequest);
    cJSON_free(doc);
    return copy;
}

void flow_destroy_argument(flowArgument* argument){
    free(argument);
}

flowTransaction * flow_create_transaction(
        sds script,
        list* arguments,
        unsigned char* reference_block_id,
        flowProposer* proposer){

    list* authorizers = listCreate();
    list* payload_signatures = listCreate();
    if (arguments==NULL){
        arguments = listCreate();
    }
    listAddNodeTail(authorizers, proposer->address);

    return flow_create_transaction_advanced(
            script,
            arguments,
            reference_block_id,
            (unsigned char*)bytes_to_hex(proposer->address, 8),
            proposer,
            authorizers,
            payload_signatures,
            9999
    );
}

flowTransaction * flow_create_transaction_advanced(
        sds script,
        list* arguments,
        unsigned char* reference_block_id,
        unsigned char* payer,
        flowProposer* proposer,
        list* authorizers,
        list* payload_signatures,
        uint32_t gas_limit){

    flowTransaction* transaction = malloc(sizeof(flowTransaction));
    transaction->script = script;
    transaction->arguments = arguments;
    memcpy(transaction->reference_block_id, hex_to_byte((char*)reference_block_id), 32);
    memcpy(transaction->payer, hex_to_byte((char*)payer), 8);
    transaction->proposer = *proposer;
    transaction->authorizers = authorizers;
    transaction->payload_signatures = payload_signatures;
    transaction->gas_limit = gas_limit;
    transaction->envelope_signatures = listCreate();
    return transaction;
}

void flow_destroy_transaction(flowTransaction* transaction) {
    sdsfree(transaction->script);
    listRelease(transaction->arguments);
    listRelease(transaction->authorizers);
    listRelease(transaction->payload_signatures);
    listRelease(transaction->envelope_signatures);
    free(transaction);
}

flowClient* flow_create_client(char* host, int port){
    flowClient* client = malloc(sizeof(flowClient));
    client->host = host;
    client->port = port;
    return client;
}

void flow_destroy_client(flowClient* client){
    free(client);
}

cJSON* flow_get_latest_block(flowClient* client) {
    sds response = flow_get(client, "/v1/blocks", "height=sealed", "");
    cJSON* result = cJSON_Parse(response);
    sdsfree(response);
    return result;
}

cJSON* flow_get_account(flowClient* client, const char* address) {
    sds path = sdsnew("/v1/accounts/");
    path = sdscatlen(path, address, strlen(address));
    sds response = flow_get(client, path, "expand=keys", "");
    sdsfree(path);
    cJSON* result = cJSON_Parse(response);
    sdsfree(response);
    return result;
}


cJSON* flow_execute_script(flowClient* client, const char* script, ...){
    cJSON* doc = cJSON_CreateObject();
    cJSON_AddStringToObject(doc,"script", base64_encode(script, strlen(script)));

    cJSON* args = cJSON_CreateArray();
    va_list ap;

    va_start(ap, script);

    flowArgument* arg;
    while ((arg = va_arg(ap, flowArgument *)) != NULL) {
        char* encoded = flow_encode_argument(arg);
        sds encodedArg = base64_encode(
                encoded,
                strlen(encoded)
                );
        cJSON_AddItemToArray(args, cJSON_CreateString(encodedArg));
    }

    va_end(ap);

    cJSON_AddItemToObject(doc, "arguments", args);

    char* encodedRequest = cJSON_PrintUnformatted(doc);
    printf("encodedRequest: %s\n", encodedRequest);

    sds response = flow_post(client, "/v1/scripts", "block_height=sealed", encodedRequest);
    sds responseDecoded = base64_decode(&response[1], strlen(response)-2);
    printf("response: %s\n", responseDecoded);
    cJSON* result = cJSON_Parse(responseDecoded);
    sdsfree(responseDecoded);
    sdsfree(response);
    return result;
}


sds signTransactionWithSecureElement(flowTransaction* tx, int keySlot){
    unsigned char signature[64];

    sds payload = prepareEnvelopeWithDomainTag(tx);
    printf("sign\n");
    printf("payload: %s\n", bytes_to_hex((unsigned char*)payload, sdslen(payload)));

    unsigned char hash[32]={0};
    int length = sdslen(payload);
    int rounds = length / 64;
    printf("rounds: %d\n", rounds);
    printf("length: %d\n", length);


    int r = atcab_sha_start();
    printf("sha_start r: %d\n", r);
    for (int i=0;i<rounds; i++){
        r = atcab_sha_update((uint8_t *)payload);
        printf("sha_update r: %d\n", r);
        sdsrange(payload, 64, -1);
    }
    r = atcab_sha_end(hash, sdslen(payload), (uint8_t *)payload);
    printf("hash r: %d\n", r);
    printf("hash: %s\n", bytes_to_hex(hash, 32));
    sdsfree(payload);

    unsigned char pub[64] = {0};
    r = atcab_get_pubkey(keySlot, pub);
    printf("pub r: %s\n", bytes_to_hex(pub,64));

    r = atcab_sign_ext(atcab_get_device(), keySlot, hash, signature);
    printf("sign r: %d\n", r);
    printf("signature: %s\n", bytes_to_hex(signature, 64));
    return sdsnewlen(signature,64);
}

cJSON* flow_send_transaction(flowClient* client, flowTransaction* tx, ...){
    cJSON* doc = cJSON_CreateObject();
    cJSON_AddStringToObject(doc,"script", base64_encode(tx->script, strlen(tx->script)));
    printf("script added\n");

    cJSON* args = cJSON_CreateArray();
    listIter* iter = listGetIterator(tx->arguments, AL_START_HEAD);
    listNode *node;

    while ((node = listNext(iter)) != NULL) {
        flowArgument *argument =  (flowArgument *)listNodeValue(node);
        char* encoded = flow_encode_argument(argument);
        sds encodedArg = base64_encode(
                encoded,
                strlen(encoded)
        );
        free(encoded);
        cJSON_AddItemToArray(args, cJSON_CreateString(encodedArg));
    }
    listReleaseIterator(iter);
    cJSON_AddItemToObject(doc, "arguments", args);
    printf("arguments added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    char str[20];
    cJSON_AddStringToObject(doc,"reference_block_id",bytes_to_hex(tx->reference_block_id,32));
    sprintf(str, "%d", tx->gas_limit);
    cJSON_AddStringToObject( doc, "gas_limit", (char*)&str);

    printf("details added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    cJSON *proposal_key = cJSON_CreateObject();
    sprintf(str, "%llu", tx->proposer.key_index);
    cJSON_AddStringToObject(proposal_key,"key_index", str);
    sprintf(str, "%llu", tx->proposer.sequence_number);
    cJSON_AddStringToObject(proposal_key,"sequence_number", str);
    cJSON_AddStringToObject(proposal_key, "address", bytes_to_hex(tx->proposer.address,8));
    cJSON_AddItemToObject(doc, "proposal_key", proposal_key);
    printf("proposal_key added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));


    cJSON_AddStringToObject(doc,"payer", bytes_to_hex(tx->payer, 8));
    printf("payer added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    cJSON* authorizers = cJSON_CreateArray();
    iter = listGetIterator(tx->authorizers, AL_START_HEAD);
    while ((node = listNext(iter)) != NULL) {
        flowAddress *address =  (flowAddress *)listNodeValue(node);
        cJSON_AddItemToArray(authorizers, cJSON_CreateString(bytes_to_hex(address,8)));
    }
    listReleaseIterator(iter);
    cJSON_AddItemToObject(doc, "authorizers", authorizers);
    printf("authorizers added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    cJSON* payload_signatures = cJSON_CreateArray();
    iter = listGetIterator(tx->payload_signatures, AL_START_HEAD);
    while ((node = listNext(iter)) != NULL) {
        flowSignature *signature =  (flowSignature *)listNodeValue(node);
        cJSON* signature_json = cJSON_CreateObject();
        cJSON_AddStringToObject(signature_json,"address", bytes_to_hex(signature->address,8));
        sprintf(str, "%llu", signature->key_index);
        cJSON_AddStringToObject(signature_json,"key_index", str);
        cJSON_AddStringToObject(signature_json,"signature", base64_encode((char*)signature->signature,64));
        cJSON_AddItemToArray(payload_signatures, signature_json);
    }
    listReleaseIterator(iter);
    cJSON_AddItemToObject(doc, "payload_signatures", payload_signatures);
    printf("payload_signatures added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    cJSON* envelope_signatures = cJSON_CreateArray();
    iter = listGetIterator(tx->envelope_signatures, AL_START_HEAD);
    while ((node = listNext(iter)) != NULL) {
        flowSignature *signature =  (flowSignature *)listNodeValue(node);
        cJSON* signature_json = cJSON_CreateObject();
        cJSON_AddStringToObject(signature_json, "address", bytes_to_hex(signature->address,8));
        sprintf(str, "%llu", signature->key_index);
        cJSON_AddStringToObject(signature_json, "key_index", str);
        cJSON_AddStringToObject(signature_json, "signature", base64_encode((char*)signature->signature,64));
        cJSON_AddItemToArray(envelope_signatures, signature_json);
    }
    cJSON_AddItemToObject(doc, "envelope_signatures", envelope_signatures);
    printf("envelopeSignatures added\n");
    printf("json: %s\n", cJSON_PrintUnformatted(doc));

    char* encodedRequest = cJSON_PrintUnformatted(doc);
    //cJSON_free(doc)
    printf("encodedRequest: %s\n", encodedRequest);

    sds response = flow_post(client, "/v1/transactions", "", encodedRequest);
    printf("response: %s\n", response);
    cJSON* result = cJSON_Parse(response);
    sdsfree(response);
    cJSON_free(doc);

    return result;
}

sds flow_get(flowClient* client,  const char* path,  const char* query, const char* data){
    return http_request(client->host, client->port, HTTP_METHOD_GET, path, query, data);
}

sds flow_post(flowClient* client, const char* path,  const char* query, const char* data){
    return http_request(client->host, client->port, HTTP_METHOD_POST, path, query, data);
}



