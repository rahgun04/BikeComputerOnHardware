#pragma once
#include "lvgl/lvgl.h"
#include "App.h"
#include <iostream>
#include <ctime>
using namespace std;
LV_C_IMG_DECLARE(topBar);

class DisplayManager {
public: 
	DisplayManager();
	int add_app(App app);
	void hamburger_menu();
    void hamburger_hide();
    bool ham_shown;
	App apps[7];
	int app_index;
	lv_obj_t* list1;
	lv_obj_t* imgbtns[7];
    void ham_press_evt_hndlr(lv_event_t* e);
    void show();
    void ham_show_evt_hndlr(lv_event_t* e);
    int findAppIndex(string name);
    void loadApp(int index);
    void br_runner();
    void fg_runner();
    
private:
    int itterCount;
    lv_anim_t ham_anim;
    time_t now;
    int current_app_index;
    lv_obj_t* timeLabel;
    
    
};
