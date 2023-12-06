# ESP-IDF component file for make based commands
#
# Component Makefile
#

# Set simple includes as default
ifndef LV_CONF_INCLUDE_SIMPLE
CFLAGS += -DLV_CONF_INCLUDE_SIMPLE
endif

COMPONENT_SRCDIRS := . \
                  lvgl/src \
                  lvgl/src/core \
                  lvgl/src/draw \
                  lvgl/src/extra \
                  lvgl/src/font \
                  lvgl/src/gpu \
                  lvgl/src/hal \
                  lvgl/src/misc \
                  lvgl/src/widgets \
                  lvgl/src/extra/layouts \
                  lvgl/src/extra/layouts/flex \
                  lvgl/src/extra/layouts/grid \
                  lvgl/src/extra/themes \
                  lvgl/src/extra/themes/basic \
                  lvgl/src/extra/themes/default \
                  lvgl/src/extra/widgets/calendar \
                  lvgl/src/extra/widgets/colorwheel \
                  lvgl/src/extra/widgets \
                  lvgl/src/extra/widgets/imgbtn \
                  lvgl/src/extra/widgets/keyboard \
                  lvgl/src/extra/widgets/led \
                  lvgl/src/extra/widgets/list \
                  lvgl/src/extra/widgets/msgbox \
                  lvgl/src/extra/widgets/spinbox \
                  lvgl/src/extra/widgets/spinner \
                  lvgl/src/extra/widgets/tabview \
                  lvgl/src/extra/widgets/tileview \
                  lvgl/src/extra/widgets/win


COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .
