#ifndef OTA_DL_H
#define OTA_DL_H

unsigned char ota_begin(unsigned long file_len);
unsigned char ota_write(unsigned long pos, unsigned char *buf, unsigned int n);
unsigned char ota_finish(unsigned long file_len);
unsigned char ota_busy(void);

#endif
