#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

asmlinkage int sys_show_mes(char __user *str){
	char message[100];
	copy_from_user(message, str, 100);
	printk("%s\n", message);
	return 1;
}
