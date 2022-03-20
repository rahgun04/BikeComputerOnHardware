#include "lvgl/lvgl.h"
#include "Ride.h"
#include "Speedometer.h"
#include <string>
#include <chrono>
#include "DisplayManager.h"


using namespace std;

LV_C_IMG_DECLARE(rider);
LV_C_IMG_DECLARE(leaf);

extern Spedo spdo;
extern DisplayManager disp_man;
lv_obj_t* speed;
lv_obj_t* rideTimeDisp;

lv_obj_t* ecoStat;
lv_obj_t* ecoImg;

lv_draw_arc_dsc_t arcDat;
lv_obj_t* canvas;

lv_color_t white;

double ecoGrams;


enum dists {
    near,
    mid,
    far,
};


static void end_ride(lv_event_t* e) {

    disp_man.apps[disp_man.findAppIndex(std::string("Ride"))].runState = state::notrunning;
    disp_man.loadApp(disp_man.findAppIndex(std::string("MainMenu")));
    lv_obj_del(disp_man.apps[disp_man.findAppIndex(std::string("Ride"))].screen);
    disp_man.apps[disp_man.findAppIndex(std::string("Ride"))].screen = lv_obj_create(NULL);

}


void show_Ride() {

    /*
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);


    static lv_point_t line_points[] = { {5, 5}, {70, 70},
                                {120, 10}, {180, 60}, {240, 10} };


    lv_obj_t* line1;
    line1 = lv_line_create(lv_scr_act());
    lv_line_set_points(line1, line_points, 5);
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_center(line1);
    */
    white = lv_color_make(255, 255, 255);
    canvas = lv_canvas_create(disp_man.apps[disp_man.findAppIndex("Ride")].screen);
    const int width = 200;
    const int height = 200;
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(width, height)];
    lv_canvas_set_buffer(canvas, cbuf, width, height, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);


    lv_draw_img_dsc_t imgDat;
    lv_draw_img_dsc_init(&imgDat);


    lv_canvas_draw_img(canvas, 0, 0, &rider, &imgDat);


    lv_draw_arc_dsc_init(&arcDat);
    arcDat.width = 5;
    arcDat.opa = LV_OPA_90;
    arcDat.rounded = 40;

    arcDat.color = lv_palette_main(LV_PALETTE_RED);

    lv_canvas_draw_arc(canvas, 80, 154, 15, 20, 90, &arcDat);


    arcDat.color = lv_palette_main(LV_PALETTE_AMBER);
    lv_canvas_draw_arc(canvas, 80, 154, 30, 0, 90, &arcDat);



    arcDat.color = lv_palette_main(LV_PALETTE_GREEN);
    lv_canvas_draw_arc(canvas, 80, 154, 45, 0, 90, &arcDat);


    lv_obj_t* label;
    lv_obj_t* btn1 = lv_btn_create(disp_man.apps[disp_man.findAppIndex("Ride")].screen);
    lv_obj_add_event_cb(btn1, end_ride, LV_EVENT_PRESSED, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 110);
    lv_obj_set_height(btn1, 40);
    label = lv_label_create(btn1);
    lv_label_set_text(label, "End Session");
    lv_obj_center(label);

    /*
        lv_theme_t * theme =  lv_theme_default_get();
        theme->color_primary = lv_palette_main(LV_PALETTE_GREY);
        theme->color_secondary = lv_palette_main(LV_PALETTE_BLUE);
        lv_theme_apply(btn1);
    */
    lv_style_t style;
    lv_style_init(&style);

    lv_obj_add_style(btn1, &style, LV_IMGBTN_STATE_PRESSED);

    speed = lv_label_create(disp_man.apps[disp_man.findAppIndex("Ride")].screen);
    lv_obj_align(speed, LV_ALIGN_TOP_LEFT, 120, 80);

    spdo.beginTime = Clock::now();
    spdo.rotCount = 0;
    rideTimeDisp = lv_label_create(disp_man.apps[disp_man.findAppIndex("Ride")].screen);
    lv_obj_align(rideTimeDisp, LV_ALIGN_TOP_LEFT, 120, 200);


    ecoStat = lv_label_create(lv_scr_act());
    lv_label_set_text(ecoStat, "0.00 g");
    lv_obj_set_pos(ecoStat, 110, 160);

    ecoImg = lv_img_create(lv_scr_act());
    lv_img_set_src(ecoImg, &leaf);

    lv_obj_set_pos(ecoImg, 110, 100);


}


void distanceGraph(dists dist) {
    switch (dist) {
    case dists::near:
        arcDat.color = lv_palette_main(LV_PALETTE_RED);
        lv_canvas_draw_arc(canvas, 80, 154, 15, 20, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 30, 0, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 45, 0, 90, &arcDat);
        break;
    case dists::mid:
        arcDat.color = lv_palette_main(LV_PALETTE_AMBER);
        lv_canvas_draw_arc(canvas, 80, 154, 15, 20, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 30, 0, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 45, 0, 90, &arcDat);
        break;
    case dists::far:
        arcDat.color = lv_palette_main(LV_PALETTE_GREEN);;
        lv_canvas_draw_arc(canvas, 80, 154, 15, 20, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 30, 0, 90, &arcDat);
        lv_canvas_draw_arc(canvas, 80, 154, 45, 0, 90, &arcDat);
        break;
    }
}


void speedUpdate() {
    spdo.rideTime = Clock::now() - spdo.beginTime;
    int n = (int)spdo.rideTime.count();


    spdo.hoursRide = n / 3600;

    n %= 3600;
    spdo.minutesRide = n / 60;

    n %= 60;
    spdo.secondsRide = n;


    std::string speedStr = std::to_string(spdo.speed);
    lv_label_set_text(speed, speedStr.c_str());
    string hStr = std::to_string(spdo.hoursRide);
    string mStr = std::to_string(spdo.minutesRide);
    string sStr = std::to_string(spdo.secondsRide);
    string conca = hStr + " : " + mStr + " : " + sStr;
    lv_label_set_text(rideTimeDisp, conca.c_str());

    ecoGrams += 0.01;
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "%.1fg Saved", ecoGrams);

    lv_label_set_text(ecoStat, buffer);
    float dist = distanceRetreive();
    dists distGrade = dists::near;
    if (dist > 1000) {
        distGrade = dists::far;
    }
    else if (dist > 1000) {
        distGrade = dists::mid;
    }
    distanceGraph(distGrade);
}


void speedBgUpdate() {

}



