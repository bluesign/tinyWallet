#include "ui.h"

lv_obj_t * ui_coin_create(lv_obj_t *comp_parent) {

    lv_obj_t *cui_coin;
    cui_coin = lv_obj_create(comp_parent);
    lv_obj_set_width(cui_coin, lv_pct(100));
    lv_obj_set_height(cui_coin, LV_SIZE_CONTENT);
    lv_obj_set_x(cui_coin, 0);
    lv_obj_set_y(cui_coin, 0);
    lv_obj_set_align(cui_coin, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(cui_coin, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_style_radius(cui_coin, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(cui_coin, 5, LV_PART_MAIN);
    lv_obj_set_style_border_width(cui_coin, 1, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cui_coin, 0x80, LV_PART_MAIN);

    lv_obj_clear_flag(cui_coin, LV_OBJ_FLAG_SCROLLABLE);    /// Flags


    ui_Image5 = lv_img_create(cui_coin);
    lv_obj_set_width( ui_Image5, 40);
    lv_obj_set_height( ui_Image5, 40);
    lv_obj_set_x( ui_Image5, 4 );
    lv_obj_set_y( ui_Image5, 5 );
    lv_obj_set_align( ui_Image5, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag( ui_Image5, LV_OBJ_FLAG_SCROLLABLE );    /// Flags


    lv_obj_t *cui_name;
    cui_name = lv_label_create(cui_coin);
    lv_obj_set_width(cui_name, 210);  /// 1
    lv_obj_set_height(cui_name, 25);   /// 1
    lv_obj_set_x(cui_name, 55);
    lv_obj_set_y(cui_name, 4);
    lv_obj_set_align(cui_name, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_color(cui_name, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_name, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(cui_name, LV_LABEL_LONG_DOT);

    lv_obj_t *cui_desription_price;
    cui_desription_price = lv_label_create(cui_coin);
    lv_obj_set_width(cui_desription_price, 210);  /// 1
    lv_obj_set_height(cui_desription_price, 35);   /// 1
    lv_obj_set_x(cui_desription_price, 55);
    lv_obj_set_y(cui_desription_price, 27);
    lv_obj_set_align(cui_desription_price, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_color(cui_desription_price, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_desription_price, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_desription_price, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(cui_desription_price, LV_LABEL_LONG_WRAP);


    lv_obj_t *cui_count;
    cui_count = lv_label_create(cui_coin);
    lv_obj_set_width(cui_count, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(cui_count, 25);   /// 1
    lv_obj_set_x(cui_count, -10);
    lv_obj_set_y(cui_count, 4);
    lv_obj_set_align(cui_count, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_text_align(cui_count, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_count, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_count, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cui_items_value;
    cui_items_value = lv_label_create(cui_coin);
    lv_obj_set_width(cui_items_value, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(cui_items_value, 25);   /// 1
    lv_obj_set_x(cui_items_value, -10);
    lv_obj_set_y(cui_items_value, 27);
    lv_obj_set_align(cui_items_value, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_text_align(cui_items_value, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cui_items_value, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(cui_items_value, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cui_items_value, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t **children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_COIN_NUM);
    children[UI_COMP_COIN_COIN] = cui_coin;
    children[UI_COMP_COIN_NAME] = cui_name;
    children[UI_COMP_COIN_DESCRIPTION_PRICE] = cui_desription_price;
    children[UI_COMP_COIN_COUNT] = cui_count;
    children[UI_COMP_COIN_ITEMS_VALUE] = cui_items_value;
    children[UI_COMP_COIN_IMAGE] = ui_Image5;

    lv_obj_add_event_cb(cui_coin, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_coin, del_component_child_event_cb, LV_EVENT_DELETE, children);

    return cui_coin;
}

