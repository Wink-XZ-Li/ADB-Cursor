#ifndef BOOT_IAP_H
#define BOOT_IAP_H

unsigned char boot_ee_read(unsigned int addr);
void boot_ee_erase(unsigned int addr);
void boot_ee_write(unsigned int addr, unsigned char v);
unsigned char boot_code_read(unsigned int addr);
void boot_code_erase(unsigned int addr);
void boot_code_write(unsigned int addr, unsigned char v);

#endif
