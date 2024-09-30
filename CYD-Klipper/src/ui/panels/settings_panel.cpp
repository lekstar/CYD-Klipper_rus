#include "lvgl.h"
#include "panel.h"
#include "../../core/screen_driver.h"
#include "../../conf/global_config.h"
#include "../main_ui.h"
#include "../ui_utils.h"
#include <Esp.h>
#include "../../core/lv_setup.h"
#include "../ota_setup.h"
#include "../nav_buttons.h"

#ifndef REPO_VERSION
    #define REPO_VERSION "Unknown"
#endif // REPO_VERSION

static void invert_color_switch(lv_event_t * e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    get_current_printer_config()->invert_colors = checked;
    write_global_config();
    set_invert_display();
}

static void reset_calibration_click(lv_event_t * e){
    global_config.screen_calibrated = false;
    write_global_config();
    ESP.restart();
}

static void reset_click(lv_event_t * e){
    ESP.restart();
}

static void reset_wifi_click(lv_event_t * e){
    global_config.wifi_configured = false;
    write_global_config();
    ESP.restart();
}

static void reset_ip_click(lv_event_t * e){
    get_current_printer_config()->ip_configured = false;
    get_current_printer_config()->auth_configured = false;
    write_global_config();
    ESP.restart();
}

static void light_mode_switch(lv_event_t * e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    get_current_printer_config()->light_mode = checked;
    write_global_config();
    set_color_scheme();
}

static void filament_move_mode_switch(lv_event_t * e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    get_current_printer_config()->custom_filament_move_macros = checked;
    write_global_config();
}

static void show_stats_on_progress_panel_dropdown(lv_event_t * e){
    auto selected = lv_dropdown_get_selected(lv_event_get_target(e));
    get_current_printer_config()->show_stats_on_progress_panel = selected;
    write_global_config();
}

static void theme_dropdown(lv_event_t * e){
    lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    get_current_printer_config()->color_scheme = selected;
    set_color_scheme();
    write_global_config();
}

const char* brightness_options = "100%\n75%\n50%\n25%";
const unsigned char  brightness_options_values[] = { 255, 192, 128, 64 };

static void brightness_dropdown(lv_event_t * e){
    lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.brightness = brightness_options_values[selected];
    set_screen_brightness();
    write_global_config();
}

const char* language_options = "English\nРусский";
const unsigned char language_options_values[] = { 0, 1 };

static void language_dropdown(lv_event_t * e){
    lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.language = language_options_values[selected];
    //set_screen_brightness();
    write_global_config();
    ESP.restart();
}
const char wake_timeout_options[2][40] = {"1m\n2m\n5m\n10m\n15m\n30m\n1h\n2h\n4h", "1м\n2м\n5м\n10м\n15м\n30м\n1h\n2ч\n4ч" };
const char  wake_timeout_options_values[] = { 1, 2, 5, 10, 15, 30, 60, 120, 240 };

static void wake_timeout_dropdown(lv_event_t * e){
    lv_obj_t * dropdown = lv_event_get_target(e);
    auto selected = lv_dropdown_get_selected(dropdown);
    global_config.screen_timeout = wake_timeout_options_values[selected];
    set_screen_timer_period();
    write_global_config();
}

static void dualusb_screen_fix_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.display_mode = checked;
    write_global_config();
    ESP.restart();
}

static void disable_m117_messaging_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.disable_m117_messaging = checked;
    write_global_config();
}

static void sort_macros_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.sort_macros = checked;
    write_global_config();
}

static void show_estop_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.show_estop = checked;
    write_global_config();
}

static void full_filenames_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.full_filenames = checked;
    write_global_config();
}

static void rotate_screen_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.rotate_screen = checked;
    global_config.screen_calibrated = false;
    write_global_config();
    ESP.restart();
}

static void on_during_print_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.on_during_print = checked;
    check_if_screen_needs_to_be_disabled();
    write_global_config();
}

static void btn_ota_do_update(lv_event_t * e){
    set_ready_for_ota_update();
}

static void auto_ota_update_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.auto_ota_update = checked;
    write_global_config();
}

static void multi_printer_switch(lv_event_t* e){
    auto state = lv_obj_get_state(lv_event_get_target(e));
    bool checked = (state & LV_STATE_CHECKED == LV_STATE_CHECKED);
    global_config.multi_printer_mode = checked;
    write_global_config();
    nav_buttons_setup(PANEL_SETTINGS);
}

const char estimated_time_options[2][120] = {"Percentage\nInterpolated\nSlicer", "Процент\nИнтерполир.\nПо слайсеру"};

static void estimated_time_dropdown(lv_event_t * e){
    lv_obj_t * dropdown = lv_event_get_target(e);
    get_current_printer_config()->remaining_time_calc_mode = lv_dropdown_get_selected(dropdown);
    write_global_config();
}

const char printer_specific_settings[2][120] = {"Stored per printer", "Записано на каждый принтер"};
#define PRINTER_SPECIFIC_SETTING global_config.multi_printer_mode ? printer_specific_settings[global_config.language] : NULL

void settings_section_theming(lv_obj_t* panel)
{

    lv_obj_t * label = lv_label_create(panel);

    switch(global_config.language){
        
        case 1:
                lv_label_set_text(label, "Темы");

                lv_create_custom_menu_dropdown("Тема", panel, theme_dropdown, "Синяя\nЗеленая\nЛимон\nСерая\nЖелтая\nОранж.\nКрасная\nФиолет.", get_current_printer_config()->color_scheme, NULL, PRINTER_SPECIFIC_SETTING);

#ifndef CYD_SCREEN_DISABLE_INVERT_COLORS
    lv_create_custom_menu_switch("Инвертир. цвета", panel, invert_color_switch, get_current_printer_config()->invert_colors, NULL, (global_config.multi_printer_mode) ? "Stored per printer" 
    #ifdef CYD_SCREEN_DRIVER_ESP32_2432S028R
        "\nОпция нужна только экранам 2.8\" с двумя USB" :  "Опция нужна только экранам 2.8\" с двумя USB"
    #else
       : NULL
    #endif
    );
#endif // CYD_SCREEN_DISABLE_INVERT_COLORS                
            lv_create_custom_menu_switch("Светлая тема", panel, light_mode_switch, get_current_printer_config()->light_mode, NULL, PRINTER_SPECIFIC_SETTING);

                break;
        default:
                lv_label_set_text(label, "Theming");

                lv_create_custom_menu_dropdown("Theme", panel, theme_dropdown, "Blue\nGreen\nLime\nGrey\nYellow\nOrange\nRed\nPurple", get_current_printer_config()->color_scheme, NULL, PRINTER_SPECIFIC_SETTING);
#ifndef CYD_SCREEN_DISABLE_INVERT_COLORS
    lv_create_custom_menu_switch("Invert Colors", panel, invert_color_switch, get_current_printer_config()->invert_colors, NULL, (global_config.multi_printer_mode) ? "Stored per printer" 
    #ifdef CYD_SCREEN_DRIVER_ESP32_2432S028R
        "\nIntended for the 2.8\" dual USB model screen" :  "Intended for the 2.8\" dual USB model screen"
    #else
       : NULL
    #endif
    );
#endif // CYD_SCREEN_DISABLE_INVERT_COLORS
        lv_create_custom_menu_switch("Light Mode", panel, light_mode_switch, get_current_printer_config()->light_mode, NULL, PRINTER_SPECIFIC_SETTING);
    }    
}

void settings_section_behaviour(lv_obj_t* panel)
{
    lv_obj_t * label = lv_label_create(panel);
    int wake_timeout_settings_index = 0;

    switch(global_config.language){
        
        case 1: 

                lv_label_set_text(label, "\nНастройки отображения");

                lv_create_custom_menu_dropdown("Время до конца печати", panel, estimated_time_dropdown, estimated_time_options[global_config.language], get_current_printer_config()->remaining_time_calc_mode, NULL, PRINTER_SPECIFIC_SETTING);
                lv_create_custom_menu_dropdown("Отображ. статистики", panel, show_stats_on_progress_panel_dropdown, "Нет\nСлои\nЧастично\nВсë", get_current_printer_config()->show_stats_on_progress_panel, NULL, PRINTER_SPECIFIC_SETTING);

                #ifndef CYD_SCREEN_DISABLE_TIMEOUT
                    for (int i = 0; i < SIZEOF(wake_timeout_options_values); i++){
                        if (wake_timeout_options_values[i] == global_config.screen_timeout){
                            wake_timeout_settings_index = i;
                            break;
                        }
                    }

                    lv_create_custom_menu_dropdown("Выкл. экрана через", panel, wake_timeout_dropdown, wake_timeout_options[global_config.language], wake_timeout_settings_index);
                #endif

                #ifndef CYD_SCREEN_DISABLE_TIMEOUT
                    lv_create_custom_menu_switch("Не выкл. экран при печати", panel, on_during_print_switch, global_config.on_during_print);
                #endif
                
                lv_create_custom_menu_switch("Несколько принтеров", panel, multi_printer_switch, global_config.multi_printer_mode);
                lv_create_custom_menu_switch("Выкл. сообщения от M117", panel, disable_m117_messaging_switch, global_config.disable_m117_messaging);
                lv_create_custom_menu_button("Задать IP принтера", panel, reset_ip_click, "Задать");

                lv_create_custom_menu_switch("Свои макросы движения филамента", panel, filament_move_mode_switch, get_current_printer_config()->custom_filament_move_macros, NULL, 
                    global_config.multi_printer_mode
                        ? "Когда вкл., вызывает FILAMENT_RETRACT и\nFILAMENT_EXTRUDE меню температур\nОтдельный для каждого принтера."
                        : "Когда вкл., вызывает FILAMENT_RETRACT и\nFILAMENT_EXTRUDE в меню температур\n");
                
                lv_create_custom_menu_switch("Сорт. Macros A->Z", panel, sort_macros_switch, global_config.sort_macros);
                lv_create_custom_menu_switch("Показать Аварийную кнопку", panel, show_estop_switch, global_config.show_estop);
                lv_create_custom_menu_switch("Показать полные пути файлов", panel, full_filenames_switch, global_config.full_filenames);

            break;
        default:
                lv_label_set_text(label, "\nBehaviour");

                lv_create_custom_menu_dropdown("Estimated Time", panel, estimated_time_dropdown, estimated_time_options[global_config.language], get_current_printer_config()->remaining_time_calc_mode, NULL, PRINTER_SPECIFIC_SETTING);
                lv_create_custom_menu_dropdown("Stats in Progress Screen", panel, show_stats_on_progress_panel_dropdown, "None\nLayers\nPartial\nAll", get_current_printer_config()->show_stats_on_progress_panel, NULL, PRINTER_SPECIFIC_SETTING);
    
                #ifndef CYD_SCREEN_DISABLE_TIMEOUT
                    for (int i = 0; i < SIZEOF(wake_timeout_options_values); i++){
                        if (wake_timeout_options_values[i] == global_config.screen_timeout){
                            wake_timeout_settings_index = i;
                            break;
                        }
                    }

                    lv_create_custom_menu_dropdown("Wake Timeout", panel, wake_timeout_dropdown, wake_timeout_options[global_config.language], wake_timeout_settings_index);
                #endif

                #ifndef CYD_SCREEN_DISABLE_TIMEOUT
                    lv_create_custom_menu_switch("Screen On During Print", panel, on_during_print_switch, global_config.on_during_print);
                #endif

                lv_create_custom_menu_switch("Multi Printer Mode", panel, multi_printer_switch, global_config.multi_printer_mode);
                lv_create_custom_menu_switch("Disable M117 Messaging", panel, disable_m117_messaging_switch, global_config.disable_m117_messaging);
                lv_create_custom_menu_button("Configure Printer IP", panel, reset_ip_click, "Restart");

                lv_create_custom_menu_switch("Custom Filament Move Macros", panel, filament_move_mode_switch, get_current_printer_config()->custom_filament_move_macros, NULL, 
                    global_config.multi_printer_mode
                        ? "Calls FILAMENT_RETRACT and\nFILAMENT_EXTRUDE in temperature menu\nwhen enabled. Stored per printer."
                        : "Calls FILAMENT_RETRACT and\nFILAMENT_EXTRUDE in temperature menu\nwhen enabled");
                
                lv_create_custom_menu_switch("Sort Macros A->Z", panel, sort_macros_switch, global_config.sort_macros);
                lv_create_custom_menu_switch("Show Emergency Stop", panel, show_estop_switch, global_config.show_estop);
                lv_create_custom_menu_switch("Show Full Filenames", panel, full_filenames_switch, global_config.full_filenames);
    }    

}

void settings_section_device(lv_obj_t* panel)
{

    lv_obj_t * label = lv_label_create(panel);
    int language_index = global_config.language;
    int brightness_settings_index = 0;
        for (int i = 0; i < SIZEOF(brightness_options_values); i++){
            if (brightness_options_values[i] == global_config.brightness){
                brightness_settings_index = i;
                break;
            }
        }

    switch(global_config.language){
        
        case 1:
            
            lv_label_set_text(label, "\nСистемные настройки");

            lv_create_custom_menu_dropdown("Яркость", panel, brightness_dropdown, brightness_options, brightness_settings_index);

            lv_create_custom_menu_dropdown("Язык", panel, language_dropdown, language_options, language_index);

            #ifdef CYD_SCREEN_DRIVER_ESP32_2432S028R
                lv_create_custom_menu_switch("Коррекция цвета", panel, dualusb_screen_fix_switch, global_config.display_mode, NULL, "Только для экрана 2.8\" с двумя USB");
            #endif
            
            #if defined(CYD_SCREEN_DRIVER_ESP32_SMARTDISPLAY) && !defined(CYD_SCREEN_DISABLE_TOUCH_CALIBRATION)
                // TODO: Rotating screen requires different calibration points. 
            #else
                lv_create_custom_menu_switch("Повернуть экран", panel, rotate_screen_switch, global_config.rotate_screen);
            #endif

            lv_create_custom_menu_switch("Автообновление", panel, auto_ota_update_switch, global_config.auto_ota_update);
            lv_create_custom_menu_label("Версия ПО", panel, REPO_VERSION "  ");

            if (ota_has_update()){
                lv_obj_t *btn = lv_btn_create(panel);
                lv_obj_add_event_cb(btn, btn_ota_do_update, LV_EVENT_CLICKED, NULL);

                lv_obj_t *label = lv_label_create(btn);
                lv_label_set_text_fmt(label, "Обновить до %s", ota_new_version_name().c_str());
                lv_obj_center(label);

                lv_create_custom_menu_entry("Прошивка", btn, panel);
            }
            else {
                lv_create_custom_menu_label("Прошивка", panel, ARDUINO_BOARD "  ");
            }

            #ifndef CYD_SCREEN_DISABLE_TOUCH_CALIBRATION
                lv_create_custom_menu_button("Калибровка экрана", panel, reset_calibration_click, "Калибр.");
            #endif // CYD_SCREEN_DISABLE_TOUCH_CALIBRATION

                lv_create_custom_menu_button("Настройка WiFi", panel, reset_wifi_click, "Настроить");
                lv_create_custom_menu_button("Перезагрузить ESP", panel, reset_click, "Перезагр.");

            break;

        default: 
            lv_label_set_text(label, "\nDevice");

            lv_create_custom_menu_dropdown("Brightness", panel, brightness_dropdown, brightness_options, brightness_settings_index);

            lv_create_custom_menu_dropdown("Language", panel, language_dropdown, language_options, language_index);

            #ifdef CYD_SCREEN_DRIVER_ESP32_2432S028R
                lv_create_custom_menu_switch("Screen Color Fix", panel, dualusb_screen_fix_switch, global_config.display_mode, NULL, "ONLY for the 2.8\" dual USB model screen");
            #endif

            #if defined(CYD_SCREEN_DRIVER_ESP32_SMARTDISPLAY) && !defined(CYD_SCREEN_DISABLE_TOUCH_CALIBRATION)
                // TODO: Rotating screen requires different calibration points. 
            #else
                lv_create_custom_menu_switch("Rotate Screen", panel, rotate_screen_switch, global_config.rotate_screen);
            #endif
            
            lv_create_custom_menu_switch("Auto Update", panel, auto_ota_update_switch, global_config.auto_ota_update);
            lv_create_custom_menu_label("Version", panel, REPO_VERSION "  ");

            if (ota_has_update()){
                lv_obj_t *btn = lv_btn_create(panel);
                lv_obj_add_event_cb(btn, btn_ota_do_update, LV_EVENT_CLICKED, NULL);

                lv_obj_t *label = lv_label_create(btn);
                lv_label_set_text_fmt(label, "Update to %s", ota_new_version_name().c_str());
                lv_obj_center(label);

                lv_create_custom_menu_entry("Device", btn, panel);
            }
            else {
                lv_create_custom_menu_label("Device", panel, ARDUINO_BOARD "  ");
            }

            #ifndef CYD_SCREEN_DISABLE_TOUCH_CALIBRATION
                lv_create_custom_menu_button("Calibrate Touch", panel, reset_calibration_click, "Restart");
            #endif // CYD_SCREEN_DISABLE_TOUCH_CALIBRATION

                lv_create_custom_menu_button("Configure WiFi", panel, reset_wifi_click, "Restart");
                lv_create_custom_menu_button("Restart ESP", panel, reset_click, "Restart");
    }

}

void settings_panel_init(lv_obj_t* panel){
    lv_obj_set_style_pad_all(panel, CYD_SCREEN_GAP_PX, 0);
    lv_layout_flex_column(panel);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);

    settings_section_theming(panel);
    settings_section_behaviour(panel);
    settings_section_device(panel);
}