#include "mlfq.h"
#include "msg.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static queue_t queues[NUM_QUEUES];
static int initialized = 0;

// Quantums de cada fila (em ms)
const uint32_t QUANTUMS[NUM_QUEUES] = {100, 200, 400};

// Inicializa filas se ainda não tiver sido feito
static void init_queues() {
    if (!initialized) {
        for (int i = 0; i < NUM_QUEUES; i++) {
            queues[i].head = queues[i].tail = NULL;
        }
        initialized = 1;
    }
}

// Adiciona PCB a uma fila específica
static void enqueue_process(pcb_t *p, int level) {
    if (level >= NUM_QUEUES) level = NUM_QUEUES - 1;
    p->queue_level = level;
    p->quantum_used_ms = 0;
    enqueue_pcb(&queues[level], p);
    DBG("MLFQ: Processo %d colocado na fila %d", p->pid, level);
}

// Retorna próximo PCB disponível (fila mais prioritária primeiro)
static pcb_t *dequeue_next() {
    for (int i = 0; i < NUM_QUEUES; i++) {
        if (queues[i].head != NULL) {
            return dequeue_pcb(&queues[i]);
        }
    }
    return NULL;
}

// Função principal do escalonador MLFQ
void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    init_queues();

    // Mover novos processos da ready_queue para a fila de alta prioridade (0)
    while (rq->head != NULL) {
        pcb_t *p = dequeue_pcb(rq);
        enqueue_process(p, 0);
    }

    // Se CPU está ocupada
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->quantum_used_ms += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            // Processo terminou
            msg_t msg = {.pid = (*cpu_task)->pid, .request = PROCESS_REQUEST_DONE, .time_ms = current_time_ms};
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t));
            DBG("MLFQ: Processo %d terminou", (*cpu_task)->pid);
            free(*cpu_task);
            *cpu_task = NULL;
        } else if ((*cpu_task)->quantum_used_ms >= QUANTUMS[(*cpu_task)->queue_level]) {
            // Quantum esgotado → desce de fila
            int new_level = (*cpu_task)->queue_level + 1;
            enqueue_process(*cpu_task, new_level);
            DBG("MLFQ: Processo %d esgotou quantum, vai para fila %d", (*cpu_task)->pid, new_level);
            *cpu_task = NULL;
        }
    }

    // Se CPU está livre, escolhe o processo da fila mais prioritária
    if (*cpu_task == NULL) {
        *cpu_task = dequeue_next();
        if (*cpu_task) {
            (*cpu_task)->quantum_used_ms = 0;
            DBG("MLFQ: Escalonou processo %d da fila %d", (*cpu_task)->pid, (*cpu_task)->queue_level);
        }
    }
}