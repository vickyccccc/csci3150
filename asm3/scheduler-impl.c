#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time);

// Implement by students
void scheduler(Process *proc, LinkedQueue **ProcessQueue, int proc_num, int queue_num, int period)
{
    if (proc_num > 0)
    {
        // TODO: init startTime and endTime for time slot and proc.completion_time
        for (int i = 0; i < proc_num; i++)
        {
            proc[i].service_time = 0;
            proc[i].waiting_time = 0;
        }
        int startTime = proc[0].arrival_time, endTime = 0;
        printf("(init) startTime: %d\n", startTime);

        // TODO: enqueue process to highest priority
        for (int i = 0; i < proc_num; i++)
            if (proc[i].arrival_time == startTime)
                EnQueue(ProcessQueue[queue_num - 1], proc[i]);

        // TODO: set N th time_slice will reduce priority
        int N[queue_num];
        for (int i = 0; i < queue_num; i++)
        {
            N[i] = ProcessQueue[i]->allotment_time / ProcessQueue[i]->time_slice;
            // QueuePrint(ProcessQueue[i]);
            // printf("N[%d]: %d\n", i, N[i]);
        }

        // int time = 0;
        int allProcDone = 0;
        while (1)
        {
            // printf("%d\n", ++time);
            // if (time > 28)
            // {
            //     break;
            // }

            // TODO: Check all process done --> end
            if (allProcDone == proc_num)
            {
                printf("end\n");
                break;
            }

            // TODO: Get the proc from highest priority
            for (int i = queue_num - 1; i >= 0; i--)
            {
                if (!IsEmptyQueue(ProcessQueue[i]))
                {
                    // QueuePrint(ProcessQueue[i]);
                    Process currentProc = DeQueue(ProcessQueue[i]);

                    // TODO: set endTime as time_slice
                    endTime = startTime + ProcessQueue[i]->time_slice;
                    printf("[%d] endTime by time_slice: %d\n", currentProc.process_id, endTime);

                    // TODO: check if Period_S, update endTime (Situation 4)
                    int isSpansPeriod = 0;
                    for (int A = 1;; A++)
                    {
                        int periodPoint = A * period;
                        if (startTime < periodPoint && periodPoint <= endTime)
                        {
                            isSpansPeriod = 1;
                            endTime = periodPoint;
                            printf("[%d] endTime by periodPoint: %d\n", currentProc.process_id, endTime);
                            break;
                        }
                        else if (periodPoint > endTime)
                            break;
                    }

                    // TODO: check remaining_time, update endTime and current proc info
                    int remaining_time = currentProc.execution_time - currentProc.service_time;
                    int timeSlot = endTime - startTime;
                    timeSlot = timeSlot < remaining_time ? timeSlot : remaining_time;
                    endTime = startTime + timeSlot;
                    printf("[%d] remaining_time: %d\n", currentProc.process_id, remaining_time);
                    printf("[%d] timeSlot: %d\n", currentProc.process_id, timeSlot);
                    currentProc.service_time += timeSlot;
                    currentProc.waiting_time++;
                    int NoEnQueue = 1;
                    printf("[%d] execution_time: %d\n", currentProc.process_id, currentProc.execution_time);
                    printf("[%d] service_time: %d\n", currentProc.process_id, currentProc.service_time);
                    if (currentProc.execution_time == currentProc.service_time)
                    {
                        printf("Process %d done at %d\n", currentProc.process_id, endTime);
                        currentProc.completion_time = endTime;
                        currentProc.turnaround_time = currentProc.completion_time - currentProc.arrival_time;
                        currentProc.waiting_time = currentProc.turnaround_time - currentProc.execution_time;
                        allProcDone++;
                    }
                    else
                        NoEnQueue = currentProc.waiting_time == N[i] ? -1 : 0;

                    printf("[%d] NoEnQueue: %d\n", currentProc.process_id, NoEnQueue);

                    // TODO: enqueue new proc to highest priority & Situation 1, 2, 5
                    for (int j = 0; j < proc_num; j++)
                    {
                        if (proc[j].arrival_time > startTime && proc[j].arrival_time < endTime)
                        {
                            EnQueue(ProcessQueue[queue_num - 1], proc[j]);
                            printf("[%d] is enqueued 1\n", proc[j].process_id);
                        }
                    }
                    if (NoEnQueue != 1)
                    {
                        currentProc.waiting_time = NoEnQueue == -1 ? 0 : currentProc.waiting_time;
                        EnQueue(ProcessQueue[i + NoEnQueue], currentProc);
                        printf("[%d] is enqueued 2\n", currentProc.process_id);
                    }
                    for (int j = 0; j < proc_num; j++)
                    {
                        if (proc[j].arrival_time == endTime)
                        {
                            EnQueue(ProcessQueue[queue_num - 1], proc[j]);
                            printf("[%d] is enqueued 3\n", proc[j].process_id);
                        }
                    }

                    // TODO: if Period_s, re-enqueue all proc by pid ascendingly
                    if (isSpansPeriod)
                    {
                        LinkedQueue *temp = InitQueue(0);
                        printf("Period! %d\n", endTime);
                        for (int k = queue_num - 1; k >= 0; k--)
                            while (!IsEmptyQueue(ProcessQueue[k]))
                            {
                                EnQueue(temp, DeQueue(ProcessQueue[k]));
                            }
                        QueuePrint(temp);

                        int size = Length(temp);
                        int pids[size];
                        for (int x = 0; x < size; x++)
                        {
                            Process y = DeQueue(temp);
                            pids[x] = y.process_id;
                            EnQueue(temp, y);
                        }

                        for (int m = 0; m < size - 1; m++)
                            for (int n = 0; n < size - m - 1; n++)
                                if (pids[n] > pids[n + 1])
                                {
                                    int z = pids[n];
                                    pids[n] = pids[n + 1];
                                    pids[n + 1] = z;
                                }

                        for (int x = 0; x < size; x++)
                        {
                            for (int y = 0; y < size - x; y++)
                            {
                                Process target = DeQueue(temp);
                                if (pids[x] == target.process_id)
                                {
                                    EnQueue(ProcessQueue[queue_num - 1], target);
                                    break;
                                }
                                else
                                    EnQueue(temp, target);
                            }
                        }
                        QueuePrint(ProcessQueue[queue_num - 1]);
                    }

                    // TODO: outprint
                    outprint(startTime, endTime, currentProc.process_id, currentProc.arrival_time, currentProc.execution_time - currentProc.service_time);

                    // TODO: set next startTime
                    startTime = endTime;
                    printf("startTime: %d\n", startTime);
                    printf("endTime: %d\n", endTime);
                    break;
                }
            }
        }
    }
}
