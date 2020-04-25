#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_get_time(unsigned long __user *a, unsigned long __user *b){
	struct timespec now;
	getnstimeofday(&now);
	copy_to_user(a, &now.tv_sec, sizeof(unsigned long));
	copy_to_user(b, &now.tv_nsec, sizeof(unsigned long));
	return 1;
}
