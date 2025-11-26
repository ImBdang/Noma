#ifndef __CLOCK_M__
#define __CLOCK_M__

/* Include header ---------------------------------------------------- */
#include "stm32f4xx_rcc.h"
#include "stddef.h"
#include <stdbool.h>

// Clock source options
typedef enum {
    CLOCK_SRC_HSI,
    CLOCK_SRC_HSE,
    CLOCK_SRC_PLL
} clock_source_t;

// PLL source options
typedef enum {
    PLL_SRC_HSI,
    PLL_SRC_HSE
} pll_source_t;

// Clock status codes
typedef enum {
    CLOCK_OK = 0,
    CLOCK_ERR_TIMEOUT,
    CLOCK_ERR_INVALID_CONFIG,
    CLOCK_ERR_PLL_LOCK_FAIL,
    CLOCK_ERR_HSE_FAIL,
    CLOCK_ERR_HSI_FAIL
} clock_status_t;

// PLL configuration structure
typedef struct {
    pll_source_t source;    // HSI or HSE
    uint32_t M;             // Division factor for PLL input (2-63)
    uint32_t N;             // Multiplication factor (50-432)
    uint32_t P;             // Division factor for main system clock (2,4,6,8)
    uint32_t Q;             // Division factor for USB/SDIO (2-15)
} pll_config_t;

// Complete clock configuration structure
typedef struct {
    // Clock source selection
    clock_source_t sysclk_source;
    
    // HSE configuration
    bool enable_hse;
    uint32_t hse_frequency;     // External crystal frequency in Hz
    
    // HSI configuration
    bool enable_hsi;
    
    // PLL configuration
    bool enable_pll;
    pll_config_t pll;
    
    // Bus prescalers
    uint32_t ahb_prescaler;     // AHB (HCLK) prescaler
    uint32_t apb1_prescaler;    // APB1 (PCLK1) prescaler
    uint32_t apb2_prescaler;    // APB2 (PCLK2) prescaler
    
    // Flash latency
    uint32_t flash_latency;     // Flash wait states
    
    // Timeout value for clock ready flags
    uint32_t timeout_ms;
} clock_config_t;

// System clock frequencies (calculated after init)
typedef struct {
    uint32_t sysclk;    // System clock frequency
    uint32_t hclk;      // AHB clock frequency
    uint32_t pclk1;     // APB1 clock frequency
    uint32_t pclk2;     // APB2 clock frequency
    uint32_t pll_vco;   // PLL VCO frequency
} clock_frequencies_t;

// Function prototypes
clock_status_t clock_init(clock_config_t* config);
clock_status_t clock_enable_hsi(uint32_t timeout_ms);
clock_status_t clock_enable_hse(uint32_t timeout_ms);
clock_status_t clock_enable_pll(pll_config_t* pll_cfg, uint32_t timeout_ms);
clock_status_t clock_set_sysclk_source(clock_source_t source, uint32_t timeout_ms);
void clock_configure_buses(uint32_t ahb_div, uint32_t apb1_div, uint32_t apb2_div);
void clock_set_flash_latency(uint32_t latency);
bool clock_validate_pll_config(pll_config_t* pll_cfg, uint32_t input_freq);
void clock_get_frequencies(clock_frequencies_t* freq);
const char* clock_get_error_string(clock_status_t status);
void bdang_init_clock(void);

// Predefined configurations for common scenarios
extern const clock_config_t CLOCK_CONFIG_64MHZ_HSI;
extern const clock_config_t CLOCK_CONFIG_84MHZ_HSE;
extern const clock_config_t CLOCK_CONFIG_168MHZ_HSE;

#endif