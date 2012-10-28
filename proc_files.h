#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/kthread.h>

// #define MAX_PROC_SIZE 1

#define UP "/proc/dm-cache/up"
#define DN "/proc/dm-cache/dn"
#define IN "/home/senior-project/in"
// #define IN "/proc/dm-cache/in"
#define SP "/proc/dm-cache/sp"

long time;
spinlock_t proc_lock;
char *un, *dn, *sp;
struct task_struct *kthread;
// static char in[MAX_PROC_SIZE];
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

/* static int write_proc(struct file *file, const char __user *buffer, unsigned long count, void *data) {
	if (count > MAX_PROC_SIZE)
		count = MAX_PROC_SIZE;
	if (copy_from_user(in, buffer, count))
		return -EFAULT;
	return count;
} */

static int up_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(un, buffer, start, offset, size, eof, data);
}

static int dn_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(dn, buffer, start, offset, size, eof, data);
}

static int sp_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(sp, buffer, start, offset, size, eof, data);
}

/* static int in_read_proc(char *buffer, char **start, off_t offset, int size, int *eof, void *data) {
	return read_proc(in, buffer, start, offset, size, eof, data);
} */

int filp_init(const char *name) {
	struct file *filp;
	int mode = O_RDWR | O_CREAT;
	int permflags = S_IRUSR | S_IWUSR;
	filp = filp_open(name, mode, permflags);
	filp_close(filp, NULL);
	return 0;
}

int set_val(char *name) {
	struct file *filp;
	mm_segment_t old_fs;
	filp = filp_open(name, O_WRONLY, 0);
	old_fs = get_fs();
	set_fs(get_ds());
	vfs_write(filp, "1", 1, &filp->f_pos);
	set_fs(old_fs);
	filp_close(filp, NULL);
	return 0;
}

int clr_val(char *name) {
	struct file *filp;
	mm_segment_t old_fs;
	filp = filp_open(name, O_WRONLY, 0);
	old_fs = get_fs();
	set_fs(get_ds());
	vfs_write(filp, "0", 1, &filp->f_pos);
	set_fs(old_fs);
	filp_close(filp, NULL);
	return 0;
}

int check_time(void *data) {
	while (1) {
		int i;
		char buf[1];
		struct file *fp;
		mm_segment_t old_fs;
		i = 0;
		// printk("elapsed: %lu\n", (long)current_kernel_time().tv_sec - time);
		if (sp[0]&0x0F && ((long)current_kernel_time().tv_sec - time >= 20)) {
			/* block until spun down */
			spin_lock(&proc_lock);
			printk(KERN_INFO "time to spin down disk\n");
			dn = "1";
			clr_val(IN);
			while (1) {
				// printk("check_time iteration: %d\n", ++i);
				fp = filp_open(IN, O_RDONLY, 0);
				old_fs = get_fs();
				set_fs(get_ds());
				vfs_read(fp, buf, 1, &fp->f_pos);
				set_fs(old_fs);
				filp_close(fp, NULL);
				if (buf[0]&0x0F || kthread_should_stop()) break;
				schedule();
			}
			dn = "0", sp = "0";
			clr_val(IN);
			printk(KERN_INFO "disk has spun down\n");
			spin_unlock(&proc_lock);
		}
		if (kthread_should_stop()) break;
		schedule();
	}
	return 0;
}
