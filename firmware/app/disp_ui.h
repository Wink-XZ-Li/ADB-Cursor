#ifndef DISP_UI_H
#define DISP_UI_H

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
    unsigned char dim);

#endif
