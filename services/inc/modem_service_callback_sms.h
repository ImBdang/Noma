#ifndef __MODEM_CALLBACK_SMS_S__
#define __MODEM_CALLBACK_SMS_S__

#include "modem_api.h"
#include "string_handle.h"
#include "event_driven.h"
#include "modem_service_sms.h"

/* ================================ API DECLARATIONS ================================== */
void sms_cmgf_callback(respon_status_t resp_status, const char *str,  uint32_t len);
void sms_cscs_callback(respon_status_t resp_status, const char *str, uint32_t len);
void sms_cmgs_callback(respon_status_t resp_status, const char *str, uint32_t len);
void sms_wait_result_callback(respon_status_t resp_status, const char *str, uint32_t len);
void sms_read_sms_callback(respon_status_t resp_status, const char *str, uint32_t len);
#endif  /* __MODEM_CALLBACK_SMS_S__ */