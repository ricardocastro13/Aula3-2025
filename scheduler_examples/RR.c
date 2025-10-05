#include "RR.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>
#include "debug.h"

void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task, uint32_t *slice_elapsed) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        *slice_elapsed += TICKS_MS;


        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {.pid = (*cpu_task)->pid, .request = PROCESS_REQUEST_DONE, .time_ms = current_time_ms};
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t));
            DBG("RR: Processo %d terminou", (*cpu_task)->pid);
            free(*cpu_task);
            *cpu_task = NULL;
            *slice_elapsed = 0;
        }

        else if (*slice_elapsed >= TIME_SLICE_MS) {
            enqueue_pcb(rq, *cpu_task);
            DBG("RR: Processo %d volta para o fim da fila", (*cpu_task)->pid);
            *cpu_task = NULL;
            *slice_elapsed = 0;
        }
    }


    if (*cpu_task == NULL && rq->head != NULL) {
        *cpu_task = dequeue_pcb(rq);
        DBG("RR: escolheu processo %d da fila", (*cpu_task)->pid);
    }
}