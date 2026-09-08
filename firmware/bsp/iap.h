#ifndef IAP_H
#define IAP_H

unsigned char iap_code_read(unsigned int addr);
void iap_code_erase(unsigned int addr);
unsigned char iap_code_write(unsigned int addr, unsigned char v);

#endif
