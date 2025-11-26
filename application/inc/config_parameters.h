

/* PARAMETERS FOR GSM_FSM --------------------------------------------------------------------------------------- */
#define TIMEOUT_WAIT_APP_RDY    7000
#define TIMEOUT_WAIT_SIM_RDY    10000
#define TIMEOUT_WAIT_AT_OK      7000
#define TIMEOUT_WAIT_CONFIG     8000
#define TIMEOUT_SMS_SEND        7000
#define TIMEOUT_EXCMD_SEND      7000
#define TIMEOUT_NETWORK_REG     7000
#define MAX_RETRY               3
#define MAX_MASTER_RETRY        5
#define USART1_RING_BUFFER_SIZE 255


/* SMS CONFIG ----------------------------------------------------------------------------------- */
extern const char target_phone_number[15];
extern const char message[128];
