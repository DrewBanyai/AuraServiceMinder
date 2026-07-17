#ifndef SERVICE_MONITOR_UI_H
#define SERVICE_MONITOR_UI_H

#include <lvgl.h>
#include <vector>

#include "declarations.h"

class ServiceMonitorUI {
    public:
        struct ServiceMonitorStatus {
            String ServiceName;
            int StagingStatus;
            int ProdStatus;
        };

        struct ServiceMonitorGrouping {
            public:
                lv_obj_t *label_ServiceName;
                lv_obj_t *image_StagingStatus;
                lv_obj_t *image_ProdStatus;
        };

        //  UI Components
        #define SERVICE_TRACKING_COUNT 3
        static inline ServiceMonitorGrouping StatusRowObjects[SERVICE_TRACKING_COUNT]{};

        static const lv_image_dsc_t* GetStatusIcon(int index) {
            switch (index) {
                case 0:     return &icon_healthy_service_marker;
                case 1:     return &icon_midhealthy_service_marker;
                case 2:     return &icon_unhealthy_service_marker;
                default:    return &icon_unhealthy_service_marker;
            }
        }

        static void CreateUI() {
            lv_obj_t *screen = lv_scr_act();
            lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

            static lv_style_t default_label_style;
            lv_style_init(&default_label_style);
            lv_style_set_text_color(&default_label_style, lv_color_hex(0xFFFFFF));
            lv_style_set_text_opa(&default_label_style, LV_OPA_COVER);

            for (int i = 0; i < SERVICE_TRACKING_COUNT; ++i)
            {
                StatusRowObjects[i].label_ServiceName   = lv_label_create(screen);
                StatusRowObjects[i].image_StagingStatus = lv_img_create(screen);
                StatusRowObjects[i].image_ProdStatus    = lv_img_create(screen);

                lv_obj_add_style(StatusRowObjects[i].label_ServiceName, &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_font(StatusRowObjects[i].label_ServiceName, get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_align(StatusRowObjects[i].label_ServiceName, LV_ALIGN_TOP_LEFT, 2, i * 24);

                lv_img_set_src(StatusRowObjects[i].image_StagingStatus, GetStatusIcon(i));
                lv_obj_align(StatusRowObjects[i].image_StagingStatus, LV_ALIGN_TOP_LEFT, 120, i * 24);

                lv_img_set_src(StatusRowObjects[i].image_ProdStatus, GetStatusIcon(i));
                lv_obj_align(StatusRowObjects[i].image_ProdStatus, LV_ALIGN_TOP_LEFT, 160, i * 24);
            }
        }

        static void UpdateUI(ServiceMonitorStatus* monitorDetails)
        {
            for (int i = 0; i < SERVICE_TRACKING_COUNT; ++i)
            {
                lv_label_set_text(StatusRowObjects[i].label_ServiceName, monitorDetails[i].ServiceName.c_str());
                lv_img_set_src(StatusRowObjects[i].image_StagingStatus, GetStatusIcon(monitorDetails[i].StagingStatus));
                lv_img_set_src(StatusRowObjects[i].image_ProdStatus,    GetStatusIcon(monitorDetails[i].ProdStatus));
            }
        }
};

#endif // SERVICE_MONITOR_UI_H