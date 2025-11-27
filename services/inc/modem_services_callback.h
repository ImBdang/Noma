#ifndef __MODEM_CALLBACK_S__
#define __MODEM_CALLBACK_S__

#include "modem_api.h"
#include "modem_services.h"
#include "string_handle.h"
#include "event_driven.h"

/*<! Used for the CPIN state */
typedef enum {
    SIM_STATE_READY,
    SIM_STATE_PIN,
    SIM_STATE_PUK,
    SIM_STATE_NOT_RDY
} sim_state_t;

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
void modem_activate_pdp_cgact_callback(respon_status_t reps_status, const char* line, uint32_t len);

#endif  /* __MODEM_CALLBACK_S__ */