#include "MainMenu.h"
#include "lvgl/lvgl.h"
//#include "Header.h"
#include "DisplayManager.h"
#include "App.h"

extern DisplayManager disp_man;
state rideRunning;
lv_obj_t* beginLabel;

static void launch_ride(lv_event_t* e) {
    disp_man.loadApp(disp_man.findAppIndex(std::string("Ride")));

}


void mainMenu_fg_exec() {
    state rideState = disp_man.apps[disp_man.findAppIndex("Ride")].runState;
    if (rideRunning != rideState) {
        if (state::running == rideState) {
            lv_label_set_text(beginLabel, "Continue Ride");
        }
        else {
            lv_label_set_text(beginLabel, "Begin Ride");
        }
        rideRunning = rideState;
    }
}



void show_mainMenu() {
    rideRunning = state::notrunning;
    lv_obj_t* btn1 = lv_btn_create(lv_scr_act());
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -20);

    beginLabel = lv_label_create(btn1);
    lv_label_set_text(beginLabel, "Begin");
    lv_obj_add_event_cb(btn1, launch_ride, LV_EVENT_PRESSED, NULL);
    lv_obj_center(beginLabel);
    lv_obj_set_size(btn1, 100, 65);

    
    btn1 = lv_btn_create(lv_scr_act());
    lv_obj_t* label;
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 100);
    label = lv_label_create(btn1);
    lv_label_set_text(label, "Turn Off");
    lv_obj_center(label);
    lv_obj_set_size(btn1, 100, 25);






}

