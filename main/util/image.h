#include "sds.h"
#include "lvgl/lvgl.h"
#include "sdsbase64.h"

#ifndef TINYWALLET_IMAGE_H
#define TINYWALLET_IMAGE_H

lv_img_dsc_t* image_from_base64(char* base64encoded, size_t width, size_t height);


#endif //TINYWALLET_IMAGE_H
