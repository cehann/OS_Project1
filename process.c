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
		for(int i = 0; i < proc.e_time; i++){
		//	fprintf(stderr, "%s %d\n", proc.name, i);
			unit_time();
		}
		syscall(get_time, &proc.end_sec, &proc.end_nsec);
		sprintf(message, "[Project 1] %d %lu.%09lu %lu.%09lu", proc.pid, proc.ini_sec, proc.ini_nsec, proc.end_sec, proc.end_nsec);
		syscall(show_mes, message);
		exit(0);
	}
	set_cpu(pid, child);
	return pid;
}
int stop(int pid){
	struct sched_param pa;
	pa.sched_priority = 0;
	return sched_setscheduler(pid, SCHED_IDLE, &pa);	
}
int keep(int pid, int policy){
	struct sched_param pa;
	if(policy == 0){
		pa.sched_priority = 50;
		int ret = sched_setscheduler(pid, SCHED_FIFO, &pa);
	}	
	return sched_setscheduler(pid, SCHED_OTHER, &pa);
}
