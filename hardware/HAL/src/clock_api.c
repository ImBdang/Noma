#include "clock_api.h"

// Timeout counter (assuming 1ms tick)
static uint32_t timeout_counter = 0;


void bdang_init_clock(void){
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_PLLConfig(
    RCC_PLLSource_HSI,
    8,                            // M: 16/8 = 2MHz
    192,                          // N: 2×192 = 384MHz 
    6,                            // P: 384/6 = 64MHz 
    8                             // Q: 384/8 = 48MHz 
  );
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}; 
  FLASH_SetLatency(FLASH_ACR_LATENCY_2WS);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  SystemCoreClockUpdate();
}



// Helper function to wait with timeout
static clock_status_t wait_flag_with_timeout(
    __IO uint32_t* reg, 
    uint32_t flag, 
    FlagStatus expected_status,
    uint32_t timeout_ms,
    clock_status_t error_code
) {
    uint32_t start_time = timeout_counter;
    
    while ((*reg & flag) != (expected_status ? flag : 0)) {
        if ((timeout_counter - start_time) > timeout_ms) {
            return error_code;
        }
    }
    return CLOCK_OK;
}

// Enable HSI oscillator
clock_status_t clock_enable_hsi(uint32_t timeout_ms) {
    RCC_HSICmd(ENABLE);
    return wait_flag_with_timeout(
        &RCC->CR, 
        RCC_FLAG_HSIRDY, 
        SET, 
        timeout_ms,
        CLOCK_ERR_HSI_FAIL
    );
}

// Enable HSE oscillator
clock_status_t clock_enable_hse(uint32_t timeout_ms) {
    RCC_HSEConfig(RCC_HSE_ON);
    return wait_flag_with_timeout(
        &RCC->CR,
        RCC_FLAG_HSERDY,
        SET,
        timeout_ms,
        CLOCK_ERR_HSE_FAIL
    );
}

// Validate PLL configuration
bool clock_validate_pll_config(pll_config_t* pll_cfg, uint32_t input_freq) {
    // Check M range (2-63)
    if (pll_cfg->M < 2 || pll_cfg->M > 63) return false;
    
    // Check N range (50-432)
    if (pll_cfg->N < 50 || pll_cfg->N > 432) return false;
    
    // Check P values (must be 2, 4, 6, or 8)
    if (pll_cfg->P != 2 && pll_cfg->P != 4 && 
        pll_cfg->P != 6 && pll_cfg->P != 8) return false;
    
    // Check Q range (2-15)
    if (pll_cfg->Q < 2 || pll_cfg->Q > 15) return false;
    
    // Check VCO input frequency (1-2 MHz recommended)
    uint32_t vco_input = input_freq / pll_cfg->M;
    if (vco_input < 1000000 || vco_input > 2000000) return false;
    
    // Check VCO output frequency (100-432 MHz)
    uint32_t vco_output = vco_input * pll_cfg->N;
    if (vco_output < 100000000 || vco_output > 432000000) return false;
    
    // Check system clock output (max 168 MHz for STM32F4)
    uint32_t sysclk = vco_output / pll_cfg->P;
    if (sysclk > 168000000) return false;
    
    return true;
}

// Enable and configure PLL
clock_status_t clock_enable_pll(pll_config_t* pll_cfg, uint32_t timeout_ms) {
    uint32_t pll_source;
    uint32_t input_freq;
    
    // Determine PLL source and input frequency
    if (pll_cfg->source == PLL_SRC_HSE) {
        pll_source = RCC_PLLSource_HSE;
        input_freq = HSE_VALUE;
    } else {
        pll_source = RCC_PLLSource_HSI;
        input_freq = HSI_VALUE;
    }
    
    // Validate configuration
    if (!clock_validate_pll_config(pll_cfg, input_freq)) {
        return CLOCK_ERR_INVALID_CONFIG;
    }
    
    // Disable PLL before configuration
    RCC_PLLCmd(DISABLE);
    
    // Configure PLL
    RCC_PLLConfig(pll_source, pll_cfg->M, pll_cfg->N, pll_cfg->P, pll_cfg->Q);
    
    // Enable PLL
    RCC_PLLCmd(ENABLE);
    
    // Wait for PLL ready
    return wait_flag_with_timeout(
        &RCC->CR,
        RCC_FLAG_PLLRDY,
        SET,
        timeout_ms,
        CLOCK_ERR_PLL_LOCK_FAIL
    );
}

// Set system clock source
clock_status_t clock_set_sysclk_source(clock_source_t source, uint32_t timeout_ms) {
    uint32_t rcc_source;
    uint32_t flag;
    
    switch (source) {
        case CLOCK_SRC_HSI:
            rcc_source = RCC_SYSCLKSource_HSI;
            flag = RCC_CFGR_SWS_HSI;
            break;
        case CLOCK_SRC_HSE:
            rcc_source = RCC_SYSCLKSource_HSE;
            flag = RCC_CFGR_SWS_HSE;
            break;
        case CLOCK_SRC_PLL:
            rcc_source = RCC_SYSCLKSource_PLLCLK;
            flag = RCC_CFGR_SWS_PLL;
            break;
        default:
            return CLOCK_ERR_INVALID_CONFIG;
    }
    
    RCC_SYSCLKConfig(rcc_source);
    
    // Wait for clock switch
    uint32_t start_time = timeout_counter;
    while ((RCC->CFGR & RCC_CFGR_SWS) != flag) {
        if ((timeout_counter - start_time) > timeout_ms) {
            return CLOCK_ERR_TIMEOUT;
        }
    }
    
    return CLOCK_OK;
}

// Configure bus prescalers
void clock_configure_buses(uint32_t ahb_div, uint32_t apb1_div, uint32_t apb2_div) {
    RCC_HCLKConfig(ahb_div);
    RCC_PCLK1Config(apb1_div);
    RCC_PCLK2Config(apb2_div);
}

// Set flash latency
void clock_set_flash_latency(uint32_t latency) {
    FLASH_SetLatency(latency);
}

// Main clock initialization function
clock_status_t clock_init(clock_config_t* config) {
    clock_status_t status;
    
    // Validate config pointer
    if (config == NULL) {
        return CLOCK_ERR_INVALID_CONFIG;
    }
    
    // Set flash latency BEFORE increasing clock speed
    clock_set_flash_latency(config->flash_latency);
    
    // Enable HSI if needed
    if (config->enable_hsi) {
        status = clock_enable_hsi(config->timeout_ms);
        if (status != CLOCK_OK) return status;
    }
    
    // Enable HSE if needed
    if (config->enable_hse) {
        status = clock_enable_hse(config->timeout_ms);
        if (status != CLOCK_OK) return status;
    }
    
    // Enable and configure PLL if needed
    if (config->enable_pll) {
        status = clock_enable_pll(&config->pll, config->timeout_ms);
        if (status != CLOCK_OK) return status;
    }
    
    // Configure bus prescalers
    clock_configure_buses(
        config->ahb_prescaler,
        config->apb1_prescaler,
        config->apb2_prescaler
    );
    
    // Switch system clock source
    status = clock_set_sysclk_source(config->sysclk_source, config->timeout_ms);
    if (status != CLOCK_OK) return status;
    
    // Update SystemCoreClock variable
    SystemCoreClockUpdate();
    
    return CLOCK_OK;
}

// Get current clock frequencies
void clock_get_frequencies(clock_frequencies_t* freq) {
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    
    freq->sysclk = rcc_clocks.SYSCLK_Frequency;
    freq->hclk = rcc_clocks.HCLK_Frequency;
    freq->pclk1 = rcc_clocks.PCLK1_Frequency;
    freq->pclk2 = rcc_clocks.PCLK2_Frequency;
    
    // Calculate PLL VCO frequency if PLL is enabled
    if (RCC->CR & RCC_CR_PLLON) {
        uint32_t pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
        uint32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6;
        uint32_t pllsrc = RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC;
        
        uint32_t pll_input = (pllsrc == RCC_PLLCFGR_PLLSRC_HSE) ? 
                             HSE_VALUE : HSI_VALUE;
        
        freq->pll_vco = (pll_input / pllm) * plln;
    } else {
        freq->pll_vco = 0;
    }
}

// Get error string
const char* clock_get_error_string(clock_status_t status) {
    switch (status) {
        case CLOCK_OK: return "No error";
        case CLOCK_ERR_TIMEOUT: return "Timeout error";
        case CLOCK_ERR_INVALID_CONFIG: return "Invalid configuration";
        case CLOCK_ERR_PLL_LOCK_FAIL: return "PLL lock failed";
        case CLOCK_ERR_HSE_FAIL: return "HSE startup failed";
        case CLOCK_ERR_HSI_FAIL: return "HSI startup failed";
        default: return "Unknown error";
    }
}

//==============================================================================
// PREDEFINED CONFIGURATIONS
//==============================================================================

// 64 MHz using HSI and PLL
const clock_config_t CLOCK_CONFIG_64MHZ_HSI = {
    .sysclk_source = CLOCK_SRC_PLL,
    .enable_hse = false,
    .enable_hsi = true,
    .enable_pll = true,
    .pll = {
        .source = PLL_SRC_HSI,
        .M = 8,   // 16MHz / 8 = 2MHz
        .N = 192, // 2MHz * 192 = 384MHz
        .P = 6,   // 384MHz / 6 = 64MHz
        .Q = 8    // 384MHz / 8 = 48MHz (for USB)
    },
    .ahb_prescaler = RCC_SYSCLK_Div1,
    .apb1_prescaler = RCC_HCLK_Div2,
    .apb2_prescaler = RCC_HCLK_Div1,
    .flash_latency = FLASH_ACR_LATENCY_2WS,
    .timeout_ms = 100
};

//==============================================================================
// USAGE EXAMPLES
//==============================================================================

/*
// Example 1: Using predefined configuration
#include "clock_config.h"

int main(void) {
    clock_status_t status;
    
    // Use predefined 64MHz HSI configuration
    clock_config_t config = CLOCK_CONFIG_64MHZ_HSI;
    status = clock_init(&config);
    
    if (status != CLOCK_OK) {
        // Handle error
        const char* error = clock_get_error_string(status);
        // Log error or handle it
    }
    
    // Get and display frequencies
    clock_frequencies_t freq;
    clock_get_frequencies(&freq);
    
    while(1) {
        // Your code here
    }
}

// Example 2: Custom configuration
int main(void) {
    clock_config_t config = {
        .sysclk_source = CLOCK_SRC_PLL,
        .enable_hse = true,
        .hse_frequency = 16000000,  // 16MHz crystal
        .enable_hsi = false,
        .enable_pll = true,
        .pll = {
            .source = PLL_SRC_HSE,
            .M = 16,  // 16MHz / 16 = 1MHz
            .N = 336, // 1MHz * 336 = 336MHz
            .P = 2,   // 336MHz / 2 = 168MHz
            .Q = 7    // 336MHz / 7 = 48MHz
        },
        .ahb_prescaler = RCC_SYSCLK_Div1,
        .apb1_prescaler = RCC_HCLK_Div4,
        .apb2_prescaler = RCC_HCLK_Div2,
        .flash_latency = FLASH_ACR_LATENCY_5WS,
        .timeout_ms = 500
    };
    
    clock_status_t status = clock_init(&config);
    if (status != CLOCK_OK) {
        // Error handling
        while(1); // Halt on error
    }
    
    while(1) {
        // Your code
    }
}

// Example 3: Step-by-step configuration with error checking
int main(void) {
    clock_status_t status;
    
    // Step 1: Enable HSE
    status = clock_enable_hse(100);
    if (status != CLOCK_OK) {
        // Fallback to HSI
        status = clock_enable_hsi(100);
        if (status != CLOCK_OK) {
            // Critical error - halt
            while(1);
        }
    }
    
    // Step 2: Configure and enable PLL
    pll_config_t pll = {
        .source = PLL_SRC_HSE,
        .M = 8,
        .N = 336,
        .P = 2,
        .Q = 7
    };
    
    // Validate before applying
    if (!clock_validate_pll_config(&pll, HSE_VALUE)) {
        // Invalid config - use safe defaults
        while(1);
    }
    
    status = clock_enable_pll(&pll, 100);
    if (status != CLOCK_OK) {
        // PLL failed - use HSE directly
        clock_set_sysclk_source(CLOCK_SRC_HSE, 100);
    } else {
        // Step 3: Configure buses and flash
        clock_set_flash_latency(FLASH_ACR_LATENCY_5WS);
        clock_configure_buses(
            RCC_SYSCLK_Div1,
            RCC_HCLK_Div4,
            RCC_HCLK_Div2
        );
        
        // Step 4: Switch to PLL
        status = clock_set_sysclk_source(CLOCK_SRC_PLL, 100);
        if (status != CLOCK_OK) {
            // Failed to switch
            while(1);
        }
    }
    
    // Step 5: Verify final frequencies
    clock_frequencies_t freq;
    clock_get_frequencies(&freq);
    
    // Check if we got expected frequency
    if (freq.sysclk != 168000000) {
        // Frequency mismatch
    }
    
    while(1) {
        // Your application
    }
}

// Example 4: Safe initialization with retry logic
clock_status_t safe_clock_init(void) {
    clock_status_t status;
    
    // Try HSE + PLL (best performance)
    clock_config_t config = CLOCK_CONFIG_168MHZ_HSE;
    status = clock_init(&config);
    
    if (status == CLOCK_OK) {
        return CLOCK_OK;
    }
    
    // Fallback to HSI + PLL (good performance)
    config = CLOCK_CONFIG_64MHZ_HSI;
    status = clock_init(&config);
    
    if (status == CLOCK_OK) {
        return CLOCK_OK;
    }
    
    // Last resort: Just use HSI (16MHz, no PLL)
    status = clock_enable_hsi(100);
    if (status == CLOCK_OK) {
        clock_set_flash_latency(FLASH_ACR_LATENCY_0WS);
        clock_configure_buses(
            RCC_SYSCLK_Div1,
            RCC_HCLK_Div1,
            RCC_HCLK_Div1
        );
        clock_set_sysclk_source(CLOCK_SRC_HSI, 100);
    }
    
    return status;
}

int main(void) {
    if (safe_clock_init() != CLOCK_OK) {
        // Even HSI failed - critical hardware problem
        while(1);
    }
    
    while(1) {
        // Your code
    }
}
*/

// 84 MHz using 8MHz HSE and PLL
const clock_config_t CLOCK_CONFIG_84MHZ_HSE = {
    .sysclk_source = CLOCK_SRC_PLL,
    .enable_hse = true,
    .hse_frequency = 8000000,
    .enable_hsi = false,
    .enable_pll = true,
    .pll = {
        .source = PLL_SRC_HSE,
        .M = 8,   // 8MHz / 8 = 1MHz
        .N = 336, // 1MHz * 336 = 336MHz
        .P = 4,   // 336MHz / 4 = 84MHz
        .Q = 7    // 336MHz / 7 = 48MHz (for USB)
    },
    .ahb_prescaler = RCC_SYSCLK_Div1,
    .apb1_prescaler = RCC_HCLK_Div2,
    .apb2_prescaler = RCC_HCLK_Div1,
    .flash_latency = FLASH_ACR_LATENCY_2WS,
    .timeout_ms = 100
};

// 168 MHz using 8MHz HSE and PLL (max for STM32F4)
const clock_config_t CLOCK_CONFIG_168MHZ_HSE = {
    .sysclk_source = CLOCK_SRC_PLL,
    .enable_hse = true,
    .hse_frequency = 8000000,
    .enable_hsi = false,
    .enable_pll = true,
    .pll = {
        .source = PLL_SRC_HSE,
        .M = 8,   // 8MHz / 8 = 1MHz
        .N = 336, // 1MHz * 336 = 336MHz
        .P = 2,   // 336MHz / 2 = 168MHz
        .Q = 7    // 336MHz / 7 = 48MHz (for USB)
    },
    .ahb_prescaler = RCC_SYSCLK_Div1,
    .apb1_prescaler = RCC_HCLK_Div4,  // Max 42MHz
    .apb2_prescaler = RCC_HCLK_Div2,  // Max 84MHz
    .flash_latency = FLASH_ACR_LATENCY_5WS,
    .timeout_ms = 100
};