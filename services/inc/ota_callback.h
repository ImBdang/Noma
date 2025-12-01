#ifndef __OTA_S__
#define __OTA_S__

#include "modem_typdef.h"
#include "string_handle.h"
#include "event_driven.h"
#include "debug.h"

extern modem_event_queue_t event_queue;
void ota_get_firmware_size_callback(respon_status_t resp_status, const char* line, uint32_t len);

#endif /* __OTA_S__ */