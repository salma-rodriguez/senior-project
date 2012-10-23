#include <linux/proc_fs.h>

#define MAX_PROC_SIZE 1

#define UP "/proc/dm-cache/up"
#define DN "/proc/dm-cache/dn"
#define IN "/home/salma/Documents/in"
// #define IN "/proc/dm-cache/in"
#define SP "/proc/dm-cache/sp"

int time;
char *un, *dn, *sp;
static char in[MAX_PROC_SIZE];
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

static int write_proc(struct file *file, const char __user *buffer, unsigned long count, void *data) {
	if (count > MAX_PROC_SIZE)
		count = MAX_PROC_SIZE;
	if (copy_from_user(in, buffer, count))
		return -EFAULT;
	return count;
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

int filp_init(const char *name) {
	struct file *filp;
	int mode = O_RDWR | O_CREAT;
	int permflags = S_IRUSR | S_IWUSR;
	filp = filp_open(name, mode, permflags);
	if (IS_ERR(filp))
		return -EBADF;
	filp_close(filp, NULL);
	return 0;
}

int set_val(char *name) {
	int ret, out_fd;
	struct file *filp;
	mm_segment_t old_fs;

	filp = filp_open(name, O_WRONLY, 0);
	old_fs = get_fs();
	set_fs(get_ds());
	//const char __user *
	/* if ((vfs_write(filp, "1", 1, &filp->f_pos)) < 0) {
		DPRINTK("I/O error");
		ret = -EIO;
	} */
	vfs_write(filp, "1", 1, &filp->f_pos);
	set_fs(old_fs);
	filp_close(filp, NULL);

	return 0;
}

int clr_val(char *name) {
	int ret, out_fd;
	struct file *filp;
	mm_segment_t old_fs;
	filp = filp_open(name, O_WRONLY, 0);
	old_fs = get_fs();
	set_fs(get_ds());
	vfs_write(filp, "0", 1, &filp->f_pos);
	// const char __user *
	/* if ((vfs_write(filp, "0", 1, &filp->f_pos)) < 0) {
		DPRINTK("I/O error");
		ret = -EIO;
	} */
	set_fs(old_fs);
	filp_close(filp, NULL);
	return 0;
}

