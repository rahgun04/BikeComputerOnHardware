#pragma once
#include "lvgl/lvgl.h"
#include <string>



using namespace std;


enum class state {
    notrunning,
    running,
};

class App {
public:
	lv_obj_t* screen;

	string name;
	lv_obj_t* trig_btn;
	const lv_img_dsc_t* img;
    void (*show)();
    state runState;
	App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc);
    App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc, void (*show)());
	App();

    App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc, void(*show_func)(), void(*br_func)());
    void (*backgroundRefresh)();
    void (*foregroundRefresh)();

    int bg_exec_period;
    int bg_exec_count;

    int fg_exec_period;
    int fg_exec_count;

private:
    
    
};
