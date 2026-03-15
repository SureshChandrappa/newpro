// atomic_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static atomic_t counter;
static struct task_struct *thread1, *thread2;

static int thread_func(void *data)
{
    int i;
    char *name = (char *)data;
    
    for (i = 0; i < 10; i++) {
        // Basic atomic operations
        atomic_inc(&counter);
        pr_info("%s: counter = %d\n", name, atomic_read(&counter));
        
        // Atomic add and return
        int old = atomic_fetch_add(5, &counter);
        pr_info("%s: added 5, old value = %d, new = %d\n", 
                name, old, atomic_read(&counter));
        
        // Atomic compare and exchange
        int expected = atomic_read(&counter);
        int new = expected + 3;
        if (atomic_cmpxchg(&counter, expected, new) == expected) {
            pr_info("%s: CAS succeeded, counter = %d\n", name, atomic_read(&counter));
        }
        
        msleep(100);
    }
    return 0;
}

static int __init atomic_init(void)
{
    pr_info("Atomic example loaded\n");
    atomic_set(&counter, 0);
    
    thread1 = kthread_run(thread_func, (void *)"Thread1", "thread1");
    thread2 = kthread_run(thread_func, (void *)"Thread2", "thread2");
    
    return 0;
}

static void __exit atomic_exit(void)
{
    kthread_stop(thread1);
    kthread_stop(thread2);
    pr_info("Atomic example unloaded, final counter = %d\n", atomic_read(&counter));
}

module_init(atomic_init);
module_exit(atomic_exit);
MODULE_LICENSE("GPL");
