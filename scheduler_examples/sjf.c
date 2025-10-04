#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "sjf.h"
#include "msg.h"
#include "queue.h"
#include "debug.h"

/**
 * Função auxiliar: encontra o processo com menor tempo restante.
 */
static queue_elem_t *find_shortest_elem(queue_t *q) {
    if (!q || !q->head) return NULL;

    queue_elem_t *it = q->head;
    queue_elem_t *best = it;
    uint32_t best_remaining = it->pcb->time_ms - it->pcb->ellapsed_time_ms;

    it = it->next;
    while (it != NULL) {
        pcb_t *p = it->pcb;
        uint32_t remaining = p->time_ms - p->ellapsed_time_ms;
        if (remaining < best_remaining) {
            best_remaining = remaining;
            best = it;
        }
        it = it->next;
    }
    return best;
}

/**
 * Escalonador SJF (não preemptivo)
 */
void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    // Se há processo em execução
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t));
            DBG("SJF: Processo %d terminou", (*cpu_task)->pid);
            free(*cpu_task);
            *cpu_task = NULL;
        }
    }

    // Se CPU está livre, escolher o processo mais curto
    if (*cpu_task == NULL) {
        queue_elem_t *best_elem = find_shortest_elem(rq);
        if (best_elem != NULL) {
            queue_elem_t *removed = remove_queue_elem(rq, best_elem);
            pcb_t *p = removed->pcb;
            free(removed);
            *cpu_task = p;
            DBG("SJF: escolheu processo %d com %u ms restantes", p->pid,
                p->time_ms - p->ellapsed_time_ms);
        }
    }
}