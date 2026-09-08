#ifndef EEPROM_H
#define EEPROM_H

unsigned char eeprom_read(unsigned int addr);
void eeprom_erase_sector(unsigned int addr);
void eeprom_write_byte(unsigned int addr, unsigned char v);

#endif
