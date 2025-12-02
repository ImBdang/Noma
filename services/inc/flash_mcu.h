#ifndef __FLASH_MCU_S__
#define __FLASH_MCU_S__

#include "stdint.h"
#include "http_service.h"
#include "stm32f4xx_flash.h"
#include "stdbool.h"
#include "ota_callback.h"
#include "debug.h"

#define  CHUNK_SIZE       512
#define  SECTOR_5_ADDR    0x08020000  
extern uint32_t current_offset;
extern uint32_t current_firmware_size;

typedef enum {
    OTA_PREPARE = 0,
    OTA_WAIT_PREPARE,

    FLASH_SECTOR_ERASE,
    FLASH_SECTOR_WAIT_ERASE,

    FLASH_SECTOR_PROGRAM,
    FLASH_SECTOR_WAIT_PROGRAM,

    MCU_REBOOT
} ota_step_t;

typedef enum {
    OTA_SET_URL = 0,

    OTA_GET_SIZE
} ota_init_step_t;

extern uint32_t firmware_size;

/* ============================== API ============================== */
void ota(void);
void flash_chunk(uint8_t* data, uint32_t len, uint32_t addr);
void flash_erase_sector_addr(uint32_t addr);

#endif /* __FLASH_MCU_S__ */