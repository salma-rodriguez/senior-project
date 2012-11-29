#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>

#define TIME_OUT 60
#define UP "/proc/dm-cache/up"
#define DN "/proc/dm-cache/dn"
#define IN "/proc/dm-cache/in"
#define SP "/proc/dm-cache/sp"

#define PROC_INIT() ({								\
	sp = "1", in = un = dn = "0";						\
	proc_parent = proc_mkdir("dm-cache", NULL);				\
	time = current_kernel_time().tv_sec;					\
	proc_input_entry = create_proc_entry("in", 0666, proc_parent);		\
	proc_input_entry->read_proc = (read_proc_t *)in_read_proc;		\
	proc_input_entry->write_proc = (write_proc_t *)write_proc;		\
	proc_input_entry->mode = S_IFREG | S_IRUGO | S_IWUSR;			\
	proc_input_entry->uid = 0;						\
	proc_input_entry->size = 1;						\
	create_proc_read_entry("up", 0, proc_parent, up_read_proc, NULL);	\
	create_proc_read_entry("dn", 0, proc_parent, dn_read_proc, NULL);	\
	create_proc_read_entry("sp", 0, proc_parent, sp_read_proc, NULL);	\
	spin_lock_init(&proc_lock);						\
	kthread = kthread_run(&check_time, NULL, "t_watchdog");			\
})

#define PROC_EXIT() 								\
	kthread_stop(kthread);							\
	remove_proc_entry("up", proc_parent);					\
	remove_proc_entry("dn", proc_parent);					\
	remove_proc_entry("sp", proc_parent);					\
	remove_proc_entry("in", proc_parent);					\
	remove_proc_entry("dm-cache", NULL);					\
})

long time;
spinlock_t proc_lock;
char *un, *dn, *sp, *in;
struct task_struct *kthread;
struct proc_dir_entry *proc_parent;
struct proc_dir_entry *proc_input_entry;

static int read_proc(char *str, char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	int len = strlen(str);
	if (size < len)
		return -EINVAL;
	if (offset != 0)
		return 0;
	strcpy(buffer, str);
	*eof = 1;
	return len;
}

static int write_proc(struct file *file, const char __user *buffer, unsigned long len, void *data) {
	in = "1";
	return len;
}

static int up_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(un, buffer, start, offset, size, eof, data);
}

static int dn_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(dn, buffer, start, offset, size, eof, data);
}

static int sp_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(sp, buffer, start, offset, size, eof, data);
}

static int in_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(in, buffer, start, offset, size, eof, data);
}

void handle_cache_miss(void) {
	/* block until disk is spinning */
	spin_lock(&proc_lock);
	if (!(sp[0]&0x0F)) {
		printk(KERN_INFO "time to spin up disk!\n");
		un = "1";
		in = "0";
		while (1) {
			if (in[0]&0x0F) break;
			msleep(100);
		}
		un = "0", sp = "1";
		in = "0";
		printk(KERN_INFO "disk has spun up\n");
	}
	spin_unlock(&proc_lock);
}

int check_time(void *data) {
	while (1) {
		msleep(1000 * TIME_OUT);
		spin_lock(&proc_lock);
		if (sp[0]&0x0F && ((long)current_kernel_time().tv_sec - time >= TIME_OUT) {
			dn = "1";
			in = "0";
			while (1) {
				if (in[0]&0x0F || kthread_should_stop()) break;
				msleep(100); /* sleep until spun down */
			}
			dn = "0", sp = "0";
			in = "0";
			printk(KERN_INFO "disk has spun down\n");
		}
		spin_unlock(&proc_lock);
		if (kthread_should_stop()) break;
	}
	return 0;
}
