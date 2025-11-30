#ifndef __MODEM_CALLBACK_FSM_S__
#define __MODEM_CALLBACK_FSM_S__

#include "modem_typdef.h"
#include "modem_api.h"
#include "modem_service.h"
#include "string_handle.h"
#include "event_driven.h"

/*<! Used for the CPIN state */
typedef enum {
    SIM_STATE_READY,
    SIM_STATE_PIN,
    SIM_STATE_PUK,
    SIM_STATE_NOT_RDY
} sim_state_t;

/*<! Used for the CREG state */
typedef enum {
    NET_NOT_REGISTERED = 0,   // stat = 0
    NET_REGISTERED_HOME,      // stat = 1
    NET_SEARCHING,            // stat = 2
    NET_REG_DENIED,           // stat = 3
    NET_UNKNOWN,              // stat = 4
    NET_REGISTERED_ROAMING,   // stat = 5
    NET_REGISTERED_SMS,       // stat = 6
    NET_REGISTERED_SMS_ROAM   // stat = 7
} network_state_t;


/* ================================ API DECLARATIONS ================================== */
/*<! CALLBACK FOR AT SYNC */
void modem_sync_at_callback(respon_status_t reps_status, const char* line, uint32_t len);            

/*<! CALLBACK FOR CONFIG STATE */
void modem_config_ate0_callback(respon_status_t reps_status, const char* line, uint32_t len);
void modem_config_cfun1_callback(respon_status_t reps_status, const char* line, uint32_t len);

/*<! CALLBACK FOR CHECK SIM STATE */
void modem_check_sim_cpin_callback(respon_status_t reps_status, const char* line, uint32_t len);

/*<! CALLBACK FOR CHECK REGISTRATION STATE */
void modem_check_reg_callback(respon_status_t reps_status, const char* line, uint32_t len);

/*<! CALLBACK FOR ATTACH PSD STATE */
void modem_attach_psd_cgatt_callback(respon_status_t reps_status, const char* line, uint32_t len);

/*<! CALLBACK FOR ACTIVATE PDP STATE */
void modem_pdp_cgdcont_callback(respon_status_t resp_status, const char* line, uint32_t len);

void modem_pdp_cgact_callback(respon_status_t resp_status, const char* line, uint32_t len);

void dummy_callback(respon_status_t resp_status, const char* line, uint32_t len);

#endif  /* __MODEM_CALLBACK_FSM_S__ */