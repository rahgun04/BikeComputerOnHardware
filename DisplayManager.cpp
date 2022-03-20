#include "DisplayManager.h"


using namespace std;


DisplayManager::DisplayManager()
{
    app_index = 0;
    ham_shown = false;
    now = time(0);
    itterCount = 0;


}




int DisplayManager::add_app(App app)
{
    if (app_index < 7) {
        apps[app_index] = app;
        app_index++;
        return 0;
    }
    return 1;

}

static void evt_handler(lv_event_t* e)
{


    DisplayManager* dm;
    dm = (DisplayManager*)e->user_data;
    dm->ham_press_evt_hndlr(e);



}
static void ham_toggle_evt_handler(lv_event_t* e) {
    DisplayManager* dm;
    dm = (DisplayManager*)e->user_data;
    dm->ham_show_evt_hndlr(e);
}

void DisplayManager::hamburger_menu()
{
    ham_shown = true;
    /*Create a list*/
    list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 90, 220);
    lv_obj_align(list1, LV_ALIGN_TOP_LEFT, -80, 75);
    for (int i = 0; i++, i < app_index;) {

        lv_list_add_text(list1, apps[i].name.c_str());
        /*Create an image button*/
        imgbtns[i] = lv_imgbtn_create(list1);
        lv_imgbtn_set_src(imgbtns[i], LV_IMGBTN_STATE_RELEASED, apps[i].img, NULL, NULL);

        lv_obj_add_event_cb(imgbtns[i], evt_handler, LV_EVENT_CLICKED, this); //this is the pointer to the instance of teh class
    }
    lv_anim_init(&ham_anim);

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(&ham_anim, (lv_anim_exec_xcb_t)lv_obj_set_x);

    /*Set target of the animation*/
    lv_anim_set_var(&ham_anim, list1);

    /*Length of the animation [ms]*/
    lv_anim_set_time(&ham_anim, 400);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(&ham_anim, -80, 5);

    lv_anim_set_path_cb(&ham_anim, lv_anim_path_ease_in_out);

    /* START THE ANIMATION
     *------------------*/
    lv_anim_start(&ham_anim);                             /*Start the animation*/
}

void DisplayManager::hamburger_hide()
{
    ham_shown = false;
    /*Create a list*/
    lv_obj_move_foreground(list1);



    lv_anim_init(&ham_anim);

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(&ham_anim, (lv_anim_exec_xcb_t)lv_obj_set_x);

    /*Set target of the animation*/
    lv_anim_set_var(&ham_anim, list1);

    /*Length of the animation [ms]*/
    lv_anim_set_time(&ham_anim, 400);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(&ham_anim, 5, -100);

    lv_anim_set_path_cb(&ham_anim, lv_anim_path_ease_in_out);

    /* START THE ANIMATION
     *------------------*/
    lv_anim_start(&ham_anim);                             /*Start the animation*/
}

void DisplayManager::ham_press_evt_hndlr(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        for (int j = 0; j < 7; j++) {
            if (imgbtns[j] == obj) {
                std::cout << "App: " << apps[j].name << std::endl;
                current_app_index = j;

                lv_scr_load(apps[j].screen);
                hamburger_hide();

                if (apps[current_app_index].runState == state::notrunning) {
                    apps[j].show();
                }
                show(); //display manager stuff
                lv_obj_move_foreground(list1);
            }
        }
    }
}

void DisplayManager::show()
{
    char* dt;
    tm* ltm;
    if (apps[current_app_index].runState == state::notrunning) {

        lv_obj_t* img1 = lv_img_create(lv_scr_act());
        lv_img_set_src(img1, &topBar);

        lv_obj_t* btn2 = lv_btn_create(img1);
        lv_obj_add_event_cb(btn2, ham_toggle_evt_handler, LV_EVENT_CLICKED, this);
        lv_obj_align(btn2, LV_ALIGN_TOP_LEFT, 10, 20);
        lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_set_height(btn2, LV_SIZE_CONTENT);
        lv_obj_t* label;
        label = lv_label_create(btn2);
        lv_label_set_text(label, LV_SYMBOL_LIST);
        lv_obj_center(label);

        //Time stuff
        ltm = localtime(&now);
        timeLabel = lv_label_create(img1);
        lv_obj_set_pos(timeLabel, 100, 10);
        now = time(0);
        // convert now to string form
        dt = ctime(&now);

        lv_label_set_text(timeLabel, dt);
        apps[current_app_index].runState = state::running;
    }
    else if (apps[current_app_index].runState == state::running) {

        //Time stuff
        ltm = localtime(&now);

        now = time(0);
        // convert now to string form
        dt = ctime(&now);

        lv_label_set_text(timeLabel, dt);

    }
}

void DisplayManager::ham_show_evt_hndlr(lv_event_t* e)
{
    if (ham_shown) {
        hamburger_hide();
    }
    else {
        hamburger_menu();
    }
}

int DisplayManager::findAppIndex(string name)
{
    for (int i = 0; i < 7; i++) {
        if (apps[i].name.compare(name) == 0) {
            return i;
        }
    }
    return -1;
}

void DisplayManager::loadApp(int index)
{
    current_app_index = index;
    lv_scr_load(apps[index].screen);


    if (apps[current_app_index].runState == state::notrunning) {
        apps[current_app_index].show();
    }
    show();
}

void DisplayManager::br_runner()
{
    for (int i = 0; i < 7; i++) {
        if ((apps[i].backgroundRefresh != NULL) && (apps[i].bg_exec_count % apps[i].bg_exec_period == 0)) {
            apps[i].backgroundRefresh();
        }
        apps[i].bg_exec_count++;
    }
}

void DisplayManager::fg_runner()
{
    if ((itterCount % apps[current_app_index].fg_exec_period) == 0) {
        if (apps[current_app_index].foregroundRefresh != NULL) {
            apps[current_app_index].foregroundRefresh();
        }
    }
    itterCount++;

}


