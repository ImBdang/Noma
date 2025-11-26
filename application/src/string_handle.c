#include "string_handle.h"

/**
 * @brief   Use to split prefix of the extended command
 * 
 * @param   excmd: Pointer to root extended command
 * @param   excmd_prefix: pointer to prefix of extended command for saving 
 */
void split_excmd(char* excmd, char* excmd_prefix, size_t buf_size){
    char* ptr = excmd + 2; 
    size_t i = 0;
    while (*ptr && *ptr != '?' && *ptr != '=' && i < buf_size - 1) {
        excmd_prefix[i++] = *ptr++;
    }
    excmd_prefix[i] = '\0'; 
}
