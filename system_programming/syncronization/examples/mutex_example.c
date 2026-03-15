// mutex_example.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

static DEFINE_MUTEX(my_mutex);
static struct list_head my_list;
static struct task_struct *producer, *consumer;

struct my_node {
    int data;
    struct list_head list;
};

static int producer_func(void *data)
{
    int i;
    struct my_node *node;
    
    for (i = 0; i < 5; i++) {
        node = kmalloc(sizeof(*node), GFP_KERNEL);
        node->data = i;
        
        mutex_lock(&my_mutex);
        list_add_tail(&node->list, &my_list);
        pr_info("Producer: added %d to list\n", i);
        mutex_unlock(&my_mutex);
        
        msleep(100);
    }
    return 0;
}

static int consumer_func(void *data)
{
    int i;
    struct my_node *node, *tmp;
    
    for (i = 0; i < 5; i++) {
        mutex_lock(&my_mutex);
        
        if (!list_empty(&my_list)) {
            node = list_first_entry(&my_list, struct my_node, list);
            list_del(&node->list);
            pr_info("Consumer: removed %d from list\n", node->data);
            kfree(node);
        } else {
            pr_info("Consumer: list empty\n");
        }
        
        mutex_unlock(&my_mutex);
        
        // Trylock example
        if (mutex_trylock(&my_mutex)) {
            pr_info("Consumer: got mutex with trylock\n");
            mutex_unlock(&my_mutex);
        }
        
        msleep(150);
    }
    
    // Cleanup remaining nodes
    mutex_lock(&my_mutex);
    list_for_each_entry_safe(node, tmp, &my_list, list) {
        list_del(&node->list);
        kfree(node);
    }
    mutex_unlock(&my_mutex);
    
    return 0;
}

static int __init mutex_example_init(void)
{
    pr_info("Mutex example loaded\n");
    mutex_init(&my_mutex);
    INIT_LIST_HEAD(&my_list);
    
    producer = kthread_run(producer_func, NULL, "producer");
    consumer = kthread_run(consumer_func, NULL, "consumer");
    
    return 0;
}

static void __exit mutex_example_exit(void)
{
    kthread_stop(producer);
    kthread_stop(consumer);
    pr_info("Mutex example unloaded\n");
}

module_init(mutex_example_init);
module_exit(mutex_example_exit);
MODULE_LICENSE("GPL");
