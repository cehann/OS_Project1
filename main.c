#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#include "process.h"
int policy, number;
char policy_name[10];
char message[200];
int now_t = 0, finish_cnt;
struct process p[200];
void get_info(){
	scanf("%s", policy_name);
 	scanf("%d", &number);
	for(int i = 0; i < number; i++)
		scanf("%s%ld%ld", p[i].name, &p[i].s_time, &p[i].e_time);
	if(strcmp(policy_name, "FIFO") == 0) policy = 0;
	if(strcmp(policy_name, "RR") == 0) policy = 1;
	if(strcmp(policy_name, "SJF") == 0) policy = 2;
	if(strcmp(policy_name, "PSJF") == 0) policy = 3;

#ifdef DEBUG
	printf("%d\n", number);
	for(int i = 0; i < number; i++)
		printf("%s %ld %ld\n", p[i].name, p[i].s_time, p[i].e_time);
#endif 
}
int cmp(const void* a, const void *b){
	return ((struct process *)a)->s_time > ((struct process *)b)->s_time;
}
void FIFO(){
	qsort(p, number, sizeof(struct process), cmp);
#ifdef DEBUG
	//printf("%d\n", number);
	for(int i = 0; i < number; i++)
		printf("%s %ld %ld\n", p[i].name, p[i].s_time, p[i].e_time);
#endif
	int running = -1; 
	while(1){
		if(running != -1 && p[running].e_time == 0){
			waitpid(p[running].pid, NULL, 0);
			finish_cnt++;
			printf("%s %d\n",p[running].name, p[running].pid);
			running = -1;
			if(finish_cnt == number) 
				break;
		}
		for(int i = 0; i < number; i++){
			if(p[i].s_time == now_t){
				p[i].pid = execute(p[i]);
				stop(p[i].pid);
			}
		}
		if(running == -1){
			for(int i = 0; i < number; i++){
				if(p[i].e_time == 0) continue;
				else {
					running = i;		
					keep(p[running].pid, policy);
#ifdef DEBUG
					for(int j = 0; j < number; j++)
						fprintf(stderr, "%d is %d\n", p[j].pid, sched_getscheduler(p[j].pid));
#endif
					break;
				}
			}
		}
		unit_time();
		if(running != -1){
			//fprintf(stderr, "%s %ld\n", p[running].name, p[running].e_time);
			p[running].e_time--;
		}
		now_t++;
	}
}
//unsigned long ini_sec, ini_nsec;
int main(){
	get_info();
#ifdef SYSCALL
	syscall(get_time, &ini_sec, &ini_nsec);
	sprintf(message, "[Project1] %d %lu.%09lu", 3, ini_sec, ini_nsec);
	printf("%s\n", message);	
	syscall(show_mes, message);
#endif 
	fprintf(stderr, "I am on %d\n", sched_getcpu());
	set_cpu(getpid(), parent);
	keep(getpid(), 0);
	if(policy == 0){
		FIFO();
	}
	return 0;
}
