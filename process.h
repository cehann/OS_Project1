#include <sys/types.h>
#define parent 0
#define child 1
void unit_time(){
	volatile unsigned long i; for(i=0;i<1000000UL;i++);
}
struct process{
	char name[10];
	unsigned long s_time, e_time;
	unsigned long ini_sec, ini_nsec, end_sec, end_nsec;
	pid_t pid;
};

int set_cpu(int pid, int c);

int execute(struct process proc);

int waiting(int pid);

int keep(int pid);