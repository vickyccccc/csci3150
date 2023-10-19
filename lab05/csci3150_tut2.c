#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

Process* proc_tmp; // Used to temporarily store the input process.

// Make sure that process.file and queue.cfg are in the same directory as your code.
int ReadProcessFile(); // Return the number of process in process.file, all the processes are stored in proc.
int min(int x, int y); // Return the less one between x and y.
Process MinProc(Process x, Process y); // Return the process arrive earlier; if arrive at the same time, return the one have less pid.
void SortProcess(Process* p, int num); // Sort proc arrording to arrival_time and pid.

int GetQueueNum(); // Return the number of queue in queue.cfg.
void ReadQueueCfg(LinkedQueue** LQueue, int num);

void InitOutputFile(); // Create void output.log file.
void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time); // Print one line to output.log file.

void Calculate(Process* proc, int proc_num);
void Schedule(Process* proc, LinkedQueue** ProcessQueue, int proc_num);

int main(){
    int proc_num = ReadProcessFile();
    Process proc[proc_num];
    for (int i = 0;i < proc_num; i++){
        proc[i].process_id = proc_tmp[i].process_id;
        proc[i].arrival_time = proc_tmp[i].arrival_time;
        proc[i].execution_time = proc_tmp[i].execution_time;
    }
    SortProcess(proc, proc_num);

    int queue_num = GetQueueNum();
    LinkedQueue** ProcessQueue = (LinkedQueue**)malloc(sizeof(LinkedQueue*) * queue_num);
    ReadQueueCfg(ProcessQueue, queue_num);


    Calculate(proc, proc_num);
    InitOutputFile();
    Schedule(proc, ProcessQueue, proc_num);

    return 0;
}

int ReadProcessFile(){
    int ProcessNum = 0;
    FILE* process_file = fopen("./process.file", "r");
    char chache_line[1000];
    int LineNum = 0;
    while (!feof(process_file)){
        fgets(chache_line, 1000, process_file);
        char* pch = strtok(chache_line, " ,:");
        if (LineNum == 0){
            int i = 0;
            while (pch != NULL){
                if (i == 1){
                    ProcessNum = atoi(pch);
                    proc_tmp = (Process*)malloc(ProcessNum * sizeof(Process));
                }
                pch = strtok(NULL, " ,:");
                i++;
            }
        }else{
            int i = 0;
            while (pch != NULL){
                if (i == 1){
                    proc_tmp[LineNum-1].process_id = atoi(pch);
                }else if (i == 3){
                    proc_tmp[LineNum-1].arrival_time = atoi(pch);
                }else if (i == 5){
                    proc_tmp[LineNum-1].execution_time = atoi(pch);
                }
                pch = strtok(NULL, " ,:");
                i++;
            }
        }
        LineNum++;
    }
    fclose(process_file);
    return ProcessNum;
}

int GetQueueNum(){
    int QueueNum = 0;
    FILE* queue_file = fopen("./queue.cfg", "r");
    char chache_line[1000];
    int LineNum = 0;
    while (!feof(queue_file)){
        fgets(chache_line, 1000, queue_file);
        char* pch = strtok(chache_line, " ,:");
        if (LineNum == 0){
            int i = 0;
            while (pch != NULL){
                if (i == 1){
                    QueueNum = atoi(pch);
                    break;
                }
                pch = strtok(NULL, " ,:");
                i++;
            }
            break;
        }
    }
    fclose(queue_file);
    return QueueNum;          
}

void ReadQueueCfg(LinkedQueue** LQueue, int num){
    // printf("in ReadQueueFcg, queuenum: %d\n", num);
    // LinkedQueue** LQueue;
    // LQueue = (LinkedQueue**)malloc(sizeof(LinkedQueue*) * num);
    int QueueNum = num;
    FILE* queue_file = fopen("./queue.cfg", "r");
    char chache_line[1000];
    int LineNum = 0;
    for (int i=0;i<QueueNum;i++){
        LQueue[i] = (LinkedQueue*)malloc(sizeof(LinkedQueue));
        LQueue[i] -> next = NULL;
    }
    while (!feof(queue_file)){
        fgets(chache_line, 1000, queue_file);
        char* pch = strtok(chache_line, " ,:");
        if (LineNum == 0){
            while (pch != NULL){
                pch=strtok(NULL, " ,:");
            }
        }else if (LineNum != 0){
            int i = 0;
            while (pch != NULL){
                if (i == 1){
                    LQueue[QueueNum-LineNum]->time_slice = atoi(pch);
                    LQueue[QueueNum-LineNum]->allotment_time = 0;
                }
                pch = strtok(NULL, " ,:");
                i++;
            }
        }
        LineNum++;
    }
    fclose(queue_file);
    // return LQueue;
}

int min(int x, int y){
    return x < y ? x : y;
}

Process MinProc(Process x, Process y){
    Process min_proc;
    if (x.arrival_time < y.arrival_time)
        min_proc = x;
    else if (x.arrival_time == y.arrival_time){
        if (x.process_id < y.process_id)
            min_proc = x;
        else if (x.process_id > y.process_id)
            min_proc = y;
    }else if (x.arrival_time > y.arrival_time)
        min_proc = y;
    return min_proc;
}

void SortProcess(Process* p, int num){
    Process* a = p;
    Process* b = (Process*)malloc(num * sizeof(Process));
    int seg, start;
    for (seg = 1; seg < num; seg += seg) {
        for (start = 0; start < num; start += seg + seg) {
            int low = start, mid = min(start + seg, num), high = min(start + seg + seg, num);
            int k = low;
            int start1 = low, end1 = mid;
            int start2 = mid, end2 = high;
            // printf("------------4\n");
            while (start1 < end1 && start2 < end2){
                Process minproc = MinProc(a[start1], a[start2]);
                if (minproc.process_id == a[start1].process_id)
                    b[k++] = a[start1++];
                else 
                    b[k++] = a[start2++];
            }
                // b[k++] = MinProc(a[start1], a[start2]);
            // printf("------------5\n");
            while (start1 < end1)
                b[k++] = a[start1++];
            // printf("------------6\n");
            while (start2 < end2)
                b[k++] = a[start2++];
        }
        Process* tmp = a;
        a = b;
        b = tmp;
    }
    if (a != p) {
        int i;
        for (i = 0; i < num; i++)
            b[i] = a[i];
        b = a;
    }
    free(b);
}

void Calculate(Process* proc, int proc_num){
    proc[0].service_time = proc[0].arrival_time;
    proc[0].waiting_time = 0;
    for(int i=1;i<proc_num;i++){
		proc[i].service_time = proc[i-1].service_time+proc[i-1].execution_time;
        if (proc[i].service_time < proc[i].arrival_time)
            proc[i].service_time = proc[i].arrival_time;
		proc[i].waiting_time = proc[i].service_time-proc[i].arrival_time;
        if(proc[i].waiting_time<0)
			proc[i].waiting_time=0;
	}

	for(int i=0;i<proc_num;i++){
		proc[i].turnaround_time = proc[i].execution_time + proc[i].waiting_time;
	}
	printf("\n\n");
	printf("Process\tExecution Time\tArrival Time\tWaiting Time\tTurnAround Time\tCompletion Time\t Service time\n");
	int total_waiting_time=0,total_turn_around_time=0;
	for(int i=0;i<proc_num;i++){
		total_waiting_time+=proc[i].waiting_time;
		total_turn_around_time+=proc[i].turnaround_time;
		proc[i].completion_time=proc[i].turnaround_time + proc[i].arrival_time;
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",proc[i].process_id,proc[i].execution_time, proc[i].arrival_time, proc[i].waiting_time,proc[i].turnaround_time,proc[i].completion_time, proc[i].service_time);
	}
	printf("Average waiting time: %f\n", (float)total_waiting_time/proc_num);
	printf("Average turnaround time: %f\n\n",(float)total_turn_around_time/proc_num);
}

void Schedule(Process* proc, LinkedQueue** ProcessQueue, int proc_num){
    int time_slice = ProcessQueue[0]->time_slice;
    int slice_used = 0;
    int slice_offset = 0;
    int current_proc = 0;
    int tmp_time = 0;
    Process de_proc;
    do {
        int flag = 0;
        for (int i=0; i<proc_num; i++){
            if (tmp_time == proc[i].arrival_time){
                if (IsEmptyQueue(ProcessQueue[0]))
                    slice_offset = tmp_time % time_slice;
                ProcessQueue[0] = EnQueue(ProcessQueue[0], proc[i]);
           }
            if (tmp_time == proc[i].completion_time){
                de_proc = DeQueue(ProcessQueue[0]);
                flag = 1;
            }
        }
        if (flag == 0){
            if(tmp_time % time_slice == slice_offset && tmp_time != 0){
                if(Length(ProcessQueue[0])){
                    Process front_proc = FrontQueue(ProcessQueue[0]);
                    if (front_proc.arrival_time == tmp_time){}
                    else {
                        outprint(tmp_time-time_slice, tmp_time, front_proc.process_id, front_proc.arrival_time, front_proc.completion_time-tmp_time);
                    }
                }
            }
        }else if (flag == 1) {
            if(tmp_time % time_slice == slice_offset){
                outprint(tmp_time-time_slice, tmp_time, de_proc.process_id, de_proc.arrival_time, de_proc.completion_time-tmp_time);
            }else {
                slice_used = (tmp_time - slice_offset) % time_slice;
                slice_offset = (slice_used + slice_offset) % time_slice;
                outprint(tmp_time-slice_used, tmp_time, de_proc.process_id, de_proc.arrival_time, de_proc.completion_time-tmp_time);
            }
            flag = 0;
        }
        if (tmp_time >= proc[proc_num-1].completion_time) break;
    } while (++tmp_time);
}

void InitOutputFile(){
    FILE* outputfile = fopen("./output.log", "w");
    fclose(outputfile);
}

void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time){
    FILE* outputfile = fopen("./output.log", "a");
    fprintf(outputfile, "Time_slot:%d-%d, pid:%d, arrival-time:%d, remaining_time:%d\n",\
            time_x, time_y, pid, arrival_time, remaining_time);
    fclose(outputfile);          
}
