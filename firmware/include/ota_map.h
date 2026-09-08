#ifndef OTA_MAP_H
#define OTA_MAP_H

#define BOOT_BASE       0x0000U
#define BOOT_SIZE       0x1000U
#define RUN_BASE        0x1000U
#define RUN_SIZE        0x7800U
#define DL_BASE         0x8800U
#define DL_SIZE         0x7800U
#define APP_ENTRY       0x1800U
#define OADB_HDR_N      16U
#define OTA_PAYLOAD_MAX ((unsigned int)(DL_SIZE - OADB_HDR_N))
#define OTA_MAGIC_EE    0x0200U
#define OTA_MAGIC0      0x55U
#define OTA_MAGIC1      0xAAU
#define OTA_MAGIC2      0x69U
#define OTA_MAGIC3      0x96U
#define OADB_0          'O'
#define OADB_1          'A'
#define OADB_2          'D'
#define OADB_3          'B'
#define IAP_SEC         512U
#define IAP_CHUNK       64U
#define IAP_SEC_N_RUN   ((unsigned char)(RUN_SIZE / IAP_SEC))
#define IAP_SEC_N_DL    ((unsigned char)(DL_SIZE / IAP_SEC))
#define APP_CODE_LAST   0x87EFU

#endif
