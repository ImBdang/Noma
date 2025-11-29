#ifndef __SIM_CONTEXT_HW__

#include "stdint.h"
#include "stdbool.h"

/* ================================== TYPEDEF STRUCT ====================================== */
typedef struct {
    char imsi[20];            // SIM's IMSI
    char iccid[25];           // SIM serial number
    char msisdn[20];          // Phone number (if provided)
    bool sim_ready;           // CPIN=READY
} sim_info_t;


typedef struct {
    char operator_name[32];   // Viettel, Vina, Mobi
    char mcc[4];              // Mobile Country Code
    char mnc[4];              // Mobile Network Code

    uint8_t csq;                 // signal (0–31)
    
    char lac[8];              // Location Area Code
    char cell_id[12];         // Cell ID
    
    uint8_t creg_status;         // GSM registration
    uint8_t cgreg_status;        // GPRS registration

    bool gprs_attached;       // CGATT=1?
} network_info_t;


typedef struct {
    char apn[32];             // APN name
    bool pdp_active;          // PDP activated?
    char ip[40];              // IP assigned by GGSN
    char pdp_type[8];         // IP / IPV6 / IP4V6

    uint8_t cid;                 // PDP context ID
} pdp_info_t;


typedef struct {
    sim_info_t     sim;
    network_info_t network;
    pdp_info_t     pdp;
} sim_context_t;

/* ================================== API DECLARETIONS ====================================== */
extern sim_context_t sim_context;

#endif /* __SIM_CONTEXT_HW__ */