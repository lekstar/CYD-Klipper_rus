#include "panel.h"
#include "../../conf/global_config.h"

void connecting_panel_init(lv_obj_t* panel) 
{
    lv_obj_t* label = lv_label_create(panel);

    switch(global_config.language){
        case 1:
            lv_label_set_text_fmt(label, "Подключение к адресу:\n%s...", (get_current_printer_config()->printer_name[0] == 0) ? get_current_printer_config()->klipper_host : get_current_printer_config()->printer_name);
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            break;
        default:
            lv_label_set_text_fmt(label, "Connecting to\n%s...", (get_current_printer_config()->printer_name[0] == 0) ? get_current_printer_config()->klipper_host : get_current_printer_config()->printer_name);
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    
}