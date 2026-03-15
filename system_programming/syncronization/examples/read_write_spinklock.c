// rwspinlock_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static rwlock_t my_rwlock;
static int protected_array[10];
static int array_size = 10;
static struct task_struct *reader1, *reader2, *writer;

static int reader_func(void *data)
{
    int i, reader_id = *(int *)data;
    unsigned long flags;
    
    for (i = 0; i < 5; i++) {
        // Multiple readers can acquire this lock simultaneously
        read_lock_irqsave(&my_rwlock, flags);
        
        pr_info("Reader %d: reading array [", reader_id);
        for (int j = 0; j < array_size; j++) {
            pr_cont("%d ", protected_array[j]);
        }
        pr_cont("]\n");
        
        read_unlock_irqrestore(&my_rwlock, flags);
        
        msleep(50);
    }
    return 0;
}

static int writer_func(void *data)
{
    int i;
    unsigned long flags;
    
    for (i = 0; i < 3; i++) {
        // Writers need exclusive access
        write_lock_irqsave(&my_rwlock, flags);
        
        pr_info("Writer: modifying array\n");
        for (int j = 0; j < array_size; j++) {
            protected_array[j] = j * (i + 1);
        }
        
        write_unlock_irqrestore(&my_rwlock, flags);
        
        msleep(200);
    }
    return 0;
}

static int __init rwspinlock_init(void)
{
    int i, id1 = 1, id2 = 2;
    
    pr_info("RW Spinlock example loaded\n");
    rwlock_init(&my_rwlock);
    
    // Initialize array
    for (i = 0; i < array_size; i++) {
        protected_array[i] = i;
    }
    
    reader1 = kthread_run(reader_func, (void *)&id1, "reader1");
    reader2 = kthread_run(reader_func, (void *)&id2, "reader2");
    writer = kthread_run(writer_func, NULL, "writer");
    
    return 0;
}

static void __exit rwspinlock_exit(void)
{
    kthread_stop(reader1);
    kthread_stop(reader2);
    kthread_stop(writer);
    pr_info("RW Spinlock example unloaded\n");
}

module_init(rwspinlock_init);
module_exit(rwspinlock_exit);
MODULE_LICENSE("GPL");
