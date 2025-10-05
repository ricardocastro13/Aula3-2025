#ifndef MLFQ_H
#define MLFQ_H

#include <stdint.h>
#include "queue.h"

#define NUM_QUEUES 3

// Só declara, não define QUANTUMS aqui!
// Definição ficará no mlfq.c
extern const uint32_t QUANTUMS[NUM_QUEUES];

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif