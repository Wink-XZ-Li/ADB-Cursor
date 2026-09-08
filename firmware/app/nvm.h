#ifndef NVM_H
#define NVM_H

typedef struct
{
    unsigned char power;
    unsigned char mode;
    unsigned char fan;
    unsigned char fan_saved;
    unsigned char unit_f;
    unsigned char sp;
    unsigned char sleep;
    unsigned char saver;
} nvm_rec_t;

unsigned char nvm_load(nvm_rec_t *rec);
void nvm_capture(nvm_rec_t *rec);
void nvm_poll(nvm_rec_t *rec);

#endif
