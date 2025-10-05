#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

typedef enum  {
    TASK_COMMAND = 0,
    TASK_BLOCKED,
    TASK_RUNNING,
    TASK_STOPPED,
    TASK_TERMINATED,
} task_status_en;

// Define the Process Control Block (PCB) structure
typedef struct pcb_st{
    int32_t pid;
    task_status_en status;
    uint32_t time_ms;
    uint32_t ellapsed_time_ms;
    uint32_t slice_start_ms;
    uint32_t sockfd;
    uint32_t last_update_time_ms;


    uint32_t queue_level;
    uint32_t quantum_used_ms;
} pcb_t;

// Define singly linked list elements
typedef struct queue_elem_st queue_elem_t;
typedef struct queue_elem_st {
    pcb_t *pcb;
    queue_elem_t *next;
} queue_elem_t;

// Define the queue structure
// We define the head and the tail to make it easier to enqueue and dequeue
typedef struct queue_st  {
    queue_elem_t* head;
    queue_elem_t* tail;
} queue_t;

/**
 * @brief Create a new pcb (process control block)
 *
 * This function allocates memory for a new pcb and initializes its fields.
 *
 * @param pid The process ID of the task
 * @param sockfd The socket file descriptor for communication with the application
 * @param time_ms a time field (either for run or block)
 * @return
 */
pcb_t *new_pcb(int32_t pid, uint32_t sockfd, uint32_t time_ms);

/**
 * @brief Enqueue a pcb into the queue
 *
 * This function adds a pcb to the end of the queue (FIFO order).
 *
 * @param q The queue to which the pcb will be added
 * @param task The pcb to be added to the queue
 * @return The number of pcb enqueued (0 on failure)
 */
int enqueue_pcb(queue_t* q, pcb_t* task);

/**
 * @brief Dequeue a pcb from the queue
 *
 * This function removes and returns the pcb at the front of the queue (FIFO order).
 *
 * @param q The queue from which the task will be removed
 * @return The pcb at the front of the queue, or NULL if the queue is empty
 */
pcb_t* dequeue_pcb(queue_t* q);

/**
 * @brief Remove a specific element from the queue
 *
 * This function removes a specific element from the queue.
 * Neither the element, nor the pcb inside the element, are freed.
 *
 * @param q The queue from which the element will be removed
 * @param elem The element to be removed from the queue
 * @return The removed element, or NULL if the element was not found
 */
queue_elem_t *remove_queue_elem(queue_t* q, queue_elem_t* elem);


#endif //QUEUE_H
