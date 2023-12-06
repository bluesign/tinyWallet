#include "ui.h"
#include "sds.h"



lv_obj_t *keyboard;

lv_obj_t *ui_Screen1;
lv_obj_t *ui_TabView1;
lv_obj_t *ui_pageHome;
lv_obj_t *ui_coin;
lv_obj_t *ui_Image5;
lv_obj_t *ui_panelAccount;
lv_obj_t *ui_labelAddress;
lv_obj_t *ui_coin1;
lv_obj_t *ui_Image6;
lv_obj_t *ui_buttonSendFT;
lv_obj_t *ui_labelSendFT;
lv_obj_t *ui_labelFindName;
lv_obj_t *ui_pageCollection;
lv_obj_t *ui_Image3;
lv_obj_t *ui_pageStaking;
lv_obj_t *ui_pageSettings;
lv_obj_t *ui____initial_actions0;

lv_obj_t *ftList;
lv_obj_t *nftList;

lv_obj_t *nftItemList;


lv_obj_t *settingWiFiSwitch;
lv_obj_t *wfList;
lv_obj_t *apSettings;
lv_obj_t *apSettingsCloseBtn;

lv_obj_t *mboxConnect;
lv_obj_t *mboxPassword;
lv_obj_t *mboxConnectBtn;
lv_obj_t *mboxCloseBtn;

lv_obj_t *brandLabel;
lv_obj_t *batteryIcon;
lv_obj_t *wifiIcon;

uint32_t LV_EVENT_GET_COMP_CHILD;

typedef struct {
    uint32_t child_idx;
    lv_obj_t* child;
} ui_comp_get_child_t;

lv_obj_t * ui_comp_get_child(lv_obj_t *comp, uint32_t child_idx) {
    ui_comp_get_child_t info;
    info.child = NULL;
    info.child_idx = child_idx;
    lv_event_send(comp, LV_EVENT_GET_COMP_CHILD, &info);
    return info.child;
}

void get_component_child_event_cb(lv_event_t* e) {
    lv_obj_t** c = lv_event_get_user_data(e);
    ui_comp_get_child_t* info = lv_event_get_param(e);
    info->child = c[info->child_idx];
}

void del_component_child_event_cb(lv_event_t* e) {
    lv_obj_t** c = lv_event_get_user_data(e);
    lv_mem_free(c);
}


void ui_init( void ){
    LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr( ui_Screen1);
}

void ui_msgbox(const char* title, const char* message, const char * buttons[], lv_event_cb_t callback )
{
    lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), title, message, buttons, false);
    lv_obj_add_event_cb(mbox1, callback, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox1);
}

lv_obj_t* ui_spinner() {
    lv_obj_t *spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
    return spinner;
}


