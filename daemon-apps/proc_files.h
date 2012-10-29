#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#define TIME_OUT 60
#define UP "/proc/dm-cache/up"
#define DN "/proc/dm-cache/dn"
#define OT "/proc/dm-cache/ot"
#define SP "/proc/dm-cache/sp"

#define PROC_INIT() ({								\
	sp = "1", ot = un = dn = "0";						\
	proc_parent = proc_mkdir("dm-cache", NULL);				\
	create_proc_read_entry("up", 0, proc_parent, up_read_proc, NULL);	\
	create_proc_read_entry("dn", 0, proc_parent, dn_read_proc, NULL);	\
	create_proc_read_entry("sp", 0, proc_parent, sp_read_proc, NULL);	\
	proc_input_entry = create_proc_entry("ot", 0666, proc_parent);		\
	proc_input_entry->read_proc = (read_proc_t *)ot_read_proc;		\
	proc_input_entry->write_proc = (write_proc_t *)write_proc;		\
	proc_input_entry->mode = S_IFREG | S_IRUGO | S_IWUSR;			\
	proc_input_entry->uid = 0;						\
	proc_input_entry->size = 1;						\
	time = current_kernel_time().tv_sec;					\
	spin_lock_init(&proc_lock);						\
	kthread = kthread_run(&check_time, NULL, "t_watchdog");			\
})

long time;
spinlock_t proc_lock;
char *un, *dn, *sp, *ot;
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
	ot = "1";
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

static int ot_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(ot, buffer, start, offset, size, eof, data);
}

void handle_cache_miss(void) {
	/* block until we are sure the disk is spinning */
	if (!(sp[0]&0x0F)) {
		spin_lock(&proc_lock);
		printk(KERN_INFO "time to spin up disk!\n");
		un = "1";
		ot = "0";
		while (1) {
			// printk(KERN_INFO "cache_miss iteration: %d\n", ++i);
			if (ot[0]&0x0F) break;
			schedule();
		}
		un = "0", sp = "1";
		ot = "0";
		// clr_val(IN);
		printk(KERN_INFO "disk has spun up\n");
		spin_unlock(&proc_lock);
	}
}

int check_time(void *data) {
	while (1) {
		if (sp[0]&0x0F && ((long)current_kernel_time().tv_sec - time >= TIME_OUT)) {
			/* block until spun down */
			spin_lock(&proc_lock);
			printk(KERN_INFO "time to spin down disk\n");
			dn = "1";
			ot = "0";
			while (1) {
				// printk("check_time iteration: %d\n", ++i);
				if (ot[0]&0x0F || kthread_should_stop()) break;
				schedule();
			}
			dn = "0", sp = "0";
			ot = "0";
			printk(KERN_INFO "disk has spun down\n");
			spin_unlock(&proc_lock);
		}
		if (kthread_should_stop()) break;
		schedule();
	}
	return 0;
}
