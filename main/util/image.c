#include "image.h"

lv_img_dsc_t* image_from_base64(char* base64encoded, size_t width, size_t height){
    sds code = sds_base64_decode(base64encoded, strlen(base64encoded));
    size_t data_size = sdslen(code);
    void *data = malloc(data_size);
    memcpy(data, code, data_size);
    sdsfree(code);

    lv_img_dsc_t *dscImage = malloc(sizeof(lv_img_dsc_t));
    dscImage->header.always_zero = 0;
    dscImage->header.w = width;
    dscImage->header.h = height;
    dscImage->header.cf = LV_IMG_CF_RAW_ALPHA;
    dscImage->data = (uint8_t *) data;
    dscImage->data_size = data_size;
    return dscImage;
}