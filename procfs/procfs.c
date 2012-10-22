#include <linux/current.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) ((a)*65536+(b)*256+(c))
#endif

// if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
#include <asm/uaccess.h>
// endif

#define MESSAGE_LENGTH 80
static char message[MESSAGE_LENGTH];

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static ssize_t module_output(
	struct file *file,
	char *buf,
	size_t len,
	loff_t *offset)
#else
static int module_output(
	struct inode *inode,
	struct file *file,
	char *buf,
	int len)
#endif
{
	static int finished = 0;
	int i;
	char message[MESSAGE_LENGTH+30];

	if (finished) {
		finished = 0;
		return 0;
	}
	
	sprintf(message, "Last input:%s", message);
	for(i = 0; i < len && message[i]; i++)
		put_user(message[i], buf + i);
	
	finished = 1;

	return i;
}

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,2,0)
static ssize_t module_input(
	struct file *file,
	const char *buf,
	size_t length,
	loff_t *offset)
#else
static int module_input(
	struct inode *inode,
	struct file *file,
	const char *buf,
	int length)
#endif
{
	int i;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	get_user(message[i], buf + i);
#else
	// message[i] = get_user(buf + i);
#endif
	message[i] = '\0';
	
	return i;
}

static int module_permission(struct inode *inode, int op)
{
	if (op == 4 || (op == 2 && current->euid == 0))
		return 0;

	return -EACCES;
}

int module_open(struct inode *inode, struct file *file)
{
	// MOD_INC_USE_COUNT; 
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
int module_close(struct inode *inode, struct file *file)
#else
	void module_close(struct inode *inode, struct file *file)
#endif
{
	// MOD_DEC_USE_COUNT;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	return 0;
#endif
}

static struct file_operations f_op = {
	NULL,
	module_output,
	module_input,
	NULL,
	NULL,
	NULL,
	NULL,
	module_open,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	NULL,
#endif
	module_close,
};

static struct inode_operations inode_ops = {
	&f_ops,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	module_permission
};

static struct proc_dir_entry proc_entry = {
	0,
	7,
	"rw_test",
	S_IFREG | S_IRUGO | S_IWUSR,
	1,
	0, 0,
	80,
	&inode_ops,
	NULL
};

int init_module()
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	return proc_register(&proc_root, &proc_entry);
#else
	return;
	// return proc_register_dynamic(&proc_root, &proc_entry);
#endif
}

void cleanup_module()
{
	proc_unregister(&proc_root, proc_file.low_ino);
}

