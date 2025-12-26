
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_SIZE 1031   
#define MAX_LINE 256

typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED, KILLED } ProcessState;

typedef struct PCB {
    int pid;
    int arrivalTime;
    int totalBurst;       
    int remainingBurst;   
    int executedTime;     
    int ioStart;         
    int ioDuration;       
    int ioRemaining;      
    int responseTime;    
    int firstScheduled;   
    int completionTime;
    ProcessState state;
    struct PCB *nextHash; 
} PCB;


static PCB *hashBuckets[HASHMAP_SIZE];

static int pidHash(int pid) {
    if (pid < 0) pid = -pid;
    return pid % HASHMAP_SIZE;
}

static void hashmapInsert(PCB *p) {
    int idx = pidHash(p->pid);
    p->nextHash = hashBuckets[idx];
    hashBuckets[idx] = p;
}

static PCB *hashmapFind(int pid) {
    int idx = pidHash(pid);
    PCB *cur = hashBuckets[idx];
    while (cur) {
        if (cur->pid == pid) return cur;
        cur = cur->nextHash;
    }
    return NULL;
}

/* Remove from hashmap (unlink only, not free) */
static void hashmapRemove(int pid) {
    int idx = pidHash(pid);
    PCB *cur = hashBuckets[idx];
    PCB *prev = NULL;
    while (cur) {
        if (cur->pid == pid) {
            if (prev) prev->nextHash = cur->nextHash;
            else hashBuckets[idx] = cur->nextHash;
            cur->nextHash = NULL;
            return;
        }
        prev = cur;
        cur = cur->nextHash;
    }
}

/* ----------------- Queues (Ready, Waiting, Terminated) ----------------- */
typedef struct Node {
    PCB *process;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} Queue;

static void queueInit(Queue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

static void queuePush(Queue *q, PCB *p) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->process = p;
    n->next = NULL;
    if (q->tail) q->tail->next = n;
    else q->head = n;
    q->tail = n;
    q->size++;
}

static PCB *queuePop(Queue *q) {
    if (q->head == NULL) return NULL;
    Node *n = q->head;
    PCB *p = n->process;
    q->head = n->next;
    if (q->head == NULL) q->tail = NULL;
    free(n);
    q->size--;
    return p;
}

/* Remove a PCB by pid from queue; return 1 if removed, 0 otherwise */
static int queueRemoveByPid(Queue *q, int pid) {
    Node *cur = q->head, *prev = NULL;
    while (cur) {
        if (cur->process->pid == pid) {
            if (prev) prev->next = cur->next;
            else q->head = cur->next;
            if (cur == q->tail) q->tail = prev;
            free(cur);
            q->size--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

/* Find but do not remove - returns pointer to PCB if present */
static PCB *queueFind(Queue *q, int pid) {
    Node *cur = q->head;
    while (cur) {
        if (cur->process->pid == pid) return cur->process;
        cur = cur->next;
    }
    return NULL;
}

/* ----------------- Global simulation queues ----------------- */
static Queue readyQueue;
static Queue waitingQueue;   /* waiting for IO */
static Queue terminatedQueue;

/* ----------------- Utility: create PCB ----------------- */
static PCB *createPCB(int pid, int arrival, int burst, int ioStart, int ioDur) {
    PCB *p = (PCB*)malloc(sizeof(PCB));
    p->pid = pid;
    p->arrivalTime = arrival;
    p->totalBurst = burst;
    p->remainingBurst = burst;
    p->executedTime = 0;
    p->ioStart = ioStart;
    p->ioDuration = ioDur;
    p->ioRemaining = 0;
    p->responseTime = -1;
    p->firstScheduled = 0;
    p->completionTime = -1;
    p->state = NEW;
    p->nextHash = NULL;
    return p;
}

/* ----------------- Input structures for events ----------------- */
typedef struct KillEvent {
    int tick;
    int pid;
    struct KillEvent *next;
} KillEvent;

static KillEvent *killEvents = NULL;

static void addKillEvent(int tick, int pid) {
    KillEvent *e = (KillEvent*)malloc(sizeof(KillEvent));
    e->tick = tick;
    e->pid = pid;
    e->next = killEvents;
    killEvents = e;
}

/* ----------------- Simulation helpers ----------------- */

/* Move process to terminated queue with final state */
static void terminateProcess(PCB *p, ProcessState finalState, int currentTick) {
    p->state = finalState;
    p->completionTime = currentTick;
    queuePush(&terminatedQueue, p);
    hashmapRemove(p->pid);
}

/* Move process to waitingQueue for IO */
static void moveToIO(PCB *p) {
    p->state = WAITING;
    p->ioRemaining = p->ioDuration;
    queuePush(&waitingQueue, p);
}

/* After IO completes, move back to ready */
static void moveFromIOToReady(PCB *p) {
    p->state = READY;
    queuePush(&readyQueue, p);
}

/* ----------------- Simulation main loop ----------------- */
static void simulate(PCB **allProcesses, int processCount, int maxTick) {
    int tick = 0;
    PCB *running = NULL;

    /* initialize queues + hashmap */
    queueInit(&readyQueue);
    queueInit(&waitingQueue);
    queueInit(&terminatedQueue);
    for (int i = 0; i < HASHMAP_SIZE; ++i) hashBuckets[i] = NULL;

    for (int i = 0; i < processCount; ++i) {
        hashmapInsert(allProcesses[i]);
    }

    while (1) {
        /* 1) Check new arrivals at this tick: add to readyQueue */
        for (int i = 0; i < processCount; ++i) {
            PCB *p = allProcesses[i];
            if (p->arrivalTime == tick && p->state == NEW) {
                p->state = READY;
                queuePush(&readyQueue, p);
            }
        }

        /* 2) Handle kill events scheduled for this tick */
        KillEvent *ke = killEvents;
        while (ke) {
            if (ke->tick == tick) {
                /* find process in running, ready or waiting */
                if (running && running->pid == ke->pid) {
                    terminateProcess(running, KILLED, tick);
                    running = NULL;
                } else {
                    /* try remove from ready */
                    if (queueRemoveByPid(&readyQueue, ke->pid)) {
                        PCB *p = hashmapFind(ke->pid);
                        if (p) terminateProcess(p, KILLED, tick);
                    } else if (queueRemoveByPid(&waitingQueue, ke->pid)) {
                        PCB *p = hashmapFind(ke->pid);
                        if (p) terminateProcess(p, KILLED, tick);
                    } else {
                        PCB *p = hashmapFind(ke->pid);
                        if (p && p->state != TERMINATED) {
                            /* could be not yet arrived - mark terminated directly */
                            terminateProcess(p, KILLED, tick);
                        }
                    }
                }
            }
            ke = ke->next;
        }

        /* 3) If CPU idle and readyQueue has processes, schedule next (FCFS) */
        if (running == NULL) {
            running = queuePop(&readyQueue);
            if (running) {
                running->state = RUNNING;
                if (!running->firstScheduled) {
                    running->responseTime = tick - running->arrivalTime;
                    running->firstScheduled = 1;
                }
            }
        }

        /* 4) Execute one tick on running process */
        if (running) {
            running->remainingBurst -= 1;
            running->executedTime += 1;

            /* Check if it needs to go to IO now (if ioStart specified and executedTime reaches ioStart) */
            if (running->ioStart >= 0 && running->executedTime == running->ioStart && running->ioRemaining == 0) {
                /* Move it to waiting for IO (preserve remainingBurst) */
                moveToIO(running);
                running = NULL;
            } else if (running->remainingBurst == 0) {
                /* Completed normally at this tick +1 (end of this tick) */
                terminateProcess(running, TERMINATED, tick + 1);
                running = NULL;
            }
        }

        /* 5) Update waiting queue (IO): decrease ioRemaining and move back if done */
        if (waitingQueue.size > 0) {
            int currentWaiting = waitingQueue.size; /* iterate exactly these many nodes */
            Node *prev = NULL;
            Node *cur = waitingQueue.head;
            while (cur && currentWaiting > 0) {
                PCB *p = cur->process;
                /* decrement IO */
                p->ioRemaining -= 1;
                /* if IO completes now, move from waiting to ready */
                Node *nextNode = cur->next; /* save next node pointer because cur may be removed */
                if (p->ioRemaining <= 0) {
                    /* remove cur node from waitingQueue */
                    if (prev) prev->next = nextNode;
                    else waitingQueue.head = nextNode;
                    if (cur == waitingQueue.tail) waitingQueue.tail = prev;
                    waitingQueue.size--;
                    /* push process to ready queue */
                    p->state = READY;
                    queuePush(&readyQueue, p);
                    /* free the node container cur */
                    free(cur);
                    /* cur moves to nextNode but prev unchanged */
                    cur = nextNode;
                } else {
                    /* still waiting */
                    prev = cur;
                    cur = nextNode;
                }
                currentWaiting--;
            }
        }

        /* 6) Termination condition: no running, ready empty, waiting empty */
        int terminatedCount = terminatedQueue.size;
        if (running == NULL && readyQueue.size == 0 && waitingQueue.size == 0) {
            /* Also ensure that all processes that were NEW have arrival times > tick */
            int allTerminated = 1;
            for (int i = 0; i < processCount; ++i) {
                PCB *p = allProcesses[i];
                if (p->state != TERMINATED && p->state != KILLED) {
                    allTerminated = 0;
                    break;
                }
            }
            if (allTerminated) {
                break;
            }
        }

        tick++;
        if (tick > maxTick && maxTick >= 0) {
            /* safety break if user wants an upper bound */
            break;
        }
    }

    /* At the end, if a process is still running (shouldn't in normal flow), terminate */
    if (running) terminateProcess(running, TERMINATED, tick);

    /* Simulation finished */
}

/* ----------------- Output / summary printing ----------------- */
static void printSummary() {
    printf("\n---- Final Process Summary ----\n");
    printf("PID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\tResponse\tState\n");
    Node *cur = terminatedQueue.head;
    while (cur) {
        PCB *p = cur->process;
        int turnaround = p->completionTime - p->arrivalTime;
        int waiting = turnaround - p->totalBurst;
        const char *stateStr = (p->state == TERMINATED) ? "TERMINATED" : (p->state == KILLED ? "KILLED" : "OTHER");
        printf("%d\t%d\t%d\t%d\t\t%d\t\t%d\t%d\t\t%s\n",
               p->pid,
               p->arrivalTime,
               p->totalBurst,
               p->completionTime,
               turnaround,
               waiting,
               p->responseTime,
               stateStr);
        cur = cur->next;
    }
}

/* ----------------- Helper: free all allocated PCBs and events (cleanup) ----------------- */
static void cleanupAll(PCB **list, int count) {
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        PCB *p = hashBuckets[i];
        while (p) {
            PCB *next = p->nextHash;
            free(p);
            p = next;
        }
        hashBuckets[i] = NULL;
    }
    for (int i = 0; i < count; ++i) list[i] = NULL;
    /* free kill events */
    KillEvent *ke = killEvents;
    while (ke) {
        KillEvent *nx = ke->next;
        free(ke);
        ke = nx;
    }
    killEvents = NULL;
}


int main(int argc, char *argv[]) {
    int N;
    if (scanf("%d", &N) != 1) {
        fprintf(stderr, "Invalid input: expected number of processes\n");
        return 1;
    }

    PCB **allProcs = (PCB**)malloc(sizeof(PCB*) * N);
    for (int i = 0; i < N; ++i) {
        int pid, arrival, burst, ioStart, ioDur;
        if (scanf("%d %d %d %d %d", &pid, &arrival, &burst, &ioStart, &ioDur) != 5) {
            fprintf(stderr, "Invalid process line at %d\n", i);
            return 1;
        }
        if (ioStart < 0) { ioStart = -1; ioDur = 0; }
        PCB *p = createPCB(pid, arrival, burst, ioStart, ioDur);
        allProcs[i] = p;
    }

    int M;
    if (scanf("%d", &M) != 1) {
        fprintf(stderr, "Invalid input: expected number of kill events\n");
        M = 0;
    }
    for (int i = 0; i < M; ++i) {
        int tick, pid;
        if (scanf("%d %d", &tick, &pid) != 2) {
            fprintf(stderr, "Invalid kill event line at %d\n", i);
            return 1;
        }
        addKillEvent(tick, pid);
    }

    int maxTick;
    if (scanf("%d", &maxTick) != 1) {
        maxTick = -1;
    }

    simulate(allProcs, N, maxTick);

    printSummary();

    cleanupAll(allProcs, N);
    free(allProcs);
    return 0;
}
