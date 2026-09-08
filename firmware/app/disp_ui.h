#ifndef DISP_UI_H
#define DISP_UI_H

#define DISP_OV_NONE  0
#define DISP_OV_DASH  1
#define DISP_OV_E1    2
#define DISP_OV_E2    3
#define DISP_OV_SPIN  4

void disp_ui_init(void);
void disp_ui_draw(
    unsigned char power,
    unsigned char saver,
    unsigned char mode,
    unsigned char fan,
    unsigned char show_num,
    unsigned char num,
    unsigned char timer_lamp,
    unsigned char wifi_lamp,
    unsigned char dim,
    unsigned char overlay);

#endif
