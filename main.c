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
int fs;
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
/*
	printf("%d\n", number);
	for(int i = 0; i < number; i++)
		printf("%s %ld %ld\n", p[i].name, p[i].s_time, p[i].e_time);
*/
}

int cmp(const void* a, const void *b){
	return ((struct process *)a)->s_time > ((struct process *)b)->s_time;
}
int cmp_for_SJF(const void *a, const void *b){
    struct process* _a = (struct process*)a;
    struct process* _b = (struct process*)b;
    if(_a->s_time == _b->s_time){
        return _a->e_time > _b->e_time;
    }
    return _a->s_time > _b->s_time;
}
int cmp_for_PSJF(const void *a, const void *b){
    struct process* _a = (struct process*) a;
    struct process* _b = (struct process*) b;
    if(_a->e_time == 0)
        return _a->e_time < _b->e_time;
    if(_b->e_time == 0)
        return _b->e_time > _a->e_time;
    return _a->e_time > _b->e_time;
}
int nxt(int pre){
    int ret = -1;
	if(policy == 0 || policy == 2 || policy == 3){
		for(int i = 0; i < number; i++){
            if(p[i].alive == 1){
                ret = i;
                break;
            }
		}
	}
    else if(policy == 1){
        if(pre == -1){
            for(int i = 0; i < number; i++){
                if(p[i].alive == 1){
                    ret = i;
                    break;
                }
            }
        }
        else{
            for(int i = 1; i <= number; i++){
                if(p[(pre+i) % number].alive == 1){
                    ret = (pre + i) % number;
                    break;
                }
            }
        }
    }
    return ret;
}
void update(){
    if(shm[4] != 0){
        int id = shm[4];
        for(int i = 0; i < number; i++){
            if(p[i].pid == id){
                p[i].e_time = shm[5];
                shm[4] = shm[5] = 0;
                break;
            }
        }
    }
    return;
}
void do_task(){
    if(policy == 2 || policy == 3)
       qsort(p, number, sizeof(struct process), cmp_for_SJF);
    else
	    qsort(p, number, sizeof(struct process), cmp);
/*
	for(int i = 0; i < number; i++)
		printf("%s %ld %ld\n", p[i].name, p[i].s_time, p[i].e_time);
*/

	int oncore = -1; 
	while(1){
        update();
        if(shm[3] != 0){
            waitpid(shm[3], NULL, 0);
            int death = 0;
            for(int i = 0; i < number; i++){
                if(p[i].pid == shm[3]){
                    p[i].alive = -1;
                    death = i;
                    break;
                }
            }
            fs++;
            printf("%s %d\n", p[death].name, shm[3]);
			fflush(stdout);
            shm[3] = 0;
            oncore = -1;
            if(fs == number)
                break;
        }
        int last = -1;
		for(int i = 0; i < number; i++){
			if(p[i].alive == 0 && p[i].s_time == shm[0]){
				p[i].pid = execute(p[i]);
                p[i].alive = 1;
                last = i;
			}
		}
        update();
        if(last != -1){
            if(last != number - 1){
                shm[1] = p[last + 1].s_time;
            }
            else
                shm[1] = -1;
            if(policy == 3){
                qsort(p, last + 1, sizeof(struct process), cmp_for_PSJF);
                oncore = -1;
            } 
        }
        if(policy == 1){
            if(oncore == -1){
                oncore = nxt(oncore);
            }
            else if(shm[6] == 1){
               oncore = nxt(oncore);
               shm[6] = 0;
            }
            if(oncore != -1){
                keep(getpid(), 0, 49);
                keep(p[oncore].pid, policy, 50);
            }
        }
        else {
            if(oncore == -1){
                oncore = nxt(oncore);
                if(oncore != -1){
                    keep(getpid(), 0, 49);
                    keep(p[oncore].pid, policy, 50);
                }
            }
            else{
                keep(getpid(), 0, 49);
                keep(p[oncore].pid, policy ,50);
            }
        }
        if(oncore == -1){
			//fprintf(stderr, "I am running %d\n", shm[0]);
		    unit_time();
            shm[0]++;
        }
	}
}
int main(){
	get_info();
	set_cpu(getpid(), parent);
	keep(getpid(), 0, 49);
    shm = create_shared_memory(1024);
    shm[2] = getpid();
    do_task();
	return 0;
}
