#include "modem_service.h"


/**
 * @brief   Power off modem
 */
bool modem_power_off(void){

}


/**
 * @brief   Power on modem
 * 
 * @note    
 */
bool modem_power_on(void)
{
    static uint8_t step = 0;
    static uint32_t t0 = 0;

    switch (step)
    {
        case 0:
            SIM_PWKEY_LOW();
            t0 = sys_get_tick_ms();
            step = 1;
            return false;

        case 1:
            if (sys_get_tick_ms() - t0 < 1200)
                return false;
            SIM_PWKEY_HIGH();
            t0 = sys_get_tick_ms();
            step = 2;
            return false;

        case 2:
            if (SIM_STATUS_READ())
            {
                step = 0;
                return true;
            }
            return false;
    }
    return false;
}



/**
 * @brief   Power reset modem
 */
void modem_power_reset(void){

}



/**
 * @brief   Set power for modem
 * 
 * @param   state: Power mode that you want to set ON/OFF/RESET
 * 
 * @retval  Status OK/ERROR/TIMEOUT
 */
modem_status_t modem_power(modem_power_t state){
    switch (state)
    {
    case MODEM_PWR_OFF:
        modem_power_off();
        break;

    case MODEM_PWR_ON:
        modem_power_on();
        break;

    default:
        modem_power_reset();
        break;
    }
}

void init_modem(void){

}