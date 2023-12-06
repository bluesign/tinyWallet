file(GLOB_RECURSE SOURCES ./main *.c)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS  "ui" "network" "util"
        REQUIRES
            esp_wifi
            nvs_flash
            esp_event
            esp-tls
            esp_http_client
            fatfs
            core2forAWS
            esp-cryptoauthlib
            core2forAWS
            spiffs
)

spiffs_create_partition_image(spiffs data FLASH_IN_PROJECT)

idf_build_set_property(COMPILE_OPTIONS "-DLV_DRAW_COMPLEX=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-I../main" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-O1" APPEND)
