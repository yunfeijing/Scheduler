#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "deque.h"
#include "process.h"
#include "algo.h"

// First come first serve algorithm
void ff(Deque* deque, char* memory_alloc, int mem_size, int quantum) {
    
    assert(deque!=NULL);
    Node *curr = deque->head;

    // Start the clock by 0
    int clock = 0;

    // Statistics variables
    int total_throughput = 0; // number of 60-sec rounds
    int curr_throughput = 0;
    int max_throughput = 0;
    int min_throughput = 999;
    double total_processes = 0;
    double total_turnaround = 0;
    double total_overhead = 0;
    double max_overhead = 0;


    while (curr!=NULL) {
        
        Process* curr_process = &curr->process;
        if (clock >= curr_process->arrival_time) {
            // When unlimited memory
            printf("%d, RUNNING, id=%d, remaining-time=%d\n", clock, curr_process->pid, curr_process->remaining_time);
            while (curr_process->remaining_time >= 0) {
                
                if (curr_process->remaining_time == 0) {
                    // Finish current process
                    curr_throughput++;
                    printf("%d, FINISHED, id=%d, proc-remaining=%d\n", clock, curr_process->pid, deque->size-1);
                }

                if (clock % 60 == 0 && clock > 0) {
                    if (curr_throughput < min_throughput) {
                        min_throughput = curr_throughput;
                    }
                    else if (curr_throughput > max_throughput) {
                        max_throughput = curr_throughput;
                    }
                    total_throughput++;
                    curr_throughput = 0;
                }

                if (curr_process->remaining_time == 0) {
                    break;
                }

                clock++;
                curr_process->remaining_time--;
                // printList(node);
            }
            
            // Statistics
            total_processes++;
            double curr_turnaround = clock - curr_process->arrival_time;
            total_turnaround += curr_turnaround;
            double curr_overhead = curr_turnaround / curr_process->job_time;
            total_overhead += curr_overhead;
            if (curr_overhead > max_overhead) {
                max_overhead = curr_overhead;
            }

            curr = curr->next;
            deque_pop(deque);
            
        } else {
            clock++;
        }
    }

    // Statistics
    int avg_throughput = (total_processes+total_throughput-1)/total_throughput; //Round up
    int avg_turnaround = (total_turnaround+total_processes-1)/total_processes; //Round up
    double avg_overhead = total_overhead/total_processes;
    printf("\nThroughput %d, %d, %d\nTurnaround time %d\nTime overhead %.2f %.2f\nMakespan %d\n", avg_throughput, min_throughput, max_throughput, avg_turnaround, max_overhead, avg_overhead, clock);

}



// Round-robin algorithm
void rr(Deque* deque, char* memory_alloc, int mem_size, int quantum) {
    
    assert(deque!=NULL);
    Node *curr = deque->head;

    // Start the clock by 0
    int clock = 0;

    while (curr!=NULL) {
        
        int elapsed = 0;
        Process* curr_process = &curr->process;

        if (clock >= curr_process->arrival_time) {
            // When unlimited memory
            printf("%d, RUNNING, id=%d, remaining-time=%d\n", clock, curr_process->pid, curr_process->remaining_time);
            while (elapsed <= quantum) {

                if (elapsed == quantum) {
                    move_to_bottom(deque);
                    curr = deque->head;
                    break;
                }

                clock++;
                elapsed++;
                curr_process->remaining_time--;

                if (curr_process->remaining_time == 0) {
                    // Finish current process
                    printf("%d, FINISHED, id=%d, proc-remaining=%d\n", clock, curr_process->pid, deque->size-1);
                    curr = curr->next;
                    deque_pop(deque);
                    break;
                }    
            }
        }
        else {
            clock++;
        }
    }
}