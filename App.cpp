
#include "App.h"

using namespace std;


void def_show_app() {
    lv_obj_t* main_label = lv_label_create(lv_scr_act());
    lv_label_set_text(main_label, "unimplemented show function pointer");

}

App::App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc)
{                                       
	screen = scr;
	name = Name;
	trig_btn = btn;
	img = img_dsc;
    show = &def_show_app;
    runState = state::notrunning;
    backgroundRefresh = NULL;
    foregroundRefresh = NULL;
    bg_exec_period = 5;
    fg_exec_period = 5;

}

App::App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc, void(*show_func)())
{
    screen = scr;
    name = Name;
    trig_btn = btn;
    img = img_dsc;
    show = show_func;
    runState = state::notrunning;
    backgroundRefresh = NULL;
    foregroundRefresh = NULL;
    bg_exec_period = 5;
    fg_exec_period = 5;

}


App::App(lv_obj_t* scr, string Name, lv_obj_t* btn, const lv_img_dsc_t* img_dsc, void(*show_func)(), void(*br_func)())
{
    screen = scr;
    name = Name;
    trig_btn = btn;
    img = img_dsc;
    show = show_func;
    runState = state::notrunning;
    backgroundRefresh = br_func;
    foregroundRefresh = NULL;
    bg_exec_period = 5;
    fg_exec_period = 5;

}

App::App() {
	LV_IMG_DECLARE(Spotify_Icon_RGB_Green);
	screen = NULL;
	name = (string) "Hi";
	trig_btn = NULL;
	//img = &Spotify_Icon_RGB_Green;
	img = NULL;
    show = &def_show_app;
    runState = state::notrunning;
    backgroundRefresh = NULL;
    foregroundRefresh = NULL;
    bg_exec_period = 5;
    fg_exec_period = 5;
    
}







