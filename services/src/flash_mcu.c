#include "flash_mcu.h"

extern modem_event_queue_t event_queue;      

static ota_step_t step = OTA_PREPARE;
static ota_init_step_t step_init = OTA_SET_URL;

uint32_t chunk_actual = 0;
uint32_t current_offset = 0;
uint32_t current_firmware_size = 0;

bool is_ota = false;
uint32_t firmware_size = 0;

static uint32_t flash_get_sector(uint32_t addr);
/* ====================================== DECLARATIONS ======================================= */
static bool ota_prepare_entry(void);
static void ota_prepare_wait(void);

static void sector_program_entry(void);
static void sector_program_wait(void);
/* =========================================================================================== */

static bool ota_prepare_entry(void){
    bool tmp = false;
    switch (step_init)
    {
        case OTA_SET_URL:{
            modem_at_cmd_t cmd = {
                .cmd = "AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/ImBdang/raw/main/main.bin\"",
                .expect = "",
                .timeout_ms = 5000,
                .start_tick = get_systick_ms(),
                .cb = dummy_callback
            };
            return modem_send_at_cmd(cmd);
        }

        case OTA_GET_SIZE: {
            modem_at_cmd_t cmd = {
                .cmd = "AT+HTTPACTION=0",
                .expect = "+HTTPACTION:",
                .timeout_ms = 5000,
                .start_tick = get_systick_ms(),
                .cb = ota_get_firmware_size_callback
            };
            return modem_send_at_cmd(cmd);
        }
    }
    return false;
}

static void ota_prepare_wait(void){
    modem_event_t evt;
    static uint8_t timeout_count = 0;
    static uint8_t max_timeout = 3;
    if (!pop_event(&event_queue, &evt))
        return;

    switch (evt)
    {
        case EVT_OK:
            timeout_count = 0; 
            if (step_init == OTA_SET_URL){
                step_init = OTA_GET_SIZE;
            }
            else if (step_init == OTA_GET_SIZE){
                step = FLASH_SECTOR_ERASE;
                step_init = OTA_SET_URL;
            }
            else {
                step = OTA_PREPARE;
            }
            break;


        case EVT_TIMEOUT:
            break;
        

        case EVT_ERROR:
            break;


        default:
            break;
    }
}

void ota(void){
    bool tmp = false;
    switch (step)
    {
    case OTA_PREPARE:
        tmp = ota_prepare_entry();
        if (tmp){
            step = OTA_WAIT_PREPARE;
        }
        break;
    
    case OTA_WAIT_PREPARE:
        ota_prepare_wait();
        break;
    
    case FLASH_SECTOR_ERASE:
        breakp();
        break;

    case FLASH_SECTOR_WAIT_ERASE:

        break;

    case FLASH_SECTOR_PROGRAM:

        break;

    case FLASH_SECTOR_WAIT_PROGRAM:

        break;


    default:
        break;
    }
}


void flash_erase_sector_addr(uint32_t addr)
{
    FLASH_Status status;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP  | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    uint32_t sector = flash_get_sector(addr);

    status = FLASH_EraseSector(sector, VoltageRange_3); 
    (void)status;

    FLASH_Lock();
}


void flash_chunk(uint8_t *data, uint32_t len, uint32_t addr)
{
    FLASH_Status status;
    while (len % 4U) {
        data[len++] = 0xFF;
    }

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP  | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    for (uint32_t i = 0; i < len; i += 4) {
        uint32_t word =
              (uint32_t)data[i]
            | ((uint32_t)data[i + 1] << 8)
            | ((uint32_t)data[i + 2] << 16)
            | ((uint32_t)data[i + 3] << 24);

        status = FLASH_ProgramWord(addr + i, word);
        if (status != FLASH_COMPLETE) {
            break;
        }
    }

    FLASH_Lock();
}

static uint32_t flash_get_sector(uint32_t addr)
{
    if (addr < 0x08004000) return FLASH_Sector_0;   // 16 KB
    if (addr < 0x08008000) return FLASH_Sector_1;   // 16 KB
    if (addr < 0x0800C000) return FLASH_Sector_2;   // 16 KB
    if (addr < 0x08010000) return FLASH_Sector_3;   // 16 KB
    if (addr < 0x08020000) return FLASH_Sector_4;   // 64 KB
    /* 128 KB */
    return FLASH_Sector_5;
}