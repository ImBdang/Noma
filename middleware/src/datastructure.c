#include "datastructure.h"

/**
 * @brief Init ring buffer
 * 
 * @param ring_buffer: Pointer to a Ring_buffer that you init 
 * @param buffer: Pointer to an array that store value inside
 * @param size: Size of an array
 * 
 * @retval None
 */
void init_ring_buffer(Ring_buffer* ring_buffer, uint16_t* buffer, size_t size){
    if (size < 2 || !ring_buffer || !buffer) return; 
    ring_buffer->buffer = buffer;
    ring_buffer->size = size;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
}

/**
 * @brief Push the data in to buffer[head]
 * 
 * @param ring_buffer: Pointer to a Ring_buffer that you init 
 * @param data: Data that will push in to an buffer
 * 
 * @retval 0 if is full
 * @retval 1 if is success
 */
uint8_t enqueue_ring_buffer(Ring_buffer* ring_buffer, uint16_t data){
    if (ring_isFull(ring_buffer))
        return 0;
    ring_buffer->buffer[ring_buffer->head] = data;
    ring_buffer->head = (ring_buffer->head + 1) % ring_buffer->size;
    return 1;
}

/**
 * @brief Get the buffer[tail] 
 * 
 * @param ring_buffer: Pointer to a Ring_buffer that you init 
 * @param data_return: Pointer that will store dequeued variable
 * 
 * @retval 0 if is empty
 * @retval 1 if it success
 */
uint8_t dequeue_ring_buffer(Ring_buffer* ring_buffer, uint16_t* data_return){
    if (ring_isEmpty(ring_buffer))
        return 0;
    *data_return = ring_buffer->buffer[ring_buffer->tail];
    ring_buffer->tail = (ring_buffer->tail + 1) % ring_buffer->size;
    return 1;
}

/**
 * @brief Check if ring buffer is full.
 *
 * @param ring Pointer to initialized Ring_buffer.
 *
 * @retval 1 Buffer is full.
 * @retval 0 Buffer is not full.
 */
uint8_t ring_isFull(Ring_buffer *ring){
    return ((ring->head + 1) % ring->size) == ring->tail;
}

/**
 * @brief Check if ring buffer is empty.
 *
 * @param ring Pointer to initialized Ring_buffer.
 *
 * @retval 1 Buffer is empty.
 * @retval 0 Buffer has data.
 */
uint8_t ring_isEmpty(Ring_buffer *ring){
    return ring->head == ring->tail;
}