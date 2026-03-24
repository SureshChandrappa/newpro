#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/device.h>

#define DEVICE_NAME "spinlock_example"
#define CLASS_NAME "spinlck"

static int major_num;
static struct class *device_class =NULL;
static struct device *device = NULL;

struct device_data {
	int counter;
	spinlock_t lock;
};

static struct device_data *dev_data;

// Open device file

static int device_open(struct inode *inode, struct file *file){
	file->private_data = dev_data;
	printk(KERN_INFO "Spinnlock example : Device opened\n");
	return 0;
}

// Read operation - protected by spinlock

static ssize_t device_read(struct file *file , char __user *buffer, size_t length,loff_t *offset){
	struct device_data *data = file->private_data;
	unsigned long flags;
	int counter_value;
	char result[32];
	int len;

	
	if (*offset > 0) {
		return 0;  // EOF - no more data to send
	}
	spin_lock_irqsave(&data->lock,flags);
	counter_value = data->counter;

	spin_unlock_irqrestore(&data->lock,flags);

	len = snprintf(result,sizeof(result),"Counter : %d\n",counter_value);

	if(copy_to_user(buffer,result,len)){
		return -EFAULT;
	}
	*offset = len;
	return len;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t length,loff_t *offset){
	struct device_data *data = file->private_data;
	unsigned long flags;
	char user_input[10];

	if (copy_from_user(user_input, buffer, min(length, sizeof(user_input)-1))) {
		return -EFAULT;
	}
	
	// Acquire spinlock - protect counter modification
	spin_lock_irqsave(&data->lock, flags);
	// Critical section - modify shared counter
	data->counter++;
	printk(KERN_INFO "Spinlock example: counter incremented to %d\n", data->counter);
	// Release spinlock
	spin_unlock_irqrestore(&data->lock, flags);
	
	return length;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.read = device_read,
	.write = device_write,
};


static int __init spinlock_init(void){
	dev_data = kmalloc(sizeof(struct device_data),GFP_KERNEL);
	if(!dev_data){
		printk(KERN_ALERT "Failed to allocate device data\n");
		return -ENOMEM;
	}

	spin_lock_init(&dev_data->lock);

	dev_data->counter =0;

	major_num = register_chrdev(0,DEVICE_NAME,&fops);
	if(major_num < 0){
		printk(KERN_ALERT "Failed to register devuce\n");
		kfree(dev_data);
		return major_num;
	}

	device_class = class_create(CLASS_NAME);
	if (IS_ERR(device_class)){
		unregister_chrdev(major_num,DEVICE_NAME);
		kfree(dev_data);
		return PTR_ERR(device_class);
	}

	device = device_create(device_class,NULL,MKDEV(major_num,0),NULL,DEVICE_NAME);

	if (IS_ERR(device)){
		class_destroy(device_class);
		unregister_chrdev(major_num,DEVICE_NAME);
                kfree(dev_data);
                return PTR_ERR(device);
        }
	printk(KERN_INFO "Spinlock example initialsed with major number %d\n",major_num);
	return 0;

}

static void __exit spinlock_exit(void){
	device_destroy(device_class, MKDEV(major_num, 0));
	class_destroy(device_class);
	unregister_chrdev(major_num, DEVICE_NAME);
	kfree(dev_data);
	printk(KERN_INFO "Spinlock example: cleaned up\n");
}

module_init(spinlock_init);
module_exit(spinlock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("S-K-c");
MODULE_DESCRIPTION("Simple spinlock example for Linux device driver");
