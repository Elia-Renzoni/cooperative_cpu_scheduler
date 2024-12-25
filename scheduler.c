/**
 * @file scheduler.c
 * @author your name (you@domain.com)
 * @brief A simple virtual cooperative scheduler built for fun.
 * @version 0.1
 * @date 2024-12-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define TASK_RUNNING         0
#define TASK_WAITING         1
#define TASK_READY           2
#define TASK_TERMINATED      -1
#define MAX_TASKS           10
#define BASE_PID             0
#define STACK_SIZE          256
#define BASE_ADDR           0x0000

/*
* struct designed for tasks
*/
struct task_block 
{
    /*
    * similar to the PID in Linux
    */
    int task_id;
    /*
    * activity of the task
    */
    int task_state;
    /*
    * init memory address of the task
    */
    uint32_t task_address;
    /*
    * running time of the task
    */
    int running_time;
};

/*
*   struct containing the queue
*/
struct scheduler_block
{
    /*
    * 
    */
    struct task_block queue[MAX_TASKS];
    /**
     * queue length
     */
    int tasks;
};

/**
 * @brief create a new task
 * 
 * @param sched 
 * @return void* 
 */
int create_task(struct scheduler_block *sched) 
{
    struct task_block task;
    if (sched->tasks >= MAX_TASKS)
    {
        return 1;
    }

    sched->tasks += 1;
    task.task_id = BASE_PID + sched->tasks + 1;
    task.task_state = TASK_WAITING;
    task.task_address = BASE_ADDR + (256 * sched->tasks);
    task.running_time = 3; // 3 seconds

    sched->queue[sched->tasks] = task;
    printf("Added a New Task to the Scheduler Queue. PID = %d \n", task.task_id);
    return 0;
}

/**
 * @brief change the state in running and execute the process
 * 
 * @param sched 
 * @param task_index 
 */
void terminate_task_exec(struct scheduler_block *sched, int task_index) 
{
    sched->queue[task_index].task_state = TASK_RUNNING;
    
    struct timespec ts;
    ts.tv_sec = 3; 
    ts.tv_nsec = 0; 
    nanosleep(&ts, NULL);

    sched->queue[0].task_state = TASK_TERMINATED;

    printf("The task with PID = %d has completed... \n", sched->queue[task_index].task_id);
}

/**
 * @brief change the states in ready 
 * 
 * @param sched 
 */
void make_task_ready(struct scheduler_block *sched) 
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        sched->queue[i].task_state = TASK_READY;
        printf("The process with PID %d is ready... \n", sched->queue[i].task_id);
    }
}

/**
 * @brief main scheduler routine 
 * 
 * @param sched 
 * @return int 
 */
int fifo_cooperative_scheduler(struct scheduler_block *sched) 
{
    make_task_ready(sched);
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (sched->queue[i].task_state == TASK_READY)
        {
            printf("Scheduled the task with PID = %d \n", sched->queue[i].task_id);
            terminate_task_exec(sched, i);
        }
        else 
            return 1;
    }
    return 0;
}

int main()
{
    struct scheduler_block *sched = (struct scheduler_block*) malloc(sizeof(struct scheduler_block));
    sched->tasks = 0;

    int i = 0;
    while (i < MAX_TASKS)
    {
        int result = create_task(sched);
        if (result)
        {
            printf("The queue is full of tasks!!");
            break;
        }
        i++;
    }

    int scheduling_result = fifo_cooperative_scheduler(sched);
    if (scheduling_result)
    {
        exit(1);
    }

    free(sched);
    return 0;
}