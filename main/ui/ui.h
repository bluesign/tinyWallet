#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H
#include "lvgl/lvgl.h"
#include "ui_comp_coin.h"
#include "sds.h"

#ifdef __cplusplus
extern "C" {
#endif

extern sds myAddress;

// SCREEN: ui_Screen1
void ui_Screen(void);
extern lv_obj_t *keyboard;

extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_TabView1;
extern lv_obj_t *ui_pageHome;
extern lv_obj_t *ui_coin;
extern lv_obj_t *ui_Image5;
extern lv_obj_t *ui_panelAccount;
extern lv_obj_t *ui_labelAddress;
extern lv_obj_t *ui_coin1;
extern lv_obj_t *ui_Image6;
extern lv_obj_t *ui_buttonSendFT;
extern lv_obj_t *ui_labelSendFT;
extern lv_obj_t *ui_labelFindName;
extern lv_obj_t *ui_pageCollection;
extern lv_obj_t *ui_Image3;
extern lv_obj_t *ui_pageStaking;
extern lv_obj_t *ui_pageSettings;
extern lv_obj_t *ui____initial_actions0;

extern lv_obj_t *ftList;
extern lv_obj_t *nftList;
extern lv_obj_t *nftItemList;


extern lv_obj_t *settingWiFiSwitch;
extern lv_obj_t *wfList;
extern lv_obj_t *apSettings;
extern lv_obj_t *apSettingsCloseBtn;

extern lv_obj_t *mboxConnect;
extern lv_obj_t *mboxPassword;
extern lv_obj_t *mboxConnectBtn;
extern lv_obj_t *mboxCloseBtn;

extern lv_obj_t *brandLabel;

extern lv_obj_t *batteryIcon;
extern lv_obj_t *wifiIcon;


void get_component_child_event_cb(lv_event_t* e);
void del_component_child_event_cb(lv_event_t* e);

lv_obj_t * ui_comp_get_child(lv_obj_t *comp, uint32_t child_idx);
extern uint32_t LV_EVENT_GET_COMP_CHILD;

extern void btn_event_cb(lv_event_t *e);
extern void text_input_event_cb(lv_event_t *e);


void ui_init(void);
lv_obj_t* ui_msgbox(const char* title, const char* message, const char * buttons[], lv_event_cb_t callback );
lv_obj_t* ui_spinner();


#endif
