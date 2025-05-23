/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-23     Bernard      the first version
 * 2010-11-10     Bernard      add cleanup callback function in thread exit.
 * 2012-12-29     Bernard      fix compiling warning.
 * 2013-12-21     Grissiom     let rt_thread_idle_excute loop until there is no
 *                             dead thread.
 * 2016-08-09     ArdaFu       add method to get the handler of the idle thread.
 * 2018-02-07     Bernard      lock scheduler to protect tid->cleanup.
 * 2018-07-14     armink       add idle hook list
 * 2018-11-22     Jesven       add per cpu idle task
 *                             combine the code of primary and secondary cpu
 */

#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_MODULE
#include <dlmodule.h>
#endif

#if defined (RT_USING_HOOK)
#ifndef RT_USING_IDLE_HOOK
#define RT_USING_IDLE_HOOK
#endif
#endif

#ifndef IDLE_THREAD_STACK_SIZE
#if defined (RT_USING_IDLE_HOOK) || defined(RT_USING_HEAP)
#define IDLE_THREAD_STACK_SIZE  256
#else
#define IDLE_THREAD_STACK_SIZE  128
#endif
#endif

#ifdef RT_USING_SMP
#define _CPUS_NR                RT_CPUS_NR
#else
#define _CPUS_NR                1
#endif

static rt_list_t _rt_thread_defunct = RT_LIST_OBJECT_INIT(_rt_thread_defunct);;

static struct rt_thread idle[_CPUS_NR];
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_thread_stack[_CPUS_NR][IDLE_THREAD_STACK_SIZE];

#ifndef SYSTEM_THREAD_STACK_SIZE
#define SYSTEM_THREAD_STACK_SIZE IDLE_THREAD_STACK_SIZE
#endif
static struct rt_thread rt_system_thread;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_system_stack[SYSTEM_THREAD_STACK_SIZE];
static struct rt_semaphore system_sem;

#ifdef RT_USING_IDLE_HOOK
#ifndef RT_IDLE_HOOK_LIST_SIZE
#define RT_IDLE_HOOK_LIST_SIZE  4
#endif

static void (*idle_hook_list[RT_IDLE_HOOK_LIST_SIZE])(void);

/**
 * @ingroup Hook
 * This function sets a hook function to idle thread loop. When the system performs
 * idle loop, this hook function should be invoked.
 *
 * @param hook the specified hook function
 *
 * @return RT_EOK: set OK
 *         -RT_EFULL: hook list is full
 *
 * @note the hook function must be simple and never be blocked or suspend.
 */
rt_err_t rt_thread_idle_sethook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;
    rt_err_t ret = -RT_EFULL;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == RT_NULL)
        {
            idle_hook_list[i] = hook;
            ret = RT_EOK;
            break;
        }
    }
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return ret;
}

/**
 * delete the idle hook on hook list
 *
 * @param hook the specified hook function
 *
 * @return RT_EOK: delete OK
 *         -RT_ENOSYS: hook was not found
 */
rt_err_t rt_thread_idle_delhook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;
    rt_err_t ret = -RT_ENOSYS;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == hook)
        {
            idle_hook_list[i] = RT_NULL;
            ret = RT_EOK;
            break;
        }
    }
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return ret;
}

#endif

#ifdef RT_USING_MODULE
/* Return whether there is defunctional thread to be deleted. */
rt_inline int _has_defunct_thread(void)
{
    /* The rt_list_isempty has prototype of "int rt_list_isempty(const rt_list_t *l)".
     * So the compiler has a good reason that the _rt_thread_defunct list does
     * not change within rt_thread_defunct_exceute thus optimize the "while" loop
     * into a "if".
     *
     * So add the volatile qualifier here. */
    const volatile rt_list_t *l = (const volatile rt_list_t *)&_rt_thread_defunct;

    return l->next != l;
}
#endif

/* enqueue a thread to defunct queue
 * it must be called between rt_hw_interrupt_disable and rt_hw_interrupt_enable
 */
void rt_thread_defunct_enqueue(rt_thread_t thread)
{
    rt_list_insert_after(&_rt_thread_defunct, &thread->tlist);
    rt_sem_release(&system_sem);
}

/* dequeue a thread from defunct queue
 * it must be called between rt_hw_interrupt_disable and rt_hw_interrupt_enable
 */
rt_thread_t rt_thread_defunct_dequeue(void)
{
    rt_thread_t thread = RT_NULL;
    rt_list_t *l = &_rt_thread_defunct;

    if (l->next != l)
    {
        thread = rt_list_entry(l->next,
                struct rt_thread,
                tlist);
        rt_list_remove(&(thread->tlist));
    }
    return thread;
}

/**
 * @ingroup Thread
 *
 * This function will perform system background job when system idle.
 */
static void rt_defunct_execute(void)
{
    /* Loop until there is no dead thread. So one call to rt_defunct_execute
     * will do all the cleanups. */
    while (1)
    {
        rt_base_t lock;
        rt_thread_t thread;
        void (*cleanup)(struct rt_thread *tid);

#ifdef RT_USING_MODULE
        struct rt_dlmodule *module = RT_NULL;
#endif
        RT_DEBUG_NOT_IN_INTERRUPT;

        /* disable interrupt */
        lock = rt_hw_interrupt_disable();

#ifdef RT_USING_MODULE
        /* check whether list is empty */
        if (!_has_defunct_thread())
        {
            rt_hw_interrupt_enable(lock);
            break;
        }
        /* get defunct thread */
        thread = rt_list_entry(_rt_thread_defunct.next,
                struct rt_thread,
                tlist);
        module = (struct rt_dlmodule*)thread->module_id;
        if (module)
        {
            dlmodule_destroy(module);
        }
        /* remove defunct thread */
        rt_list_remove(&(thread->tlist));
#else
        thread = rt_thread_defunct_dequeue();
        if (!thread)
        {
            rt_hw_interrupt_enable(lock);
            break;
        }
#endif
        /* invoke thread cleanup */
        cleanup = thread->cleanup;
        if (cleanup != RT_NULL)
        {
            rt_hw_interrupt_enable(lock);
            cleanup(thread);
            lock = rt_hw_interrupt_disable();
        }

#ifdef RT_USING_SIGNALS
        rt_thread_free_sig(thread);
#endif

        /* if it's a system object, not delete it */
        if (rt_object_is_systemobject((rt_object_t)thread) == RT_TRUE)
        {
            /* detach this object */
            rt_object_detach((rt_object_t)thread);
            /* enable interrupt */
            rt_hw_interrupt_enable(lock);
        }
        else
        {
            rt_hw_interrupt_enable(lock);
#ifdef RT_USING_HEAP
            /* release thread's stack */
            RT_KERNEL_FREE(thread->stack_addr);
            /* delete thread object */
            rt_object_delete((rt_object_t)thread);
#endif
        }
    }
}

extern void rt_system_power_manager(void);
static void rt_thread_idle_entry(void *parameter)
{
#ifdef RT_USING_SMP
    if (rt_hw_cpu_id() != 0)
    {
        while (1)
        {
            rt_hw_secondary_cpu_idle_exec();
        }
    }
#endif

    while (1)
    {
#ifdef RT_USING_IDLE_HOOK
        rt_size_t i;

        for (i = 0; i < RT_IDLE_HOOK_LIST_SIZE; i++)
        {
            if (idle_hook_list[i] != RT_NULL)
            {
                idle_hook_list[i]();
            }
        }
#endif

#ifndef RT_USING_SMP
#endif

#ifdef RT_USING_PM
        rt_system_power_manager();
#endif
    }
}

static void rt_thread_system_entry(void *parameter)
{
    while (1)
    {
        rt_sem_take(&system_sem, RT_WAITING_FOREVER);
        rt_defunct_execute();
    }
}

/**
 * @ingroup SystemInit
 *
 * This function will initialize idle thread, then start it.
 *
 * @note this function must be invoked when system init.
 */
void rt_thread_idle_init(void)
{
    rt_ubase_t i;
    char tidle_name[RT_NAME_MAX];

    for (i = 0; i < _CPUS_NR; i++)
    {
        rt_sprintf(tidle_name, "tidle%d", i);
        rt_thread_init(&idle[i],
                tidle_name,
                rt_thread_idle_entry,
                RT_NULL,
                &rt_thread_stack[i][0],
                sizeof(rt_thread_stack[i]),
                RT_THREAD_PRIORITY_MAX - 1,
                32);
#ifdef RT_USING_SMP
        rt_thread_control(&idle[i], RT_THREAD_CTRL_BIND_CPU, (void*)i);
#endif
        /* startup */
        rt_thread_startup(&idle[i]);
    }

    RT_ASSERT(RT_THREAD_PRIORITY_MAX > 2);

    rt_sem_init(&system_sem, "defunct", 1, RT_IPC_FLAG_FIFO);

    /* create defunct thread */
    rt_thread_init(&rt_system_thread,
            "tsystem",
            rt_thread_system_entry,
            RT_NULL,
            rt_system_stack,
            sizeof(rt_system_stack),
            RT_THREAD_PRIORITY_MAX - 2,
            32);
    /* startup */
    rt_thread_startup(&rt_system_thread);
}

/**
 * @ingroup Thread
 *
 * This function will get the handler of the idle thread.
 *
 */
rt_thread_t rt_thread_idle_gethandler(void)
{
#ifdef RT_USING_SMP
    register int id = rt_hw_cpu_id();
#else
    register int id = 0;
#endif

    return (rt_thread_t)(&idle[id]);
}
