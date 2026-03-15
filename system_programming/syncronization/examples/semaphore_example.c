// semaphore_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct semaphore pool_sem;
static int pool_size = 3;
static struct task_struct *threads[5];

static int thread_use_resource(void *data)
{
    int thread_id = *(int *)data;
    
    pr_info("Thread %d: waiting for resource\n", thread_id);
    
    // Try to get a resource from pool
    if (down_interruptible(&pool_sem)) {
        pr_info("Thread %d: interrupted by signal\n", thread_id);
        return -EINTR;
    }
    
    pr_info("Thread %d: acquired resource\n", thread_id);
    
    // Use the resource
    msleep(2000);
    
    // Release the resource
    pr_info("Thread %d: releasing resource\n", thread_id);
    up(&pool_sem);
    
    return 0;
}

static int __init semaphore_init(void)
{
    int i;
    int ids[5] = {1, 2, 3, 4, 5};
    
    pr_info("Semaphore example loaded\n");
    
    // Initialize semaphore with count = pool_size (3)
    sema_init(&pool_sem, pool_size);
    
    // Create 5 threads competing for 3 resources
    for (i = 0; i < 5; i++) {
        threads[i] = kthread_run(thread_use_resource, (void *)&ids[i], 
                                 "thread%d", i);
    }
    
    return 0;
}

static void __exit semaphore_exit(void)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (threads[i])
            kthread_stop(threads[i]);
    }
    pr_info("Semaphore example unloaded\n");
}

module_init(semaphore_init);
module_exit(semaphore_exit);
MODULE_LICENSE("GPL");
