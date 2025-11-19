#ifndef __DATASTRUCTURE__
#define __DATASTRUCTURE__

#include <stdint.h>
#include <stddef.h>


/**
 * @brief Basic ring buffer
 * 
 * @note User provide an array to a buffer, size of array using init function
 *       Remember buffer type is uint8_t
 */
typedef struct{
    uint8_t* buffer;
    size_t size;
    volatile size_t head;
    volatile size_t tail;
} Ring_buffer;

uint8_t ring_isFull(Ring_buffer* ring);
uint8_t ring_isEmpty(Ring_buffer* ring);
void init_ring_buffer(Ring_buffer* ring_buffer, uint8_t* buffer, size_t size);
uint8_t enqueue_ring_buffer(Ring_buffer* ring_buffer, uint8_t data);
uint8_t dequeue_ring_buffer(Ring_buffer* ring_buffer, uint8_t* data_return);
/* --------------------------------------------------------------------------- */

#endif /* __DATASTRUCTURE__ */


