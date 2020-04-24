#define parent 0
#define child 1
#define show_mes 333
#define get_time 334
#define unit_time() \
{ \
	volatile unsigned long i; for(i=0;i<1000000UL;i++); \
} \

struct process{
	char name[10];
	unsigned long s_time, e_time;
	unsigned long ini_sec, ini_nsec, end_sec, end_nsec;
	pid_t pid;
};
int set_cpu(int pid, int c);

int execute(struct process proc);

int stop(int pid);

int keep(int pid, int policy);
