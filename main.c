#include <stdio.h>
#include <string.h>
void unit_time(){
	volatile unsigned long i; for(i=0;i<1000000UL;i++);
}
int policy, number, s_time[15], e_time[15];
char process_name[15][35], policy_name[10];
char message[200];
void get_info(){
	scanf("%s", policy_name);
	if(strcmp(policy_name, "FIFO") == 0) policy = 0;
	scanf("%d", &number);
	for(int i = 0; i < number; i++)
		scanf("%s%d%d", process_name[i], &s_time[i], &e_time[i]);
	printf("%d\n", number);
	for(int i = 0; i < number; i++)
	printf("%s %d %d\n", process_name[i], s_time[i], e_time[i]);
}
int main(){
	unsigned long ini_sec, ini_nsec;
	get_time(ini_sec, ini_nsec);
	sprintf(message, "[Project1] %d %lu.%09lu", 3, ini_sec, ini_nsec);
	get_info(message);
	return 0;
}