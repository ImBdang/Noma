/* DEFINE --------------------------------------------------------------------------------------- */
#define TIMEOUT_WAIT_APP_RDY    7000
#define TIMEOUT_WAIT_SIM_RDY    7000
#define TIMEOUT_WAIT_AT_OK      7000
#define TIMEOUT_WAIT_CONFIG     7000
#define TIMEOUT_SMS_SEND        7000
#define TIMEOUT_EXCMD_SEND      7000
#define MAX_RETRY               3
#define MAX_MASTER_RETRY        5


/* SMS CONFIG ----------------------------------------------------------------------------------- */
char target_phone_number[15] =  "+84869668837";
//char target_phone_number[15] =  "+84837645067";
char message[128] =             "Big cow, small cow";