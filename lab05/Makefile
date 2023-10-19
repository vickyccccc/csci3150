CC=gcc

FIFOScheduler: fifo-scheduler-exec.o  linkedlist.o queue.o
		$(CC) -o FIFOScheduler  fifo-scheduler-exec.o linkedlist.o queue.o

fifo-scheduler-exec.o: csci3150_tut2.c
		$(CC) -c -o fifo-scheduler-exec.o csci3150_tut2.c

queue.o: queue.c
		$(CC) -c -o queue.o queue.c

linkedlist.o: linkedlist.c
		$(CC) -c -o linkedlist.o linkedlist.c

clean:
		rm *.o & rm FIFOScheduler