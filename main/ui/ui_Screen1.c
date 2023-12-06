#include "ui.h"

#define TAB_HEIGHT 40
#define STATUS_HEIGHT 25


void ui_Screen(void) {

    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_style_radius(ui_Screen1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_Screen1, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_Screen1, 0, LV_PART_MAIN);

    keyboard = lv_keyboard_create(ui_Screen1);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *statusBar = lv_obj_create(ui_Screen1);
    lv_obj_set_size(statusBar, 320, STATUS_HEIGHT);
    lv_obj_clear_flag(statusBar, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_style_radius(statusBar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(statusBar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    lv_obj_remove_style(statusBar, NULL, LV_PART_SCROLLBAR | LV_STATE_ANY);
    lv_obj_set_y(statusBar, 240-STATUS_HEIGHT);
    lv_obj_set_x(statusBar, 0);
    lv_obj_set_style_border_width(statusBar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(statusBar, 0, LV_PART_MAIN);


    brandLabel = lv_label_create(statusBar);
    lv_obj_set_size(brandLabel, 320, STATUS_HEIGHT);
    lv_label_set_text(brandLabel, "tinyWallet");
    lv_obj_set_style_text_color(brandLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(brandLabel, LV_ALIGN_LEFT_MID, 5, 5);
    lv_obj_set_style_text_font(brandLabel, &lv_font_montserrat_16, LV_PART_MAIN| LV_STATE_DEFAULT);

    batteryIcon = lv_label_create(statusBar);
    lv_obj_set_size(batteryIcon,  LV_SIZE_CONTENT, STATUS_HEIGHT);
    lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_FULL);
    lv_obj_align(batteryIcon, LV_ALIGN_RIGHT_MID, -5, 5);
    lv_obj_set_style_text_color(batteryIcon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    wifiIcon = lv_label_create(statusBar);
    lv_obj_set_size(wifiIcon,  LV_SIZE_CONTENT, STATUS_HEIGHT);
    lv_label_set_text(wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_align_to(wifiIcon, batteryIcon, LV_ALIGN_LEFT_MID, -25, 0);
    lv_obj_set_style_text_color(wifiIcon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(wifiIcon,  0x90, LV_PART_MAIN | LV_STATE_DEFAULT );

    ui_TabView1 = lv_tabview_create(ui_Screen1, LV_DIR_TOP, TAB_HEIGHT);
    lv_obj_set_width(ui_TabView1, 320);
    lv_obj_set_height(ui_TabView1, 240 - STATUS_HEIGHT);
    lv_obj_set_y(ui_TabView1, 0);
    lv_obj_set_align(ui_TabView1, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(ui_TabView1, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_pageHome = lv_tabview_add_tab(ui_TabView1, LV_SYMBOL_HOME " Home");
    lv_obj_set_style_shadow_color(ui_pageHome, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_pageHome, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_clear_flag( ui_pageHome, LV_OBJ_FLAG_SCROLLABLE );
    lv_obj_set_style_pad_all(ui_pageHome, 0, LV_PART_MAIN);

    ui_panelAccount = lv_obj_create(ui_pageHome);
    lv_obj_set_width(ui_panelAccount, 320);
    lv_obj_set_height(ui_panelAccount, 70);
    lv_obj_set_x(ui_panelAccount, 0);
    lv_obj_set_y(ui_panelAccount, 10);
    lv_obj_set_align(ui_panelAccount, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(ui_panelAccount, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_style_shadow_color(ui_panelAccount, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_panelAccount, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_labelAddress = lv_label_create(ui_panelAccount);
    lv_obj_set_width(ui_labelAddress, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_labelAddress, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_labelAddress, 0);
    lv_obj_set_y(ui_labelAddress, 0);
    lv_obj_set_align(ui_labelAddress, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_labelAddress, myAddress);

    ui_labelFindName = lv_label_create(ui_panelAccount);
    lv_obj_set_width(ui_labelFindName, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_labelFindName, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_labelFindName, 0);
    lv_obj_set_y(ui_labelFindName, 24);
    lv_obj_set_align(ui_labelFindName, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_text_color(ui_labelFindName, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_labelFindName, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_labelFindName, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_labelFindName, "");

    ui_buttonSendFT = lv_btn_create(ui_panelAccount);
    lv_obj_set_width(ui_buttonSendFT, 80);
    lv_obj_set_height(ui_buttonSendFT, 24);
    lv_obj_set_x(ui_buttonSendFT, 0);
    lv_obj_set_y(ui_buttonSendFT, 0);
    lv_obj_set_align(ui_buttonSendFT, LV_ALIGN_RIGHT_MID);
    lv_obj_add_flag(ui_buttonSendFT, LV_OBJ_FLAG_SCROLL_ON_FOCUS);   /// Flags
    lv_obj_clear_flag(ui_buttonSendFT, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_labelSendFT = lv_label_create(ui_buttonSendFT);
    lv_label_set_text(ui_labelSendFT, "Send FT");
    lv_obj_set_align(ui_labelSendFT, LV_ALIGN_CENTER);

    ftList = lv_list_create(ui_pageHome);
    lv_obj_set_size(ftList, 320, LV_SIZE_CONTENT);
    lv_obj_align_to(ftList, ui_pageHome, LV_ALIGN_TOP_LEFT, 0, 90);
    lv_obj_set_style_pad_all(ftList, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ftList, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    ui_pageCollection = lv_tabview_add_tab(ui_TabView1, LV_SYMBOL_IMAGE " Collection");
    lv_obj_set_style_pad_all(ui_pageCollection, 0, LV_PART_MAIN);

    nftList = lv_list_create(ui_pageCollection);
    lv_obj_set_size(nftList, 320, LV_SIZE_CONTENT);
    lv_obj_align_to(nftList, ui_pageCollection, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_pad_all(nftList, 0, LV_PART_MAIN);
    lv_obj_clear_flag(ftList, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    nftItemList = lv_list_create(ui_pageCollection);
    lv_obj_set_size(nftItemList, 320, LV_SIZE_CONTENT);
    lv_obj_align_to(nftItemList, ui_pageCollection, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_pad_all(nftItemList, 0, LV_PART_MAIN);
    lv_obj_clear_flag(nftItemList, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_add_flag(nftItemList, LV_OBJ_FLAG_HIDDEN);    /// Flags


    //ui_pageStaking = lv_tabview_add_tab(ui_TabView1, "Staking");

    ui_pageSettings = lv_tabview_add_tab(ui_TabView1, LV_SYMBOL_SETTINGS " Settings");

    lv_obj_t *ui_labelWifi = lv_label_create(ui_pageSettings);
    lv_label_set_text(ui_labelWifi, "Wi-Fi: ");
    lv_obj_set_align(ui_labelWifi, LV_ALIGN_LEFT_MID);
    lv_obj_set_width(ui_labelWifi, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_labelWifi, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_labelWifi, 0);
    lv_obj_set_y(ui_labelWifi, 0);
    lv_obj_set_align(ui_labelWifi, LV_ALIGN_TOP_LEFT);

    settingWiFiSwitch = lv_switch_create(ui_pageSettings);
    lv_obj_add_event_cb(settingWiFiSwitch, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align_to(settingWiFiSwitch, ui_labelWifi, LV_ALIGN_RIGHT_MID, 60, 0);

    apSettings = lv_obj_create(ui_Screen1);
    lv_obj_set_size(apSettings, 320-50, 210);
    lv_obj_align(apSettings, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_clear_flag(apSettings, LV_OBJ_FLAG_SCROLLABLE);    /// Flags

    lv_obj_t *settinglabel = lv_label_create(apSettings);
    lv_label_set_text(settinglabel, "Select Access Point");
    lv_obj_align(settinglabel, LV_ALIGN_TOP_LEFT, 0, 10);

    apSettingsCloseBtn = lv_btn_create(apSettings);
    lv_obj_set_size(apSettingsCloseBtn, 30, 30);
    lv_obj_align(apSettingsCloseBtn, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(apSettingsCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_t *btnSymbol = lv_label_create(apSettingsCloseBtn);
    lv_label_set_text(btnSymbol, LV_SYMBOL_CLOSE);
    lv_obj_center(btnSymbol);

    wfList = lv_list_create(apSettings);
    lv_obj_set_size(wfList, 320 - 80, 140);
    lv_obj_align_to(wfList, settinglabel, LV_ALIGN_TOP_LEFT, 0, 30);
    lv_obj_set_style_pad_all(wfList, 0, LV_PART_MAIN);

    lv_obj_add_flag(apSettings, LV_OBJ_FLAG_HIDDEN);

    mboxConnect = lv_obj_create(ui_Screen1);
    lv_obj_set_size(mboxConnect, 320-100, 130);
    lv_obj_align(mboxConnect, LV_ALIGN_CENTER, 0, -50);

    lv_obj_t *mboxTitle = lv_label_create(mboxConnect);
    lv_label_set_text(mboxTitle, "Enter password for Wi-Fi");
    lv_obj_align(mboxTitle, LV_ALIGN_TOP_LEFT, 0, 0);

    mboxPassword = lv_textarea_create(mboxConnect);
    lv_obj_clear_flag(mboxPassword, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_size(mboxPassword, 180, 35);
    lv_obj_align_to(mboxPassword, mboxTitle, LV_ALIGN_TOP_LEFT, 0, 20);
    lv_textarea_set_placeholder_text(mboxPassword, "Password?");
    lv_obj_add_event_cb(mboxPassword, text_input_event_cb, LV_EVENT_ALL, keyboard);

    mboxConnectBtn = lv_btn_create(mboxConnect);
    lv_obj_add_event_cb(mboxConnectBtn, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(mboxConnectBtn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_size(mboxConnectBtn, 80, 30);
    lv_obj_t *btnLabel = lv_label_create(mboxConnectBtn);
    lv_label_set_text(btnLabel, "Connect");
    lv_obj_center(btnLabel);

    mboxCloseBtn = lv_btn_create(mboxConnect);
    lv_obj_add_event_cb(mboxCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(mboxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_size(mboxCloseBtn, 80, 30);
    lv_obj_t *btnLabel2 = lv_label_create(mboxCloseBtn);
    lv_label_set_text(btnLabel2, "Cancel");
    lv_obj_center(btnLabel2);

    lv_obj_add_flag(mboxConnect, LV_OBJ_FLAG_HIDDEN);

}
