#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#include "process.h"
extern int errno;
int* create_shared_memory(size_t size) {
	_fd = shm_open("osshm", O_CREAT | O_RDWR, S_IRWXU);
	ftruncate(_fd, sizeof(int));
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return (int *)mmap(NULL, size, protection, visibility, _fd, 0);
}
int set_cpu(int pid, int c){
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(c, &mask);
	sched_setaffinity(pid, sizeof(mask), &mask);
	return 0;
}

int execute(struct process proc){
	
	int pid = fork();
	if(pid == 0){
		char message[200];
		proc.pid = getpid();
		syscall(get_time, &proc.ini_sec, &proc.ini_nsec);
        int tot = 0;
		while(proc.e_time != 0){
			if(shm[1] == shm[0]){
				keep(getpid(), 0, 49);
				shm[4] = getpid(); 
				shm[5] = proc.e_time;
				
				keep(shm[2], 0, 51);	
			}
#ifdef DEBUG
			fprintf(stderr, "%s %ld\n", proc.name, proc.e_time);
#endif
			unit_time();
			proc.e_time--;
			shm[0]++;
            tot++;
            if(tot % 500 == 0 && proc.e_time != 0){
                shm[6] = 1;
                keep(getpid(), 0, 49);
                shm[4] = getpid();
                shm[5] = proc.e_time;
                keep(shm[2], 0, 51);
            }
		}
		syscall(get_time, &proc.end_sec, &proc.end_nsec);
		sprintf(message, "[Project 1] %d %lu.%09lu %lu.%09lu", proc.pid, proc.ini_sec, proc.ini_nsec, proc.end_sec, proc.end_nsec);
		syscall(show_mes, message);
		shm[3] = getpid();
		shm[4] = getpid();
		shm[5] = proc.e_time;
		exit(0);
	}
	set_cpu(pid, parent);
	stop(pid);
	return pid;
}
int stop(int pid){
	struct sched_param pa;
	pa.sched_priority = 0;
	return sched_setscheduler(pid, SCHED_IDLE, &pa);	
}
int keep(int pid, int policy, int priority){
	struct sched_param pa;
	pa.sched_priority = priority;
	int ret = sched_setscheduler(pid, SCHED_FIFO, &pa);
	return ret;
}
