// spinlock_fixed1.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/printk.h>

static spinlock_t my_lock;
static int shared_counter = 0;
static struct task_struct *thread1, *thread2;
static int thread1_running = 1, thread2_running = 1;

static int thread_func(void *data)
{
    int i;
    char *name = (char *)data;
    int local_counter;
    
    for (i = 0; i < 10; i++) {
        // Acquire the lock
        spin_lock(&my_lock);
        
        // Critical section - modify shared data
        shared_counter++;
        local_counter = shared_counter;  // Copy for printing after unlock
        
        pr_info("%s: incremented counter to %d\n", name, shared_counter);
        
        // Release the lock
        spin_unlock(&my_lock);
        
        // Simulate work outside the lock
        msleep(100);
        
        // Trylock example - non-blocking attempt
        if (spin_trylock(&my_lock)) {
            pr_info("%s: got lock with trylock, counter = %d\n", 
                    name, shared_counter);
            spin_unlock(&my_lock);
        }
        
        msleep(50);
    }
    
    return 0;
}

static int __init spinlock_fixed_init(void)
{
    pr_info("Fixed Spinlock Example: Loading\n");
    
    // Initialize the spinlock
    spin_lock_init(&my_lock);
    shared_counter = 0;
    
    // Create two kernel threads
    thread1 = kthread_run(thread_func, (void *)"Thread1", "thread1");
    if (IS_ERR(thread1)) {
        pr_err("Failed to create thread1\n");
        return PTR_ERR(thread1);
    }
    
    thread2 = kthread_run(thread_func, (void *)"Thread2", "thread2");
    if (IS_ERR(thread2)) {
        pr_err("Failed to create thread2\n");
        kthread_stop(thread1);
        return PTR_ERR(thread2);
    }
    
    pr_info("Spinlock example: Threads started\n");
    return 0;
}

static void __exit spinlock_fixed_exit(void)
{
    // Stop the threads
    if (thread1 && !IS_ERR(thread1)) {
        kthread_stop(thread1);
    }
    if (thread2 && !IS_ERR(thread2)) {
        kthread_stop(thread2);
    }
    
    pr_info("Fixed Spinlock Example: Unloaded. Final counter = %d\n", 
            shared_counter);
}

module_init(spinlock_fixed_init);
module_exit(spinlock_fixed_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Fixed Spinlock Example");
